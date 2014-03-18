package com.bronzecastle.as65

import scala.util.parsing.input.Positional

object AST {
  case class Program(list: Seq[Statement]) extends Node

  case class Label(name: Identifier) extends Node

  trait Statement extends Node
  case class Ins(label: Label,iop: IOp,operand: Operand) extends Statement

  trait Mnemonic extends Node {
    def isValidOperand(op: Operand) = (validOperands & op.mask) != 0
    val validOperands = 0
  }

  case class ADC(oper: Operand) extends Mnemonic {
    override val validOperands = M_IMM|M_ABS|M_ABSX|M_ABSY|M_INDX|M_INDY
  }
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
  case class LDA(oper: Operand) extends Mnemonic
  case class LDX(oper: Operand) extends Mnemonic
  case class LDY(oper: Operand) extends Mnemonic
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
  case class STA(oper: Operand) extends Mnemonic
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
  case class Acc(value: Value) extends Operand(M_ACC)
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

  trait IOp // make case classes for all instructions + code gen

    /*
    
    
    namespace ast {
      //
      // keywords & definitions
      //
      enum Enum {
          U_POS = 0, U_NEG, U_NOT, U_MAX,
  
          B_ANCHOR = 0,B_ADD, B_SUB, B_MUL, B_DIV, B_MOD, B_AND, B_OR, B_XOR, B_SHL, B_SHR, B_MAX,
  
          OP_ADC = 0, OP_AND, OP_ASL,
          OP_BCC, OP_BCS, OP_BEQ, OP_BIT, OP_BMI, OP_BNE, OP_BPL, OP_BRK, OP_BVC, OP_BVS,
          OP_CLC, OP_CLD, OP_CLI, OP_CLV, OP_CMP, OP_CPX, OP_CPY,
          OP_DB, OP_DEC, OP_DEX, OP_DEY,
          OP_EOR, OP_ESC,
          OP_INC, OP_INX, OP_INY,
          OP_JMP, OP_JSR,
          OP_LDA, OP_LDX, OP_LDY, OP_LSR,
          OP_NOP,
          OP_ORA,
          OP_PHA, OP_PHP, OP_PLA, OP_PLP,
          OP_ROL,
          OP_ROR, OP_RTI, OP_RTS,
          OP_SBC, OP_SEC, OP_SED, OP_SEI, OP_STA, OP_STX, OP_STY,
          OP_TAX, OP_TAY, OP_TSX, OP_TXA, OP_TXS, OP_TYA,
          OP_MAX,
  
          M_ACC = 0, M_IMM, M_ABS, M_ABSX, M_ABSY, M_INDX, M_INDY, M_LABEL, M_MAX,
      };
  
      typedef signed long Result;
      struct UOP { Enum id; const wchar_t *match; std::function<Result(Result)> apply; };
      struct BINOP { Enum id; const wchar_t *match; int prec; std::function<Result(Result,Result)> apply; };
      struct IOP { Enum id; const wchar_t *match; };
      extern UOP uopTable[U_MAX];
      extern BINOP binopTable[B_MAX];
      extern IOP iopTable[OP_MAX];
  
      //
      // AST structures
      //
      struct Expr;
      struct Identifier {
          std::wstring name;
          std::wstring toString() const { return name; }
      };
      struct QString {
          std::wstring value;
          std::wstring toString() const { return L"\""+value+L"\""; }
      };
      typedef boost::variant<
          unsigned, // number leaf
          Identifier, // identifier leaf
          boost::recursive_wrapper<Expr> // subexpression tree
          // avoid runtime init recursion by putting above element last. if it's first, the
          // default value for 'Value' is an Expr, which is not a leaf and will recurse
          // until all available memory has been exhausted. this is not in the boost docs
      > Value;
      struct Term {
          std::vector<Enum> uop;
          Value value;
          inline std::wstring toString() const;
      };
      struct ExprTail {
          Enum binop;
          Term term;
          std::wstring toString() const { return binopTable[binop].match+term.toString(); }
      };
      struct Expr {
          Term term;
          std::vector<ExprTail> tail;
          std::wstring toString() const {
              std::wstring s = term.toString();
              for (auto it=tail.begin(); it!=tail.end(); it++) s+=it->toString();
              return s;
          }
      };
  
      struct Imm {
          Expr value;
          std::wstring toString() const { return L"#"+value.toString(); }
      };
      struct Abs {
          Expr address;
          std::wstring toString() const { return address.toString(); }
      };
      struct AbsX {
          Expr address;
          std::wstring toString() const { return address.toString()+L",x"; }
      };
      struct AbsY {
          Expr address;
          std::wstring toString() const { return address.toString()+L",y"; }
      };
      struct IndX {
          Expr address;
          std::wstring toString() const { return L"("+address.toString()+L",x)"; }
      };
      struct IndY {
          Expr address;
          std::wstring toString() const { return L"("+address.toString()+L"),y"; }
      };
  
      struct Label { Identifier name; };
      typedef boost::variant<
          Imm,
          Abs,
          AbsX,
          AbsY,
          IndX,
          IndY,
          Label
      > Oper;
      struct Ins {
          boost::optional<Label> label;
          boost::optional<Enum> iop;
          boost::optional<Oper> operand;
  
          Ins(){}
          Ins(const Label& _l):label(_l) {}
          Ins(const Label& _l,Enum _i):label(_l),iop(_i) {}
          Ins(const Label& _l,Enum _i,Oper& _o):label(_l),iop(_i),operand(_o) {}
  
          inline std::wstring toString() const;
      };
  
      struct _toStringVisitor: public boost::static_visitor<std::wstring> {
          std::wstring operator()(const unsigned a) const { return dec(a); }
          std::wstring operator()(const Identifier& a) const { return a.toString(); }
          std::wstring operator()(const boost::recursive_wrapper<Expr>& a) const { return L"("+a.get().toString()+L")"; }
          std::wstring operator()(const Imm& a) const { return a.toString(); }
          std::wstring operator()(const Abs& a) const { return a.toString(); }
          std::wstring operator()(const AbsX& a) const { return a.toString(); }
          std::wstring operator()(const AbsY& a) const { return a.toString(); }
          std::wstring operator()(const IndX& a) const { return a.toString(); }
          std::wstring operator()(const IndY& a) const { return a.toString(); }
          std::wstring operator()(const Label& a) const { return L"Label("+a.name.name+L")"; }
      };
  
      inline std::wstring Term::toString() const {
          std::wstring s;
          for (auto it=uop.begin(); it!=uop.end(); it++) s+=uopTable[*it].match;
          return s+boost::apply_visitor(_toStringVisitor(),value);
      }
      inline std::wstring Ins::toString() const {
          std::wstring rv;
          if (label) rv += label->name.name+L":";
          if (iop) {
              while (rv.size() < 8) rv.push_back(' ');
              rv += iopTable[*iop].match;
              if (operand) rv += L" "+boost::apply_visitor(_toStringVisitor(),*operand);
          }
          return rv;
      }
  
      struct TestParseString { std::vector<QString> list; };
      struct TestParseNumber { std::vector<unsigned> list; };
      struct TestParseValue { std::vector<Value> list; };
      struct TestParseTerm { std::vector<Term> list; };
      struct TestParseExpr { std::vector<Expr> list; };
  
      typedef boost::variant<
          TestParseString,
          TestParseNumber,
          TestParseValue,
          TestParseTerm,
          TestParseExpr
      > Test;
  
      typedef boost::variant<
          Ins,
          Test
      > Statement;
  
      typedef boost::variant<
          std::vector<Statement>
      > Program;
  
    
     */
  
  
  
}
