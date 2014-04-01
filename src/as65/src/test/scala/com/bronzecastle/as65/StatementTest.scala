package com.bronzecastle.as65

import org.junit.Test
import org.junit.Assert._

@Test
class StatementTest {
  @Test
  def test() {
    class TestCase(validate: ()=>Unit) { def run() = validate() }
    case class NoParseCase(input: String) extends TestCase(()=>{ Grammar.parse(input).right.foreach(v=>assert(false,"Should not parse")) })
    case class PassCase(input: String,result: String) extends TestCase(()=>Grammar.parse(input) match {
      case Left(e) => fail(e.toString)
      case Right(v) => assert (v.toString==result,"Got '"+v.toString+"', expected '"+result+"'")
    })

    val tests = Seq(
      PassCase("label:","Program(List(Label(Identifier(label))))"),
      PassCase(".x:","Program(List(Label(Identifier(.x))))"),
      PassCase(".x:.y:","Program(List(Label(Identifier(.x)), Label(Identifier(.y))))"),
      PassCase("nop","Program(List(Ins(NOP())))"),
      PassCase("asl a","Program(List(Ins(ASL(Acc()))))"),
      PassCase("lda #$10","Program(List(Ins(LDA(Imm(Number(16))))))"),
      PassCase("adc $10","Program(List(Ins(ADC(Abs(Number(16))))))"),
      PassCase("sbc $10,x","Program(List(Ins(SBC(AbsX(Number(16))))))"),
      PassCase("stx $10,y","Program(List(Ins(STX(AbsY(Number(16))))))"),
      PassCase("sta ($10),y","Program(List(Ins(STA(IndY(Number(16))))))"),
      PassCase("sta (var,x)","Program(List(Ins(STA(IndX(Identifier(var))))))"),
      PassCase("db %1001,10,$b","Program(List(Db(List(Number(9), Number(10), Number(11)))))"),
      PassCase("db \"hello\",0","""Program(List(Db(List(QString("hello"), Number(0)))))"""),
      NoParseCase("xxx")
    )
    tests.foreach(_.run())
  }
}
