package com.bronzecastle.as65

import java.io.{FileReader, File, PrintWriter}
import org.apache.commons.io.FilenameUtils

/**
 * program context that translates to LLVM IR and finally an object file
 */
class Program(val outputFile: File,val buildPath: File,val llvm: LLVM) {
  /**
   * temp file manager
   */
  buildPath.mkdirs()
  var tempFiles = List[File]()
  val baseTempName = FilenameUtils.getBaseName(outputFile.getName)
  def createTempFile(suffix: String) = {
    tempFiles = new File(buildPath,baseTempName+suffix) :: tempFiles
    tempFiles.head.delete()
    tempFiles.head
  }

  /**
   * evaluation context
   */
  val E = new Evaluator

  /**
   * translation
   */
  case class InputFile(filename: String,program: AST.Program) {
    val shortname = FilenameUtils.getName(filename)
  }
  var ast = List[InputFile]()
  def readAll(input: Seq[String]): Boolean = {
    input.foreach(filename => {
      val f = new FileReader(filename)
      try {
        Grammar.parse(f) match {
          case Left(x) => println(x.msg); return false // TODO: show filename, line & col
          case Right(x) => ast = InputFile(filename,x) :: ast
        }
      } finally {
        f.close()
      }
    })

    true
  }
  def link() {
    // assemble & link
    val opt = createTempFile(".opt.bc")
    val asm = createTempFile(".s")
    assert(0 == llvm.opt(Seq(irFile.getAbsolutePath), opt.getAbsolutePath))
    assert(0 == llvm.llc(Seq(opt.getAbsolutePath), asm.getAbsolutePath))
    assert(0 == llvm.gcc(Seq(asm.getAbsolutePath), outputFile.getAbsolutePath))
  }

  /**
   * IR i/o
   */
  val irFile = createTempFile(".ll")
  val irWriter = new PrintWriter(irFile)
  var irIndent = 0

