package com.bronzecastle.as65

class Evaluator {
  /**
   * evaluation
   */
  def evaluate(v: AST.Value): Either[(String,AST.Value),BigInt] = {
    v match {
      case AST.Number(n) => Right(n)
      case AST.Identifier(i) => symtab.get(i).map(Right(_)).getOrElse(parent.map(_.evaluate(v)).getOrElse(Left("Undefined symbol",v)))
      case AST.Negate(a) => evaluate(a).right.map(-_)
      case AST.Sum(a,b) => evaluate(a).fold(l=>Left(l),A=>evaluate(b).right.map(B=>A+B))
      case AST.Diff(a,b) => evaluate(a).fold(l=>Left(l),A=>evaluate(b).right.map(B=>A-B))
      case AST.Product(a,b) => evaluate(a).fold(l=>Left(l),A=>evaluate(b).right.map(B=>A*B))
      case AST.Quotient(a,b) => evaluate(a).fold(l=>Left(l),A=>evaluate(b).fold(l=>Left(l),B=>if (B==0) Left(("Divide by zero",v)) else Right(A/B)))
      case AST.Remainder(a,b) => evaluate(a).fold(l=>Left(l),A=>evaluate(b).fold(l=>Left(l),B=>if (B==0) Left(("Divide by zero",v)) else Right(A%B)))
    }
  }
  
  /**
   * symbols
   */
  var parent: Option[Evaluator] = None // allows hierarchical scope for symbols
  val symtab = new collection.mutable.HashMap[String,BigInt]
}
