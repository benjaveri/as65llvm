package com.bronzecastle.as65

import scala.util.parsing.combinator.RegexParsers
import scala.util.parsing.input.Positional
import scala.collection.JavaConversions._
import java.io.File

object Main {
  val usage =
    """
      |Usage:
      |  java -r as65.jar [options] input+
      |
      |Options:
      |  -o <filename>    - output filename
      |  -llvm <path>     - path to llvm tools
      |
    """.stripMargin
  object AST {
    trait Node extends Positional
    case class Path(name: String) extends Node
    case class Input(path: Path) extends Node
    case class Output(path: Path) extends Node
    case class LlvmPath(path: Path) extends Node
  }
  object Grammar extends RegexParsers {
    override protected val whiteSpace = "\\s+".r

    lazy val commandline = rep(switch | input) <~ "$".r
    lazy val input = positioned(path ^^ { AST.Input })
    lazy val switch = positioned(
      "-o" ~> path ^^ { AST.Output } |
      "-llvm" ~> path ^^ { AST.LlvmPath }
    )

    lazy val path = arg ^^ { AST.Path }
    lazy val arg =
      "('.*?')".r ^^ { s=>s.drop(1).dropRight(1) } |
      "(\\S+)".r

    def parse(cl: String): Either[Grammar.NoSuccess,List[AST.Node]] = {
      super.parse(commandline,cl) match {
        case Success(result,next) => Right(result)
        case e: Error => Left(e)
        case f: Failure => Left(f)
      }
    }
  }

  def main(args : Array[String]) {
    val arg = args.map(a=>if (a.contains(' ')) "'"+a+"'" else a).foldLeft("")((a,i)=>a+" "+i)
    val rv = invoke(arg)
    if (rv != 0) System.exit(rv)
  }

  def invoke(arg: String): Int = {
    Grammar.parse(arg) match {
      case Left(e) => {
        println(e.toString)
        return 1
      }
      case Right(cl) => {
        // extract command line arguments
        var llvm: Option[LLVM] = None
        var output: Option[String] = None
        var input = List[String]()
        cl.foreach({
          case AST.Input(i) => input = i.name :: input
          case AST.Output(o) => {
            output.foreach(x=>{ println("More than one output specified"); return 2 })
            output = Some(o.name)
          }
          case AST.LlvmPath(l) => {
            llvm.foreach(x=>{ println("More than one llvm path specified"); return 3 })
            llvm = Some(new LLVM(Some(l.name)))
          }
        })

        // validation
        if (input.isEmpty) { println("No input files specified"); return 4 }

        // defaults
        if (output.isEmpty) output = Some(input(0).replaceAll("\\.[^.]*$", "")) // drop extension
        if (llvm.isEmpty) llvm = Some(new LLVM(None))

        // create global program context
        val prg = new Program(new File(output.get),llvm.get)
        try {
          if (prg.readAll(input)) {
            // PASS 1
            // TODO: here

            // PASS 2
            // TODO: here

            // emit IR
            prg.emitPreamble()
            // TODO: here
            prg.emitEpilogue()
            prg.close()
            prg.compile()
          }
        } finally {
          prg.close()
          prg.cleanup()
        }

        0
      }
    }
  }
}