  def println: (String)=>Unit = (text)=>{
    val s = "  "*irIndent+text
    //Predef.println(s) // echo
    irWriter.println(s)
  }
  def comment(s: String) { println(s"; $s") }
  def newline() { println("") }
  def close() {
    irWriter.close()
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
  val S = Global("@RS","i8")
  
  // processor status flags
  val NF = Global("@NF","i8")
  val VF = Global("@VF","i8")
  val BF = Global("@BF","i8")
  val DF = Global("@DF","i8")
  val IF = Global("@IF","i8")
  val ZF = Global("@ZF","i8")
  val CF = Global("@CF","i8")

  // data memory
  val MEM = Global("@MEM","[65536 x i8]")
  
  // immediate as a typed variable
  case object Imm {
    case class Value(n: Int,t: String) extends Variable(n.toString,t)
    def i8(n: Int) = { Value(n,"i8") }
    def i16(n: Int) = { Value(n,"i16") }
    def i32(n: Int) = { Value(n,"i32") }
    def apply(n: Int,t: String) = { Value(n,t) }
  }
  // temp variable (internal to llvm ir)
  case object Temp {
    case class Value(n: String,t: String) extends Variable(n,t)
    var tempID = 0
    def i1 = { apply("i1") }
    def i8 = { apply("i8") }
    def i16 = { apply("i16") }
    def i32 = { apply("i32") }
    def apply(t: String) = { tempID += 1; Value("%t"+tempID,t) }
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
  case object ashr extends mnemonic2("ashr")

  ////////////////////////////////////////////////////////////////////////////


  ////////////////////////////////////////////////////////////////////////////

  def declareGlobal(a: Global) = {
    val tp = a.typ3
    val d0 = a.name
    println(s"$d0 = common global $tp 0, align 1")
    a
  }

  ////////////////////////////////////////////////////////////////////////////

  /*
    eq: yields true if the operands are equal, false otherwise. No sign interpretation is necessary or performed.
    ne: yields true if the operands are unequal, false otherwise. No sign interpretation is necessary or performed.
    ugt: interprets the operands as unsigned values and yields true if op1 is greater than op2.
    uge: interprets the operands as unsigned values and yields true if op1 is greater than or equal to op2.
    ult: interprets the operands as unsigned values and yields true if op1 is less than op2.
    ule: interprets the operands as unsigned values and yields true if op1 is less than or equal to op2.
    sgt: interprets the operands as signed values and yields true if op1 is greater than op2.
    sge: interprets the operands as signed values and yields true if op1 is greater than or equal to op2.
    slt: interprets the operands as signed values and yields true if op1 is less than op2.
    sle: interprets the operands as signed values and yields true if op1 is less than or equal to op2.
   */
  def icmp(cond: String,a: Variable,b: Variable) = {
    assert(a.typ3==b.typ3)
    val rv = Temp.i1
    val d0 = rv.name
    val tp = a.typ3
    val a0 = a.name
    val a1 = b.name
    println(s"$d0 = icmp $cond $tp $a0, $a1")
    rv
  }

  ////////////////////////////////////////////////////////////////////////////

  case object load {
    def z32(a: Variable) = zext.i32(apply(a))
    def apply(a: Variable) = {
      val rv = Temp(a.typ3)
      val d0 = rv.name
      val t0 = a.typ3
      val a0 = a.name
      println(s"$d0 = load $t0* $a0")
      rv
    }
  }

  case object lshr extends mnemonic2("lshr")

  ////////////////////////////////////////////////////////////////////////////

  case object or extends mnemonic2("or")

  ////////////////////////////////////////////////////////////////////////////

  def peek(addr: Variable) = {
    val mn = MEM.name
    val mt = MEM.typ3
    val an = addr.name
    val at = addr.typ3
    val t0 = Temp.i8
    val t0n = t0.name
    println(s"$t0n = getelementptr inbounds $mt* $mn, i16 0, $at $an")
    load(t0)
  }
  
  def poke(a: Variable,addr: Variable) {
    val mn = MEM.name
    val mt = MEM.typ3
    val an = addr.name
    val at = addr.typ3
    val t0 = Temp.i8
    val t0n = t0.name
    println(s"$t0n = getelementptr inbounds $mt* $mn, i16 0, $at $an")
    store(a,t0)
  }

  ////////////////////////////////////////////////////////////////////////////

  def ret(a: Variable) = {
    val t0 = a.typ3
    val a0 = a.name
    println(s"ret $t0 $a0")
    a
  }

  ////////////////////////////////////////////////////////////////////////////


  def select(cc: Variable,a: Variable,b: Variable) = {
    assert(cc.typ3=="i1")
    assert(a.typ3==b.typ3)
    val cn = cc.name
    val tp = a.typ3
    val a0 = a.name
    val a1 = b.name
    val rv = Temp(tp)
    val d0 = rv.name
    println(s"$d0 = select i1 $cn, $tp $a0, $tp $a1")
    rv
  }

  case object shl extends mnemonic2("shl")

  // [d] := a
  def store(a: Variable,d: Variable) {
    assert(d.typ3 == a.typ3)
    val d0 = d.name
    val dt = d.typ3
    val a0 = a.name
    println(s"store $dt $a0,$dt* $d0")
  }

  case object sub extends mnemonic2("sub")

  ////////////////////////////////////////////////////////////////////////////

  case object zext {
    def i8(a: Variable) = apply("i8",a)
    def i16(a: Variable) = apply("i16",a)
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
    comment("options")
    println("target triple = \"x86_64-apple-macosx10.9.0\"") // TODO: dont hardcode
    newline()
    comment("machine registers")
    declareGlobal(A)
    declareGlobal(X)
    declareGlobal(Y)
    declareGlobal(S)
    declareGlobal(NF)
    declareGlobal(VF)
    declareGlobal(BF)
    declareGlobal(DF)
    declareGlobal(IF)
    declareGlobal(ZF)
    declareGlobal(CF)
    println(s"@MEM = common global [65536 x i8] zeroinitializer, align 1")
    newline()
    comment("main")
    println("define i32 @main() #0 {")
    irIndent += 1
    // init registers
    comment("initialize cpu to power-up state")
    store(Imm.i8(1),IF)
    store(Imm.i8(255),S)
  }

  def emitEpilogue() {
    comment("return value of accumulator as exit code")
    ret(load.z32(A))
    irIndent -= 1
    println("}")
  }

  var currentInputFile: InputFile = null

  def emitProgram() {
    for (prg <- ast) {
      currentInputFile = prg
      for (stmt <- prg.program.list) stmt match {
        case AST.Ins(m) => emitInstruction(m)
        case _ => throw new Exception("NOT IMPLEMENTED YET")
      }
    }
  }

  def emitInstruction(m: AST.Mnemonic) {
    def updateNZ(a: Variable) {
      // NF = a.7
      store(lshr(a,Imm.i8(7)),NF)
      // ZF = (a==0)?1:0
      store(zext.i8(icmp("eq",a,Imm.i8(0))),ZF)
    }

    def computeEA(oper: AST.Operand) = oper match {
      case AST.Abs(expr) => E.evaluate(expr) match {
        case Left(x) => throw new Exception(x.toString())
        case Right(value) => {
          assert((value >= 0) && (value <= 65535))
          Imm.i16(value.toInt)
        }
      }
      case AST.AbsX(expr) => E.evaluate(expr) match {
        case Left(x) => throw new Exception(x.toString())
        case Right(value) => {
          assert((value >= 0) && (value <= 65535))
          add(Imm.i16(value.toInt),zext.i16(load(X)))
        }
      }
      case AST.AbsY(expr) => E.evaluate(expr) match {
        case Left(x) => throw new Exception(x.toString())
        case Right(value) => {
          assert((value >= 0) && (value <= 65535))
          add(Imm.i16(value.toInt),zext.i16(load(Y)))
        }
      }
      case AST.IndX(expr) => E.evaluate(expr) match {
        case Left(x) => throw new Exception(x.toString())
        case Right(value) => {
          assert((value >= 0) && (value <= 255))
          val x = load(X)
          val lea = peek(zext.i16(add(Imm.i8(value.toInt),x)))
          val hea = peek(zext.i16(add(Imm.i8(value.toInt+1),x)))
          or(shl(zext.i16(hea),Imm.i16(8)),zext.i16(lea))
        }
      }
      case AST.IndY(expr) => E.evaluate(expr) match {
        case Left(x) => throw new Exception(x.toString())
        case Right(value) => {
          assert((value >= 0) && (value <= 255))
          val lea = peek(zext.i16(Imm.i8(value.toInt)))
          val hea = peek(zext.i16(Imm.i8(value.toInt+1)))
          add(or(shl(zext.i16(hea),Imm.i16(8)),zext.i16(lea)),zext.i16(load(Y)))
        }
      }
      case x => throw new Exception("NOT IMPLEMENTED: "+x.toString)
    }
    def readOperand(oper: AST.Operand) = oper match {
      case AST.Imm(expr) => E.evaluate(expr) match {
        case Left(x) => throw new Exception(x.toString())
        case Right(value) => {
          assert((value >= 0) && (value <= 255))
          Imm.i8(value.toInt)
        }
      }
      case x => peek(computeEA(x))
    }
    def writeOperand(a: Variable,oper: AST.Operand) = poke(a,computeEA(oper))

    comment(currentInputFile.shortname+"("+m.pos.toString+"): "+m.toString)
    m match {
      case AST.LDA(oper) => {
        assert(m.isValidOperand(oper))
        val a = readOperand(oper)
        store(a,A)
        updateNZ(a)
      }
      case AST.LDX(oper) => {
        assert(m.isValidOperand(oper))
        val x = readOperand(oper)
        store(x,X)
        updateNZ(x)
      }
      case AST.LDY(oper) => {
        assert(m.isValidOperand(oper))
        val y = readOperand(oper)
        store(y,Y)
        updateNZ(y)
      }
      case AST.NOP() => {}
      case AST.PHA() => {
        // [S] = A
        val sp0 = load(S)
        poke(load(A),add(zext.i16(sp0),Imm.i16(0x100)))
        // S--
        val sp1 = sub(sp0,Imm.i8(1))
        store(sp1,S)
      }
      case AST.PHP() => {
        // [S] = NV-BDIZC
        val s = shl(load(NF),Imm.i8(7))
        val v = or(s,shl(load(VF),Imm.i8(6)))
        val b = or(v,shl(load(BF),Imm.i8(4)))
        val d = or(b,shl(load(DF),Imm.i8(3)))
        val i = or(d,shl(load(DF),Imm.i8(2)))
        val z = or(i,shl(load(ZF),Imm.i8(1)))
        val c = or(z,load(CF))
        val sp0 = load(S)
        poke(c,add(zext.i16(sp0),Imm.i16(0x100)))
        // S--
        val sp1 = sub(sp0,Imm.i8(1))
        store(sp1,S)
      }
      case AST.PLA() => {
        // S++
        val sp0 = load(S)
        val sp1 = add(sp0,Imm.i8(1))
        store(sp1,S)
        // A = [S]
        val a = peek(add(zext.i16(sp1),Imm.i16(0x100)))
        store(a,A)
        updateNZ(a)
      }
      case AST.SEC() => store(Imm.i8(1),CF)
      case AST.STA(oper) => {
        assert(m.isValidOperand(oper))
        writeOperand(load(A),oper)
      }
      case AST.TAX() => {
        val a = load(A)
        store(a,X)
        updateNZ(a)
      }
      case AST.TAY() => {
        val a = load(A)
        store(a,Y)
        updateNZ(a)
      }
      case AST.TXA() => {
        val x = load(X)
        store(x,A)
        updateNZ(x)
      }
      case AST.TYA() => {
        val y = load(Y)
        store(y,A)
        updateNZ(y)
      }
      case x => throw new Exception("NOT IMPLEMENTED: "+x.toString)
    }
  }
}
