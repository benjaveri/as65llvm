#ifndef _node_h
#define _node_h

#include "base.h"
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant.hpp>

namespace ast {
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
        std::vector<std::string> uop;
        Value value;
    };
    struct ExprTail {
        std::string binop;
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
BOOST_FUSION_ADAPT_STRUCT(ast::Term,(std::vector<std::string>,uop)(ast::Value,value))
BOOST_FUSION_ADAPT_STRUCT(ast::ExprTail,(std::string,binop)(ast::Term,term))
BOOST_FUSION_ADAPT_STRUCT(ast::Expr,(ast::Term,term)(std::vector<ast::ExprTail>,tail))
BOOST_FUSION_ADAPT_STRUCT(ast::TestParseExpr,(std::vector<ast::Expr>,list))
BOOST_FUSION_ADAPT_STRUCT(ast::TestParseNumber,(std::vector<unsigned>,list))
BOOST_FUSION_ADAPT_STRUCT(ast::TestParseValue,(std::vector<ast::Value>,list))
BOOST_FUSION_ADAPT_STRUCT(ast::TestParseTerm,(std::vector<ast::Term>,list))
BOOST_FUSION_ADAPT_STRUCT(ast::TestParseString,(std::vector<ast::QString>,list))

#endif
