package com.bronzecastle.as65

import java.io.{FileReader, File, PrintWriter}

/**
 * program context that translates to LLVM IR and finally an object file
 */
class Program(val outputFilename: File,val llvm: LLVM) {
  val irFilename = File.createTempFile("as65_ir_",".ll")
  val irFile = new PrintWriter(irFilename)
  var indent = 0
  var ast = List[AST.Program]()

  /**
   * translation
   */
  def readAll(input: Seq[String]): Boolean = {
    input.foreach(filename => {
      val f = new FileReader(filename)
      try {
        Grammar.parse(f) match {
          case Left(x) => println(x.msg); return false // TODO: show filename, line & col
          case Right(x) => ast = x :: ast
        }
      } finally {
        f.close()
      }
    })

    true
  }

  /**
   * IR i/o
   */
  def compile() = {
    val asm = File.createTempFile("as65_ir_",".s")
    val bc = File.createTempFile("as65_ir_",".bc")
    try {
      llvm.llc(Seq(irFilename.getAbsolutePath), asm.getAbsolutePath)
      llvm.gcc(Seq(asm.getAbsolutePath), outputFilename.getAbsolutePath)
    } finally {
      asm.delete()
      bc.delete()
    }
  }
  def println: (String)=>Unit = (text)=>{
    val s="  "*indent+text;
    Predef.println(s); // echo
    irFile.println(s)
  }
  def close() {
    irFile.close()
  }
  def cleanup() {
    irFile.close()
    irFilename.delete()
  }

  /**
   * IR generation
   */
  class Variable(val name: String,val typ3: String)
  case class Global(n: String,t: String) extends Variable(n,t)

  // registers
  val A = Global("@RA","i8")
  val X = Global("@RX","i8")
  val Y = Global("@RY","i8")

  // status bits
  // 7   6   5   4   3   2   1   0
  // S   V       B   D   I   Z   C
  val SS = Global("@SS","i8")
  val SV = Global("@SV","i8")
  val SB = Global("@SB","i8")
  val SD = Global("@SD","i8")
  val SI = Global("@SI","i8")
  val SZ = Global("@SZ","i8")
  val SC = Global("@SC","i8")

  case object Imm {
    def i8(n: Int) = { apply("i8",n) }
    def i16(n: Int) = { apply("i16",n) }
    def i32(n: Int) = { apply("i32",n) }
    def apply(t: String,n: Int) = { new Variable(n.toString,t) }
  }
  case object Temp {
    var tempID = 0
    def i8 = { apply("i8") }
    def i16 = { apply("i16") }
    def i32 = { apply("i32") }
    def apply(t: String) = { tempID += 1; new Variable("%t"+tempID,t) }
  }

  // llvm mnemonics
  class mnemonic1(mnemonic: String) {
    def apply(a: Variable): Variable = {
      val rv = Temp(a.typ3)
      val tp = rv.typ3
      val d0 = rv.name
      val a0 = a.name
      println(s"$d0 = $mnemonic $tp $a0")
      rv
    }
  }
  class mnemonic2(mnemonic: String) {
    def apply(a: Variable,b: Variable): Variable = {
      assert(a.typ3==b.typ3)
      val rv = Temp(a.typ3)
      val tp = rv.typ3
      val d0 = rv.name
      val a0 = a.name
      val a1 = b.name
      println(s"$d0 = $mnemonic $tp $a0, $a1")
      rv
    }
  }

  ////////////////////////////////////////////////////////////////////////////

  case object add extends mnemonic2("add")

  ////////////////////////////////////////////////////////////////////////////

  def glob(a: Global) = {
    val tp = a.typ3
    val d0 = a.name
    println(s"$d0 = common global $tp 0, align 1")
    a
  }

  ////////////////////////////////////////////////////////////////////////////

  case object load {
    def z32(a: Global) = zext.i32(apply(a))
    def apply(a: Global) = {
      val rv = Temp(a.typ3)
      val d0 = rv.name
      val t0 = a.typ3
      val a0 = a.name
      println(s"$d0 = load $t0* $a0")
      rv
    }
  }
  ////////////////////////////////////////////////////////////////////////////

  case object or extends mnemonic2("or")

  ////////////////////////////////////////////////////////////////////////////

  def ret(a: Variable) = {
    val t0 = a.typ3
    val a0 = a.name
    println(s"ret $t0 $a0")
    a
  }

  ////////////////////////////////////////////////////////////////////////////

  case object shl extends mnemonic2("shl")
  def store(d: Global,a: Variable) {
    assert(d.typ3 == a.typ3)
    val d0 = d.name
    val dt = d.typ3
    val a0 = a.name

    println(s"store $dt $a0,$dt* $d0")
  }

  ////////////////////////////////////////////////////////////////////////////

  case object zext {
    def i32(a: Variable) = apply("i32",a)
    def apply(dt: String,a: Variable) = {
      val rv = Temp(dt)
      val d0 = rv.name
      val t0 = a.typ3
      val a0 = a.name
      println(s"$d0 = zext $t0 $a0 to $dt")
      rv
    }
  }

  ////////////////////////////////////////////////////////////////////////////

  /**
   * boiler plate
   */
  def emitPreamble() {
    println("; options")
    println("target triple = \"x86_64-apple-macosx10.9.0\"")
    println("")
    println("; machine registers")
    glob(A)
    glob(X)
    glob(Y)
    glob(SS)
    glob(SV)
    glob(SB)
    glob(SD)
    glob(SI)
    glob(SZ)
    glob(SC)
    println("")
    println("; main")
    println("define i32 @main() #0 {")
    indent += 1
  }

  def emitEpilogue() {
    println("; some test code")
    store(A,Imm.i8(100))
    println("")


    println("; prepare program exit code")
    val t0 = shl(load.z32(SS),Imm.i32(31))
    val t1 = or(t0,shl(load.z32(SV),Imm.i32(30)))
    val t2 = or(t1,shl(load.z32(SB),Imm.i32(28)))
    val t3 = or(t2,shl(load.z32(SD),Imm.i32(27)))
    val t4 = or(t3,shl(load.z32(SI),Imm.i32(26)))
    val t5 = or(t4,shl(load.z32(SZ),Imm.i32(25)))
    val t6 = or(t5,shl(load.z32(SC),Imm.i32(24)))
    val t7 = or(t6,shl(load.z32(Y),Imm.i32(16)))
    val t8 = or(t7,shl(load.z32(X),Imm.i32(8)))
    val t9 = or(t8,load.z32(A))
    ret(t9)
    indent -= 1
    println("}")
  }
}
