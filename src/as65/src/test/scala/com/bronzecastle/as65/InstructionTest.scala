package com.bronzecastle.as65

import org.junit.Test
import java.io.{PrintWriter, File}

@Test
class InstructionTest {
  @Test
  def testPrototype() {
    val output = File.createTempFile("as65_out_","")
    val input = File.createTempFile("as65_test_",".asm")
    try {
      val f = new PrintWriter(input)
      f.print(
        """nop
          |nop
        """.stripMargin
      )
      f.close()

      val ec = Main.invoke(input.getAbsolutePath+" -llvm /opt/llvm-3.4/bin -o "+output.getAbsolutePath)
      if (ec == 0) {
        import scala.sys.process._
        output.setExecutable(true)
        val res = Seq(output.getAbsolutePath).!
        println("res = %08x".format(res))
      }


    } finally {
      input.delete()
      output.delete()
    }
  }
}
