package com.bronzecastle.as65

import org.junit.Test
import org.junit.Assert._
import java.io.{PrintWriter, File}

@Test
class InstructionTest {
  @Test
  def testPrototype() {
    // order of tests matter
    trait Test { val list: Seq[TestCase] }
    case class TestCase(input: String,result: Int) extends Test {
      override val list = Seq(this)
    }
    case class TestADC(a0: Int,a1: Int,cf: Int,df: Int,ra: Int,rn: Int,rv: Int,rz: Int,rc: Int) extends Test {
      val scf = if (cf==0) "clc" else "sec"
      val sdf = if (df==0) "cld" else "sed"
      override val list = Seq(
        TestCase(f"$sdf $scf lda #$$$a0%02x adc #$$$a1%02x",ra),
        TestCase(f"$sdf $scf lda #$$$a0%02x adc #$$$a1%02x php pla",(rn<<7)|(rv<<6)|32|(df<<8)|(rz<<1)|rc)
      )
    }
    case class TestSBC(a0: Int,a1: Int,cf: Int,df: Int,ra: Int,rn: Int,rv: Int,rz: Int,rc: Int) extends Test {
      val scf = if (cf==0) "clc" else "sec"
      val sdf = if (df==0) "cld" else "sed"
      override val list = Seq(
        TestCase(f"$sdf $scf lda #$$$a0%02x sbc #$$$a1%02x",ra),
        TestCase(f"$sdf $scf lda #$$$a0%02x sbc #$$$a1%02x php pla",(rn<<7)|(rv<<6)|32|(df<<8)|(rz<<1)|rc)
      )
    }
    val suite = Seq(
      //
      // VALIDATE: LDA imm, PHA, PLA, PHP
      //

      // LDA #imm is an early workhorse
      TestCase("lda #123",123),
      TestCase("lda #255",255),
      TestCase("lda #1",1),
      // LDA setting N and Z flags; implicit assumption PHP/PLA works
      TestCase("lda #0 php pla",0x22), // Z, ~N
      TestCase("lda #127 php pla",0x20), // ~Z, ~N
      TestCase("lda #255 php pla",0xa0), // ~Z, N
      // PLA setting N and Z flags
      TestCase("lda #0 pha pla php pla",0x22), // Z, ~N
      TestCase("lda #127 pha pla php pla",0x20), // ~Z, ~N
      TestCase("lda #255 pha pla php pla",0xa0), // ~Z, N

      //
      // VALIDATE: TAX, TXA, TAY, TYA, LDX #imm, LDY #imm
      //
      TestCase("lda #255 txa",0),
      TestCase("lda #255 tax txa",255),
      TestCase("lda #255 tya",0),
      TestCase("lda #255 tay tya",255),
      TestCase("lda #0 tax php pla",0x22), // Z, ~N
      TestCase("lda #127 tax php pla",0x20), // ~Z, ~N
      TestCase("lda #255 tax php pla",0xa0), // ~Z, N
      TestCase("lda #0 tay php pla",0x22), // Z, ~N
      TestCase("lda #127 tay php pla",0x20), // ~Z, ~N
      TestCase("lda #255 tay php pla",0xa0), // ~Z, N
      TestCase("lda #0 tax txa php pla",0x22), // Z, ~N
      TestCase("lda #127 tax txa php pla",0x20), // ~Z, ~N
      TestCase("lda #255 tax txa php pla",0xa0), // ~Z, N
      TestCase("lda #0 tay tya php pla",0x22), // Z, ~N
      TestCase("lda #127 tay tya php pla",0x20), // ~Z, ~N
      TestCase("lda #255 tay tya php pla",0xa0), // ~Z, N
      TestCase("ldx #123 txa",123),
      TestCase("ldx #255 txa",255),
      TestCase("ldx #1 txa",1),
      TestCase("ldx #0 php pla",0x22), // Z, ~N
      TestCase("ldx #127 php pla",0x20), // ~Z, ~N
      TestCase("ldx #255 php pla",0xa0), // ~Z, N
      TestCase("ldy #123 tya",123),
      TestCase("ldy #255 tya",255),
      TestCase("ldy #1 tya",1),
      TestCase("ldy #0 php pla",0x22), // Z, ~N
      TestCase("ldy #127 php pla",0x20), // ~Z, ~N
      TestCase("ldy #255 php pla",0xa0), // ~Z, N

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
      // reference: http://homepage.ntlworld.com/cyborgsystems/CS_Main/6502/6502.htm#DETAIL
      //

      // ADC (c.f. http://visual6502.org/wiki/index.php?title=6502DecimalMode)
      // bin   A0   A1  C D |  R  N V Z C
      TestADC(0x00,0x00,0,0, 0x00,0,0,1,0),
      TestADC(0x79,0x00,1,0, 0x7a,0,0,0,0),
      TestADC(0x93,0x82,0,0, 0x15,0,1,0,1),
      TestADC(0x89,0x76,0,0, 0xff,1,0,0,0),
      TestADC(0x89,0x76,1,0, 0x00,0,0,1,1),
      TestADC(0x80,0xf0,0,0, 0x70,0,1,0,1),
      // bcd   A0   A1  C D |  R  N V Z C
      TestADC(0x00,0x00,0,1, 0x00,0,0,1,0),
      TestADC(0x79,0x00,1,1, 0x80,1,1,0,0),
      TestADC(0x24,0x56,0,1, 0x80,1,1,0,0),
      TestADC(0x93,0x82,0,1, 0x75,0,1,0,1),
      TestADC(0x89,0x76,0,1, 0x65,0,0,0,1),
      TestADC(0x89,0x76,1,1, 0x66,0,0,1,1),
      TestADC(0x80,0xf0,0,1, 0xd0,0,1,0,1),
      TestADC(0x80,0xfa,0,1, 0xe0,1,0,0,1),
      TestADC(0x2f,0x4f,0,1, 0x74,0,0,0,0),
      TestADC(0x6f,0x00,1,1, 0x76,0,0,0,0),
    
      // CLC
      TestCase("sec clc php pla",0x20),

      // CLD
      TestCase("sed cld php pla",0x20),

      // CLI
      TestCase("cli php pla",0x20),

      // SBC (c.f. http://visual6502.org/wiki/index.php?title=6502DecimalMode)
      // bin   A0   A1  C D |  R  N V Z C
      TestSBC(0x00,0x00,0,0, 0x99,1,0,0,0), // todo
      TestSBC(0x00,0x00,1,0, 0x00,0,0,1,1), // todo
      TestSBC(0x00,0x01,1,0, 0x99,1,0,0,0), // todo
      TestSBC(0x0a,0x00,1,0, 0x0a,0,0,0,1), // todo
      TestSBC(0x0b,0x00,0,0, 0x0a,0,0,0,1), // todo
      TestSBC(0x9a,0x00,1,0, 0x9a,1,0,0,1), // todo
      TestSBC(0x9b,0x00,0,0, 0x9a,1,0,0,1), // todo
      // bcd   A0   A1  C D |  R  N V Z C
      TestSBC(0x00,0x00,0,1, 0x99,1,0,0,0),
      TestSBC(0x00,0x00,1,1, 0x00,0,0,1,1),
      TestSBC(0x00,0x01,1,1, 0x99,1,0,0,0),
      TestSBC(0x0a,0x00,1,1, 0x0a,0,0,0,1),
      TestSBC(0x0b,0x00,0,1, 0x0a,0,0,0,1),
      TestSBC(0x9a,0x00,1,1, 0x9a,1,0,0,1),
      TestSBC(0x9b,0x00,0,1, 0x9a,1,0,0,1),
    
      // SEC
      TestCase("sec php pla",0x21),

      // SED
      TestCase("sed php pla",0x28),

      // SEI
      TestCase("sei php pla",0x24),

      // done
      TestCase("nop",0)
    )

    // run
    val temp = new File("target")
    val output = new File(temp,"test")
    val input = new File(temp,"test.asm")

    for (tests <- suite; test <- tests.list) {
      output.delete()
      val f = new PrintWriter(input)
      f.print(test.input)
      f.close()

      val ec = Main.invoke(input.getAbsolutePath + " -temp " + temp.getAbsolutePath + " -llvm /opt/llvm-3.4/bin -o " + output.getAbsolutePath)
      assertTrue(s"$test: Compilation failed", ec == 0)

      import scala.sys.process._
      output.setExecutable(true)
      val res: Int = Seq(output.getAbsolutePath).!
      assertTrue(f"$test: Output mismatch (got $res [$$$res%02x])", res == test.result)
    }
  }
}
