#ifndef _parser_h
#define _parser_h

#if 0

#define BOOST_SPIRIT_DEBUG

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/repository/include/qi_confix.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include "base.h"
#include "ast.h"
#include "error.h"

namespace parser {
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace repo = boost::spirit::repository;
    namespace ph = boost::phoenix;

    template<typename Iterator>
    struct Skipper : qi::grammar<Iterator> {
        Skipper() : Skipper::base_type(skip) {
            using boost::spirit::qi::lit;
            using boost::spirit::ascii::char_;
            using boost::spirit::qi::eol;
            using boost::spirit::qi::eoi;

            skip = char_(" \t\v") | (lit(';') >> *(char_ - eos) >> eos);
            eos  = eol | eoi;
        }

        qi::rule<Iterator> skip;
        qi::rule<Iterator> eos;
    };

    template<typename Iterator>
    struct Grammar : qi::grammar<Iterator,ast::Program(),Skipper<Iterator>> {
        Grammar(ErrorHandler<Iterator>& error_handler) : Grammar::base_type(program) {
            using boost::spirit::qi::lit;
            using boost::spirit::qi::lexeme;
            using boost::spirit::ascii::char_;
            using boost::spirit::ascii::string;
            using boost::spirit::repository::confix;

            //
            // symbols
            //
            addSymbols(usym,ast::uopTable,ast::U_MAX);
            addSymbols(bsym,ast::binopTable,ast::B_MAX);
            addSymbols(isym,ast::iopTable,ast::OP_MAX);

            //
            // grammar
            //
            program         = qi::no_case[statement % qi::eol] >> qi::eoi;
            statement       = instruction
            #ifdef TEST
                            | testEscape
            #endif
                            ;

            eos             = qi::eol | qi::eoi;
            instruction     = (-label >> isym >> -operand >> &eos)
                            | (label >> &eos);
            label           = (identifier - isym) >> ':';
            operand         = (imm | indx | indy | absx | absy | abs);
            imm             = '#' >> expr;
            absx            = expr >> ',' >> 'x';
            absy            = expr >> ',' >> 'y';
            abs             = expr;
            indx            = '(' >> expr >> ',' >> 'x' >> ')';
            indy            = '(' >> expr >> ')' >> ',' >> 'y';

            expr            = term >> *exprTail;
            exprTail        = bsym > term;
            term            = *usym > value;
            value           = ('(' > expr > ')')
                            | (identifier - isym)
                            | number;
            number          = qi::as<unsigned>()[lexeme[qi::uint_ | (lit('$') > qi::hex) | (lit('%') > qi::bin)]];
            identifier      = qi::as_string[lexeme[char_("a-zA-Z_.") >> *char_("0-9a-zA-Z_")]];
            qstring         = lexeme['"' > qi::as_string[*((
                                  string("\\\\") | string("\\\"") | string("\\n") | string("\\t") | string("\\r") |
                                  string("\\v") | qi::as_string[string("\\x")>char_("0-9a-fA-F")>char_("0-9a-fA-F")] |
                                  char_
                              ) - ('"' | qi::eol | qi::eoi))] > '"'];

            testEscape      = lit("$$test$$") > (testParseString | testParseNumber | testParseValue | testParseTerm | testParseExpr);
            testParseString = lit("qstring") > (qstring % ",");
            testParseNumber = lit("number") > (number % ",");
            testParseValue  = lit("value") > (value % ",");
            testParseTerm   = lit("term") > (term % ",");
            testParseExpr   = lit("expr") > (expr % ",");

            program.name("program");
            statement.name("statement");
            instruction.name("instruction");
            label.name("label");
            operand.name("operand");
            expr.name("expression");
            exprTail.name("expression");
            term.name("term");
            value.name("value");
            number.name("number");
            identifier.name("identifier");
            qstring.name("string");

            typedef ph::function<parser::ErrorHandler<Iterator>> error_handler_function;
            using namespace qi::labels;
            qi::on_error<qi::fail>(program,error_handler_function(error_handler)("Error: expecting ", _4, _3));
            qi::on_error<qi::fail>(operand,error_handler_function(error_handler)("Error: expecting operand ", _4, _3));
            //qi::on_error<qi::fail>(expr,error_handler_function(error_handler)("Error: Expecting expression ", _4, _3));
            //qi::on_error<qi::fail>(term,error_handler_function(error_handler)("Error: Expecting term ", _4, _3));
//                std::cout << "Expected " << _4 << /*" at offset " << (ph::construct<Iterator>(_3)-_1) <<*/ " here '" << ph::construct<std::string>(_3,_2) << "'\n"
//            );

            BOOST_SPIRIT_DEBUG_NODE(instruction);
        }

