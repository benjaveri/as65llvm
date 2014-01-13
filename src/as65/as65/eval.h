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
            val = ast::uopTable[*it].apply(val);
        }
        return val;
    }

    Result evalExpr(const ast::Expr& expr) const {
        return BinaryEvaluator(*this).eval(expr);
    }

protected:
    // variation of the shunting-yard algorithm
    struct BinaryEvaluator {
        const Evaluator& E;
        std::stack<Result> num;
        std::stack<ast::Enum> opp;

        BinaryEvaluator(const Evaluator& _):E(_){}

        Result eval(const ast::Expr& expr) {
            ast::Enum anchor = ast::B_ANCHOR;
            opp.push(anchor); // place anchor
            num.push(E.evalTerm(expr.term));
            for (auto it = expr.tail.begin(); it != expr.tail.end(); it++) {
                int lp = prec(it->binop);
                while (lp <= prec(opp.top())) fold();
                opp.push(it->binop);
                num.push(E.evalTerm(it->term));
            }
            while (opp.top()!=anchor) fold();
            return num.top();
        }

        void fold() {
            ast::Enum op = opp.top(); opp.pop();
            Result b = num.top(); num.pop();
            Result a = num.top(); num.pop();
            Result r = apply(op,a,b);
            num.push(r);
        }

        inline int prec(ast::Enum op) { return ast::binopTable[op].prec; }
        inline Result apply(ast::Enum op,const Result a,const Result b) { return ast::binopTable[op].apply(a,b); }
    };

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
