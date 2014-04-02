package com.bronzecastle.as65

import org.junit.Test
import org.junit.Assert._
import java.io.{PrintWriter, File}

@Test
class InstructionTest {
  @Test
  def testPrototype() {
    // order of tests matter
    case class TestCase(input: String,result: Int)
    val tests = Seq(
      //
      // VALIDATE: LDA imm, PHA, PLA, PHP
      //

      // LDA #imm is an early workhorse
      TestCase("lda #123",123),
      TestCase("lda #255",255),
      TestCase("lda #1",1),
      // LDA setting N and Z flags; implicit assumption PHP/PLA works
      TestCase("lda #0 php pla",2), // Z, ~N
      TestCase("lda #127 php pla",0), // ~Z, ~N
      TestCase("lda #255 php pla",128), // ~Z, N
      // PLA setting N and Z flags
      TestCase("lda #0 pha pla php pla",2), // Z, ~N
      TestCase("lda #127 pha pla php pla",0), // ~Z, ~N
      TestCase("lda #255 pha pla php pla",128), // ~Z, N

      //
      // VALIDATE: TAX, TXA, TAY, TYA, LDX #imm, LDY #imm
      //
      TestCase("lda #255 txa",0),
      TestCase("lda #255 tax txa",255),
      TestCase("lda #255 tya",0),
      TestCase("lda #255 tay tya",255),
      TestCase("lda #0 tax php pla",2), // Z, ~N
      TestCase("lda #127 tax php pla",0), // ~Z, ~N
      TestCase("lda #255 tax php pla",128), // ~Z, N
      TestCase("lda #0 tay php pla",2), // Z, ~N
      TestCase("lda #127 tay php pla",0), // ~Z, ~N
      TestCase("lda #255 tay php pla",128), // ~Z, N
      TestCase("lda #0 tax txa php pla",2), // Z, ~N
      TestCase("lda #127 tax txa php pla",0), // ~Z, ~N
      TestCase("lda #255 tax txa php pla",128), // ~Z, N
      TestCase("lda #0 tay tya php pla",2), // Z, ~N
      TestCase("lda #127 tay tya php pla",0), // ~Z, ~N
      TestCase("lda #255 tay tya php pla",128), // ~Z, N
      TestCase("ldx #123 txa",123),
      TestCase("ldx #255 txa",255),
      TestCase("ldx #1 txa",1),
      TestCase("ldx #0 php pla",2), // Z, ~N
      TestCase("ldx #127 php pla",0), // ~Z, ~N
      TestCase("ldx #255 php pla",128), // ~Z, N
      TestCase("ldy #123 tya",123),
      TestCase("ldy #255 tya",255),
      TestCase("ldy #1 tya",1),
      TestCase("ldy #0 php pla",2), // Z, ~N
      TestCase("ldy #127 php pla",0), // ~Z, ~N
      TestCase("ldy #255 php pla",128), // ~Z, N

      //
      // VALIDATE: STA then operands: abs, abs+x, abs+y, (ind+x), (ind)+y
      //
      TestCase("lda 0",0),
      TestCase("lda #123 sta 0 lda #0 lda 0",123),
      TestCase("lda #123 sta 1 ldx #1 lda 0,x",123),
      TestCase("lda #123 sta 1 ldy #1 lda 0,y",123),
      TestCase("lda #123 sta 1 ldx #2 lda $ffff,x",123),
      TestCase("lda #123 sta 1 ldy #2 lda $ffff,y",123),
      TestCase("lda #123 sta $1234 lda #$34 sta $80 lda #$12 sta $81 ldx #0 lda ($80,x)",123),
      TestCase("lda #123 sta $1234 lda #$34 sta $80 lda #$12 sta $81 ldx #1 lda ($7f,x)",123),
      TestCase("lda #123 sta $1234 lda #$34 sta $ff lda #$12 sta $00 ldx #1 lda ($fe,x)",123), // (ind,x) zp wrap
      TestCase("lda #123 sta $1234 lda #$30 sta $80 lda #$12 sta $81 ldy #4 lda ($80),y",123),

      //
      // alphabetical validation of basic function of remaining
      //  instructions, some may assume valid operation of
      //  not-yet validated instructions. e.g. adc needs clc and
      //  sec working
      //

      // ADC



      TestCase("nop",0)
    )

    // run
    val temp = new File("target")
    val output = new File(temp,"test")
    val input = new File(temp,"test.asm")

    for (test <- tests) {
      output.delete()
      val f = new PrintWriter(input)
      f.print(test.input)
      f.close()

      val ec = Main.invoke(input.getAbsolutePath + " -temp " + temp.getAbsolutePath + " -llvm /opt/llvm-3.4/bin -o " + output.getAbsolutePath)
      assertTrue(s"$test: Compilation failed", ec == 0)

      import scala.sys.process._
      output.setExecutable(true)
      val res = Seq(output.getAbsolutePath).!
      assertTrue(s"$test: Output mismatch (got $res)", res == test.result)
    }
  }
}
