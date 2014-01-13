#ifndef _node_h
#define _node_h

#include "base.h"
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant.hpp>

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

        M_ACC = 0, M_IMM, M_ZP, M_ZPX, M_ABS, M_ABSX, M_ABSY, M_INDX, M_INDY, M_LABEL, M_MAX,
    };

    typedef signed long Result;
    struct UOP { Result (*apply)(const Result a); };
    struct BINOP { int prec; Result (*apply)(const Result a,const Result b); };
    struct IOP {};
    extern UOP uopTable[U_MAX];
    extern BINOP binopTable[B_MAX];
    extern IOP iopTable[OP_MAX];

    //
    // AST structures
    //
    struct Expr;
    struct Identifier { std::string name; };
    struct QString { std::string value; };
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
    };
    struct ExprTail {
        Enum binop;
        Term term;
    };
    struct Expr {
        Term term;
        std::vector<ExprTail> tail;
    };

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
BOOST_FUSION_ADAPT_STRUCT(ast::TestParseExpr,(std::vector<ast::Expr>,list))
BOOST_FUSION_ADAPT_STRUCT(ast::TestParseNumber,(std::vector<unsigned>,list))
BOOST_FUSION_ADAPT_STRUCT(ast::TestParseValue,(std::vector<ast::Value>,list))
BOOST_FUSION_ADAPT_STRUCT(ast::TestParseTerm,(std::vector<ast::Term>,list))
BOOST_FUSION_ADAPT_STRUCT(ast::TestParseString,(std::vector<ast::QString>,list))

#endif
