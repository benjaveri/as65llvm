package com.bronzecastle.as65

import scala.util.parsing.combinator.RegexParsers
import java.io.Reader

object Grammar extends RegexParsers {
  override protected val whiteSpace = "\\s|(;.*)".r

  lazy val program: Parser[AST.Program] = positioned(
    rep(statement <~ eos) ^^ { AST.Program }
  )

  lazy val statement: Parser[AST.Statement] = statement // or asm directives, like .org

  lazy val instruction = rep(label) ~ mnemonic ^^ { case l~i => AST.Ins }

  lazy val label: Parser[AST.Label] = positioned(identifier <~ ":" ^^ { AST.Label })

  lazy val reserved =
    "adc" | "and" | "asl" |
    "bcc" | "bcs" | "beq" | "bit" | "bmi" | "bne" | "bpl" | "brk" | "bvc" | "bvs" |
    "clc" | "cld" | "cli" | "clv" | "cmp" | "cpx" | "cpy" |
    "dec" | "dex" | "dey" |
    "eor" |
    "inc" | "inx" | "iny" |
    "jmp" | "jsr" |
    "lda" | "ldx" | "ldy" | "lsr" |
    "nop" |
    "ora" |
    "pha" | "php" | "pla" | "plp" |
    "rol" | "ror" | "rti" | "rts" |
    "sbc" | "sec" | "sed" | "sei" | "sta" |
    "stx" | "sty" | "tax" | "tay" | "tsx" | "txa" | "txs" | "tya"

  lazy val mnemonic: Parser[AST.Mnemonic] = positioned(
    adc | and | asl |
    bcc | bcs | beq | bit | bmi | bne | bpl | brk | bvc | bvs |
    clc | cld | cli | clv | cmp | cpx | cpy |
    dec | dex | dey |
    eor |
    inc | inx | iny |
    jmp | jsr |
    lda | ldx | ldy | lsr |
    nop |
    ora |
    pha | php | pla | plp |
    rol | ror | rti | rts |
    sbc | sec | sed | sei | sta |
    stx | sty | tax | tay | tsx | txa | txs | tya
  )

  // can we use "acceptIf" & "into" against bitmask to parse operands precisely?
  lazy val adc = "adc" ~> operand ^^ { AST.ADC }
  lazy val and = "and" ~> operand ^^ { AST.AND }
  lazy val asl = "asl" ~> operand ^^ { AST.ASL }
  lazy val bcc = "bcc" ~> label ^^ { AST.BCC }
  lazy val bcs = "bcs" ~> label ^^ { AST.BCS }
  lazy val beq = "beq" ~> label ^^ { AST.BEQ }
  lazy val bit = "bit" ~> operand ^^ { AST.BIT }
  lazy val bmi = "bmi" ~> label ^^ { AST.BMI }
  lazy val bne = "bne" ~> label ^^ { AST.BNE }
  lazy val bpl = "bpl" ~> label ^^ { AST.BPL }
  lazy val brk = "brk" ^^ { case _ => AST.BRK() }
  lazy val bvc = "bvc" ~> label ^^ { AST.BVC }
  lazy val bvs = "bvs" ~> label ^^ { AST.BVS }
  lazy val clc = "clc" ^^ { case _ => AST.CLC() }
  lazy val cld = "cld" ^^ { case _ => AST.CLD() }
  lazy val cli = "cli" ^^ { case _ => AST.CLI() }
  lazy val clv = "clv" ^^ { case _ => AST.CLV() }
  lazy val cmp = "cmp" ~> operand ^^ { AST.CMP }
  lazy val cpx = "cpx" ~> operand ^^ { AST.CPX }
  lazy val cpy = "cpy" ~> operand ^^ { AST.CPY }
  lazy val dec = "dec" ~> operand ^^ { AST.DEC }
  lazy val dex = "dex" ^^ { case _ => AST.DEX() }
  lazy val dey = "dey" ^^ { case _ => AST.DEY() }
  lazy val eor = "eor" ~> operand ^^ { AST.EOR }
  lazy val inc = "inc" ~> operand ^^ { AST.INC }
  lazy val inx = "inx" ^^ { case _ => AST.INX() }
  lazy val iny = "iny" ^^ { case _ => AST.INY() }
  lazy val jmp = "jmp" ~> label ^^ { AST.JMP }
  lazy val jsr = "jsr" ~> label ^^ { AST.JSR }
  lazy val lda = "lda" ~> operand ^^ { AST.LDA }
  lazy val ldx = "ldx" ~> operand ^^ { AST.LDX }
  lazy val ldy = "ldy" ~> operand ^^ { AST.LDY }
  lazy val lsr = "lsr" ~> operand ^^ { AST.LSR }
  lazy val nop = "nop" ^^ { case _ => AST.NOP() }
  lazy val ora = "ora" ~> operand ^^ { AST.ORA }
  lazy val pha = "pha" ^^ { case _ => AST.PHA() }
  lazy val php = "php" ^^ { case _ => AST.PHP() }
  lazy val pla = "pla" ^^ { case _ => AST.PLA() }
  lazy val plp = "plp" ^^ { case _ => AST.PLP() }
  lazy val rol = "rol" ~> operand ^^ { AST.ROL }
  lazy val ror = "ror" ~> operand ^^ { AST.ROR }
  lazy val rti = "rti" ^^ { case _ => AST.RTI() }
  lazy val rts = "rts" ^^ { case _ => AST.RTS() }
  lazy val sbc = "sbc" ~> operand ^^ { AST.SBC }
  lazy val sec = "sec" ^^ { case _ => AST.SEC() }
  lazy val sed = "sed" ^^ { case _ => AST.SED() }
  lazy val sei = "sei" ^^ { case _ => AST.SEI() }
  lazy val sta = "sta" ~> operand ^^ { AST.STA }
  lazy val stx = "stx" ~> operand ^^ { AST.STX }
  lazy val sty = "sty" ~> operand ^^ { AST.STY }
  lazy val tax = "tax" ^^ { case _ => AST.TAX() }
  lazy val tay = "tay" ^^ { case _ => AST.TAY() }
  lazy val tsx = "tsx" ^^ { case _ => AST.TSX() }
  lazy val txa = "txa" ^^ { case _ => AST.TXA() }
  lazy val txs = "txs" ^^ { case _ => AST.TXS() }
  lazy val tya = "tya" ^^ { case _ => AST.TYA() }

