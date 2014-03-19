package com.bronzecastle.as65

import org.junit.Test
import org.junit.Assert._

@Test
class ValueTest {
  @Test
  def testValues() {
    val P = new Evaluator
    val x = 123; P.symtab += "x"->x
    val y = 456; P.symtab += "y"->y
    val E = new Evaluator
    E.parent = Some(P)
    val dx = 100; E.symtab += ".x"->dx
    val dy = 300; E.symtab += ".y"->dy

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
      NoParseCase("nop"), // reserved word
      EvalCase("123",123),
      EvalCase("$100",256),
      EvalCase("$Aa",170),
      NoParseCase("$ 1001"),
      EvalCase("%1001",9),
      NoParseCase("% 1001"),
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
      EvalCase(".x+y",dx+y),
      EvalCase("x-.y",x-dy),
      EvalCase(".x*y",dx*y),
      EvalCase("x+y",x+y),
      EvalCase("x-y",x-y),
      EvalCase("x*y",x*y),
      BadEvalCase("z")
    )
    tests.foreach(_.run())
  }

  @Test
  def testQStrings() {
    class TestCase(validate: ()=>Unit) { def run() = validate() }
    case class NoParseCase(input: String) extends TestCase(()=>{ Grammar.parseQString(input).right.foreach(v=>assert(false,"Should not parse")) })
    case class PassCase(input: String,result: String) extends TestCase(()=>Grammar.parseQString(input) match {
      case Left(e) => fail(e.result.toString)
      case Right(v) => assert (v.toString==result,"Got '"+v.toString+"', expected '"+result+"'")
    })

    val tests = Seq(
      NoParseCase("xyz"),
      NoParseCase("\"xyz"),
      NoParseCase("\"xy\nz\""),
      NoParseCase("x\"yz"),
      NoParseCase("""\q"""), // bad escape
      PassCase(""""123"""","""QString("123")"""),
      PassCase(""""123\"456"""","""QString("123\"456")"""),
      PassCase(""""\r\"\n\\"""","""QString("\r\"\n\\")""")
    )
    tests.foreach(_.run())
  }
}