        qi::symbols<char,ast::Enum> usym;
        qi::symbols<char,ast::Enum> bsym;
        qi::symbols<char,ast::Enum> isym;
        template<typename T,typename R> void addSymbols(T& sym,R* table,int count) {
            for (int i = 0; i < count; i++) {
                ast::Enum id = ast::Enum(i);
                assert(id==table[i].id); // sanity check
                sym.add(table[i].match,id);
            }
        }

        qi::rule<Iterator,ast::Program(),Skipper<Iterator>> program;
        qi::rule<Iterator,ast::Statement(),Skipper<Iterator>> statement;

        qi::rule<Iterator,void(),Skipper<Iterator>> eos;
        qi::rule<Iterator,ast::Ins(),Skipper<Iterator>> instruction;
        qi::rule<Iterator,ast::Label(),Skipper<Iterator>> labelDecl;
        qi::rule<Iterator,ast::Label(),Skipper<Iterator>> label;
        qi::rule<Iterator,ast::Oper(),Skipper<Iterator>> operand;
        qi::rule<Iterator,ast::Imm(),Skipper<Iterator>> imm;
        qi::rule<Iterator,ast::AbsX(),Skipper<Iterator>> absx;
        qi::rule<Iterator,ast::AbsY(),Skipper<Iterator>> absy;
        qi::rule<Iterator,ast::Abs(),Skipper<Iterator>> abs;
        qi::rule<Iterator,ast::IndX(),Skipper<Iterator>> indx;
        qi::rule<Iterator,ast::IndY(),Skipper<Iterator>> indy;

        qi::rule<Iterator,ast::Expr(),Skipper<Iterator>> expr;
        qi::rule<Iterator,ast::ExprTail(),Skipper<Iterator>> exprTail;
        qi::rule<Iterator,ast::Term(),Skipper<Iterator>> term;
        qi::rule<Iterator,ast::Value(),Skipper<Iterator>> value;
        qi::rule<Iterator,unsigned(),Skipper<Iterator>> number;

        qi::rule<Iterator,ast::Identifier(),Skipper<Iterator>> identifier;
        qi::rule<Iterator,ast::QString(),Skipper<Iterator>> qstring;

        qi::rule<Iterator,ast::Test(),Skipper<Iterator>> testEscape;
        qi::rule<Iterator,ast::TestParseString(),Skipper<Iterator>> testParseString;
        qi::rule<Iterator,ast::TestParseNumber(),Skipper<Iterator>> testParseNumber;
        qi::rule<Iterator,ast::TestParseValue(),Skipper<Iterator>> testParseValue;
        qi::rule<Iterator,ast::TestParseTerm(),Skipper<Iterator>> testParseTerm;
        qi::rule<Iterator,ast::TestParseExpr(),Skipper<Iterator>> testParseExpr;
    };

    bool parse(std::string storage,ast::Program& out) {
        using boost::spirit::ascii::space;
        using boost::spirit::qi::parse;

        typedef std::string::const_iterator Iterator;
        Iterator iter = storage.begin(),end = storage.end();
        ErrorHandler<Iterator> errorHandler(iter, end);
        Grammar<Iterator> grammar(errorHandler);
        Skipper<Iterator> skipper;

        if (qi::phrase_parse(iter,end,grammar,skipper,out)) {
            if (iter==end) {
                //std::cout << "parse succeeded!\n";
                return true;
            } else {
                //std::cout << "input not fully consumed: '" << std::string(iter,end) << "'\n";
                return false;
            }
        } else {
            //std::cout << "parse failed\n";
            return false;
        }
    }
}

#endif
#endif