  lazy val operand: Parser[AST.Operand] = (imm | indx | indy | absx | absy | abs)
  lazy val imm = "#" ~> expr ^^ { AST.Imm }
  lazy val absx = expr <~ ","~"[xX]".r ^^ { AST.AbsX }
  lazy val absy = expr <~ ","~"[yY]".r ^^ { AST.AbsY }
  lazy val abs = expr ^^ { AST.Abs }
  lazy val indx = "(" ~> expr <~ ","~"[xX]".r~")" ^^ { AST.IndX }
  lazy val indy = "(" ~> expr <~ ")"~","~"[yY]".r ^^ { AST.IndX }


  lazy val expr: Parser[AST.Value] = positioned(
    term ~ rep("+" ~ term | "-" ~ term) ^^ {
      case num ~ list => list.foldLeft(num)((x,t) => t match {
        case "+" ~ y => AST.Sum(x,y)
        case "-" ~ y => AST.Diff(x,y)
      })
    }
  )

  lazy val term: Parser[AST.Value] = positioned(
    factor ~ rep("*" ~ factor | "/" ~ factor | "%" ~ factor) ^^ {
      case num ~ list => list.foldLeft(num)((x,t) => t match {
        case "*" ~ y => AST.Product(x,y)
        case "/" ~ y => AST.Quotient(x,y)
        case "%" ~ y => AST.Remainder(x,y)
      })
    }
  )

  lazy val factor: Parser[AST.Value] = positioned(
    "-" ~> factor ^^ { case f => AST.Negate(f) } |
    "+" ~> factor ^^ { case f => f } |
    tfactor
  )

  lazy val tfactor: Parser[AST.Value] = positioned(
    "(" ~> expr <~ ")" |
    number |
    identifier |
    qstring |
    failure("Number, identifier, string or scalar expression expected")
  )

  lazy val number: Parser[AST.Number] = positioned(
    "[0-9]+".r ^^ { s => AST.Number(BigInt(s)) } |
    "\\$[0-9a-fA-F]+".r ^^ { s => AST.Number(BigInt(s.substring(1),16)) } |
    "\\%[0-1]+".r ^^ { s => AST.Number(BigInt(s.substring(1),2)) }
  )

  lazy val qstring: Parser[AST.QString] = positioned(
    "\"" ~> rep(not(eos) ~ not("\"") ~ ".".r) <~ "\"" ^^ { l => AST.QString(l.foldLeft("")((z,i)=>z+i)) }
  )

  lazy val identifier: Parser[AST.Identifier] = positioned(
    not(reserved) ~> "[a-zA-Z_\\.][0-9a-zA-Z_]*".r ^^ { AST.Identifier }
  )

  lazy val eos = eol | eoi
  lazy val eol: Parser[String] = "[\r\n]".r
  lazy val eoi: Parser[String] = "$".r


  /**
   * internal parse context
   */
  class ParseFailureException(val result: ParseResult[Nothing]) extends Exception

  /**
   * parsing entry points
   */
  def parse(reader: Reader): Either[Throwable,AST.Program] = {
    super.parse(program,reader) match {
      case Success(result,next) => Right(result)
      case e: Error => Left(new ParseFailureException(e))
      case f: Failure => Left(new ParseFailureException(f))
    }
  }
  def parse(s: String): Either[Throwable,AST.Program] = {
    super.parse(program,s) match {
      case Success(result,next) => Right(result)
      case e: Error => Left(new ParseFailureException(e))
      case f: Failure => Left(new ParseFailureException(f))
    }
  }
  def parseExpr(s: String): Either[Throwable,AST.Value] = {
    super.parse(expr,s) match {
      case Success(result,next) => Right(result)
      case e: Error => Left(new ParseFailureException(e))
      case f: Failure => Left(new ParseFailureException(f))
    }
  }
}
