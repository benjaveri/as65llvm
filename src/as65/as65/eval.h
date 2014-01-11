#ifndef _eval_h
#define _eval_h

#include "base.h"
#include "ast.h"

typedef signed long EResult;
struct Evaluator: public boost::static_visitor<EResult> {
    struct Exception { std::string reason; Exception(std::string _):reason(_){} };
    struct SymbolNotFoundException: public Exception { SymbolNotFoundException(std::string _):Exception(_){} };
    struct RuntimeException: public Exception { RuntimeException(std::string _):Exception(_){} };

    typedef EResult Result;
    typedef boost::variant<Result,ast::Expr> SymVal;

    std::map<std::string,SymVal> symtab; // symbol table, stored as a result or a subexpression
    std::map<std::string,int> prec;      // binary operator precedence table

    Evaluator() {
        prec["*"]=400; prec["'"]=400; prec["%"]=400;
        prec["+"]=300; prec["-"]=300;
        prec["<<"]=200; prec[">>"]=200;
        prec["&"]=120;
        prec["^"]=110;
        prec["|"]=100;
        prec[""]=0; // anchor
    }

    Result evalIdentifier(const ast::Identifier& id) const {
        const auto it = symtab.find(id.name);
        if (it == symtab.end()) throw new SymbolNotFoundException(id.name);
        return boost::apply_visitor(*this,it->second);
    }

    Result evalValue(const ast::Value& val) const {
        return boost::apply_visitor(*this,val);
    }

    Result evalTerm(const ast::Term& term) const {
        Result val = evalValue(term.value);
        for (auto it = term.uop.rbegin(); it != term.uop.rend(); it++) {
            if (*it=="+") {}
            else if (*it=="-") val = -val;
            else if (*it=="~") val = ~val;
            else throw new RuntimeException("Incomplete uop implementation");
        }
        return val;
    }

    // variation of the shunting-yard algorithm
    Result evalExpr(const ast::Expr& expr) const {
        std::stack<Result> num;
        std::stack<std::string> opp;
        opp.push(""); // place anchor
        num.push(evalTerm(expr.term));
        for (auto it = expr.tail.begin(); it != expr.tail.end(); it++) {
            int lp = _bprec(it->binop);
            while (lp <= _bprec(opp.top())) _bfold(num,opp);
            opp.push(it->binop);
            num.push(evalTerm(it->term));
        }
        while (!opp.top().empty()) _bfold(num,opp);
        return num.top();
    }

protected:
    inline int _bprec(std::string op) const { return prec.find(op)->second; }

    void _bfold(std::stack<Result>& num,std::stack<std::string>& opp) const {
        std::string& op = opp.top(); opp.pop();
        Result b = num.top(); num.pop();
        Result a = num.top(); num.pop();
        if (op=="+") a += b;
        else if (op=="-") a -= b;
        else if (op=="*") a *= b;
        else if (op=="/") a /= b;
        else if (op=="%") a %= b;
        else if (op=="&") a &= b;
        else if (op=="|") a |= b;
        else if (op=="^") a ^= b;
        else if (op=="<<") a <<= b & 63;
        else if (op==">>") a >>= b & 63;
        else throw new RuntimeException("Incomplete binop implementation");
        num.push(a);
    }

public:
    Result operator()(const Result& ee) const {
        return ee;
    }
    Result operator()(const unsigned& ee) const {
        return Result(ee);
    }
    Result operator()(const ast::Identifier& ee) const {
        return evalIdentifier(ee);
    }
    Result operator()(const ast::Term& ee) const {
        return evalTerm(ee);
    }
    Result operator()(const ast::Expr& ee) const {
        return evalExpr(ee);
    }
};

#endif
