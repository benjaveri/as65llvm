package com.bronzecastle.as65

import scala.util.parsing.input.Positional

object AST {
  case class Program(list: Seq[Statement]) extends Node

  trait Statement extends Node
  case class Label(name: Identifier) extends Statement
  case class Ins(iop: Mnemonic) extends Statement
  case class Db(list: List[Value]) extends Statement

  class Mnemonic(val validOperands: Int) extends Node {
    def this() = this(0)
    def isValidOperand(op: Operand) = (validOperands & op.mask) != 0
  }

  case class ADC(oper: Operand) extends Mnemonic(M_IMM|M_ABS|M_ABSX|M_ABSY|M_INDX|M_INDY)
  case class AND(oper: Operand) extends Mnemonic
  case class ASL(oper: Operand) extends Mnemonic
  case class BCC(target: Label) extends Mnemonic
  case class BCS(target: Label) extends Mnemonic
  case class BEQ(target: Label) extends Mnemonic
  case class BIT(oper: Operand) extends Mnemonic
  case class BMI(target: Label) extends Mnemonic
  case class BNE(target: Label) extends Mnemonic
  case class BPL(target: Label) extends Mnemonic
  case class BRK() extends Mnemonic
  case class BVC(target: Label) extends Mnemonic
  case class BVS(target: Label) extends Mnemonic
  case class CLC() extends Mnemonic
  case class CLD() extends Mnemonic
  case class CLI() extends Mnemonic
  case class CLV() extends Mnemonic
  case class CMP(oper: Operand) extends Mnemonic
  case class CPX(oper: Operand) extends Mnemonic
  case class CPY(oper: Operand) extends Mnemonic
  case class DEC(oper: Operand) extends Mnemonic
  case class DEX() extends Mnemonic
  case class DEY() extends Mnemonic
  case class EOR(oper: Operand) extends Mnemonic
  case class INC(oper: Operand) extends Mnemonic
  case class INX() extends Mnemonic
  case class INY() extends Mnemonic
  case class JMP(target: Label) extends Mnemonic
  case class JSR(target: Label) extends Mnemonic
  case class LDA(oper: Operand) extends Mnemonic(M_IMM|M_ABS|M_ABSX|M_ABSY|M_INDX|M_INDY)
  case class LDX(oper: Operand) extends Mnemonic(M_IMM|M_ABS|M_ABSY)
  case class LDY(oper: Operand) extends Mnemonic(M_IMM|M_ABS|M_ABSX)
  case class LSR(oper: Operand) extends Mnemonic
  case class NOP() extends Mnemonic
  case class ORA(oper: Operand) extends Mnemonic
  case class PHA() extends Mnemonic
  case class PHP() extends Mnemonic
  case class PLA() extends Mnemonic
  case class PLP() extends Mnemonic
  case class ROL(oper: Operand) extends Mnemonic
  case class ROR(oper: Operand) extends Mnemonic
  case class RTI() extends Mnemonic
  case class RTS() extends Mnemonic
  case class SBC(oper: Operand) extends Mnemonic
  case class SEC() extends Mnemonic
  case class SED() extends Mnemonic
  case class SEI() extends Mnemonic
  case class STA(oper: Operand) extends Mnemonic(M_ABS|M_ABSX|M_ABSY|M_INDX|M_INDY)
  case class STX(oper: Operand) extends Mnemonic
  case class STY(oper: Operand) extends Mnemonic
  case class TAX() extends Mnemonic
  case class TAY() extends Mnemonic
  case class TSX() extends Mnemonic
  case class TXA() extends Mnemonic
  case class TXS() extends Mnemonic
  case class TYA() extends Mnemonic

 
  val M_ACC = 1
  val M_IMM = 2
  val M_ABS = 4
  val M_ABSX = 8
  val M_ABSY = 16
  val M_INDX = 32
  val M_INDY = 64
  class Operand(val mask: Int) extends Node
  case class Acc() extends Operand(M_ACC)
  case class Imm(value: Value) extends Operand(M_IMM)
  case class Abs(value: Value) extends Operand(M_ABS)
  case class AbsX(value: Value) extends Operand(M_ABSX)
  case class AbsY(value: Value) extends Operand(M_ABSY)
  case class IndX(value: Value) extends Operand(M_INDX)
  case class IndY(value: Value) extends Operand(M_INDY)

  /**
   * expressions
   */
  trait Value extends Node
  case class Identifier(name: String) extends Value
  case class QString(value: String) extends Value
  case class Number(value: BigInt) extends Value
  case class Negate(a: Value) extends Value
  case class Sum(a: Value,b: Value) extends Value
  case class Diff(a: Value,b: Value) extends Value
  case class Product(a: Value,b: Value) extends Value
  case class Quotient(a: Value,b: Value) extends Value
  case class Remainder(a: Value,b: Value) extends Value

  trait Node extends Positional
}
