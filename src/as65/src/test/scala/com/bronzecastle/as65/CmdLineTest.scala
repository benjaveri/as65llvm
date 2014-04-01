package com.bronzecastle.as65

import org.junit.Test
import org.junit.Assert._

@Test
class CmdLineTest {
  @Test
  def test() {
    class TestCase(validate: ()=>Unit) {
      def run() = validate()
    }
    case class NoParseCase(input: String) extends TestCase(() => {
      Main.Grammar.parse(input).fold(l => assert(true), r => assert(false, "Should not parse"))
    })
    case class PassCase(input: String, result: String) extends TestCase(() => Main.Grammar.parse(input) match {
      case Left(e) => fail(e.toString)
      case Right(v) => assert(v.toString == result, "Got '" + v.toString + "', expected '" + result + "'")
    })

    val tests = Seq(
      PassCase("f1", "List(Input(Path(f1)))"),
      PassCase("f1 f2", "List(Input(Path(f1)), Input(Path(f2)))"),
      PassCase("f1 -o f2", "List(Input(Path(f1)), Output(Path(f2)))"),
      PassCase("f1 -llvm f2 -o f3 f4", "List(Input(Path(f1)), LlvmPath(Path(f2)), Output(Path(f3)), Input(Path(f4)))"),
      PassCase("'f1 f2'", "List(Input(Path(f1 f2)))"),
      PassCase("-o 'f1 f2' f3", "List(Output(Path(f1 f2)), Input(Path(f3)))")
    )
    tests.foreach(_.run())
  }
}
