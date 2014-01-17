#ifndef _node_h
#define _node_h

#include "base.h"
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant.hpp>
#include <boost/optional.hpp>

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
    struct UOP { Enum id; const char *match; std::function<Result(Result)> apply; };
    struct BINOP { Enum id; const char *match; int prec; std::function<Result(Result,Result)> apply; };
    struct IOP { Enum id; const char *match; };
    extern UOP uopTable[U_MAX];
    extern BINOP binopTable[B_MAX];
    extern IOP iopTable[OP_MAX];

    //
    // AST structures
    //
    struct Expr;
    struct Identifier {
        std::string name;
        std::string toString() const { return name; }
    };
    struct QString {
        std::string value;
        std::string toString() const { return "\""+value+"\""; }
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
        inline std::string toString() const;
    };
    struct ExprTail {
        Enum binop;
        Term term;
        std::string toString() const { return binopTable[binop].match+term.toString(); }
    };
    struct Expr {
        Term term;
        std::vector<ExprTail> tail;
        std::string toString() const {
            std::string s = term.toString();
            for (auto it=tail.begin(); it!=tail.end(); it++) s+=it->toString();
            return s;
        }
    };

    struct Imm {
        Expr value;
        std::string toString() const { return "#"+value.toString(); }
    };
    struct Abs {
        Expr address;
        std::string toString() const { return address.toString(); }
    };
    struct AbsX {
        Expr address;
        std::string toString() const { return address.toString()+",x"; }
    };
    struct AbsY {
        Expr address;
        std::string toString() const { return address.toString()+",y"; }
    };
    struct IndX {
        Expr address;
        std::string toString() const { return "("+address.toString()+",x)"; }
    };
    struct IndY {
        Expr address;
        std::string toString() const { return "("+address.toString()+"),y"; }
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

        inline std::string toString() const;
    };

    struct _toStringVisitor: public boost::static_visitor<std::string> {
        std::string operator()(const unsigned a) const { return dec(a); }
        std::string operator()(const Identifier& a) const { return a.toString(); }
        std::string operator()(const boost::recursive_wrapper<Expr>& a) const { return "("+a.get().toString()+")"; }
        std::string operator()(const Imm& a) const { return a.toString(); }
        std::string operator()(const Abs& a) const { return a.toString(); }
        std::string operator()(const AbsX& a) const { return a.toString(); }
        std::string operator()(const AbsY& a) const { return a.toString(); }
        std::string operator()(const IndX& a) const { return a.toString(); }
        std::string operator()(const IndY& a) const { return a.toString(); }
        std::string operator()(const Label& a) const { return "Label("+a.name.name+")"; }
    };

    inline std::string Term::toString() const {
        std::string s;
        for (auto it=uop.begin(); it!=uop.end(); it++) s+=uopTable[*it].match;
        return s+boost::apply_visitor(_toStringVisitor(),value);
    }
    inline std::string Ins::toString() const {
        std::string rv;
        if (label) rv += label->name.name+":";
        if (iop) {
            while (rv.size() < 8) rv.push_back(' ');
            rv += iopTable[*iop].match;
            if (operand) rv += " "+boost::apply_visitor(_toStringVisitor(),*operand);
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
}

BOOST_FUSION_ADAPT_STRUCT(ast::Identifier,(std::string,name))
BOOST_FUSION_ADAPT_STRUCT(ast::QString,(std::string,value))
BOOST_FUSION_ADAPT_STRUCT(ast::Term,(std::vector<ast::Enum>,uop)(ast::Value,value))
BOOST_FUSION_ADAPT_STRUCT(ast::ExprTail,(ast::Enum,binop)(ast::Term,term))
BOOST_FUSION_ADAPT_STRUCT(ast::Expr,(ast::Term,term)(std::vector<ast::ExprTail>,tail))

BOOST_FUSION_ADAPT_STRUCT(ast::Imm,(ast::Expr,value));
BOOST_FUSION_ADAPT_STRUCT(ast::Abs,(ast::Expr,address));
BOOST_FUSION_ADAPT_STRUCT(ast::AbsX,(ast::Expr,address));
BOOST_FUSION_ADAPT_STRUCT(ast::AbsY,(ast::Expr,address));
BOOST_FUSION_ADAPT_STRUCT(ast::IndX,(ast::Expr,address));
BOOST_FUSION_ADAPT_STRUCT(ast::IndY,(ast::Expr,address));
BOOST_FUSION_ADAPT_STRUCT(ast::Label,(ast::Identifier,name));
BOOST_FUSION_ADAPT_STRUCT(ast::Ins,(boost::optional<ast::Label>,label)
                                   (boost::optional<ast::Enum>,iop)
                                   (boost::optional<ast::Oper>,operand));

BOOST_FUSION_ADAPT_STRUCT(ast::TestParseExpr,(std::vector<ast::Expr>,list))
BOOST_FUSION_ADAPT_STRUCT(ast::TestParseNumber,(std::vector<unsigned>,list))
BOOST_FUSION_ADAPT_STRUCT(ast::TestParseValue,(std::vector<ast::Value>,list))
BOOST_FUSION_ADAPT_STRUCT(ast::TestParseTerm,(std::vector<ast::Term>,list))
BOOST_FUSION_ADAPT_STRUCT(ast::TestParseString,(std::vector<ast::QString>,list))

#endif
