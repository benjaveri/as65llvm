package com.bronzecastle.as65

import java.io.File

/**
 * LLVM interfaces
 */
class LLVM(val path: Option[String]) {
  import scala.sys.process._

  def llc(input: Seq[String],output: String) = {
    val command = Seq(
      new File(path.get,"llc").getAbsolutePath,
      "-o",output,
      "-disable-cfi"
    ) ++ input
    command.!
  }

  def opt(input: Seq[String],output: String) = {
    val command = Seq(
      new File(path.get,"opt").getAbsolutePath,
      "-o",output,
      "-O3"
    ) ++ input
    command.!
  }

  // gcc can take .s files and link c runtime in (handy)
  def gcc(input: Seq[String],output: String) = {
    val command = Seq(
      "gcc",
      "-o",output
    ) ++ input
    command.!
  }
}
