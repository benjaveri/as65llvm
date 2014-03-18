package com.bronzecastle.as65

import org.junit.Test
import org.junit.Assert._

@Test
class ParseTest {
  @Test
  def testValues() {
    val E = new Evaluator
    val x = 123; E.symtab += "x"->x
    val y = 456; E.symtab += "y"->y

    class TestCase(validate: ()=>Unit) { def run() = validate() }
    case class NoParseCase(input: String) extends TestCase(()=>{ Grammar.parseExpr(input).right.foreach(v=>assert(false,"Should not parse")) })
    case class BadEvalCase(input: String) extends TestCase(()=>Grammar.parseExpr(input) match {
      case Left(e) => fail(e.toString)
      case Right(v) => E.evaluate(v) match {
        case Left(t) => assert(true)
        case Right(out) => assert(false,"Should not evaluate")
      }
    })
    case class EvalCase(input: String,result: BigInt) extends TestCase(()=>Grammar.parseExpr(input) match {
      case Left(e) => fail(e.toString)
      case Right(v) => E.evaluate(v) match {
        case Left(t) => assert(false,t._1)
        case Right(out) => assert(out == result, "input: "+input+", output: "+out)
      }
    })

    val tests = Seq(
      EvalCase("123",123),
      EvalCase("+123",123),
      EvalCase("-123",-123),
      EvalCase("--123",123),
      EvalCase("1200+34",1234),
      EvalCase("1268-34",1234),
      EvalCase("1268+-34",1234),
      EvalCase("1+2-3",0),
      EvalCase("2*6/3",4),
      BadEvalCase("2/0"),
      EvalCase("5%2",1),
      EvalCase("2+2*3",8),
      EvalCase("(2+2)*3",12),
      EvalCase("-(2+2)*3",-12),
      EvalCase("-(2+2)*-3",12),
      EvalCase("x+y",x+y),
      EvalCase("x-y",x-y),
      EvalCase("x*y",x*y),
      BadEvalCase("z")
    )
    tests.foreach(_.run())
  }
}
