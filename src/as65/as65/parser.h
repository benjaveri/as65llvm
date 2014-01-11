#ifndef _parser_h
#define _parser_h

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

namespace parser {
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;
    namespace repo = boost::spirit::repository;
    namespace ph = boost::phoenix;

    // this class (error_handler) comes from the boost Qi compiler example.
    // Copyright (c) 2001-2011 Joel de Guzman
    // http://www.boost.org/LICENSE_1_0.txt
    template <typename Iterator>
    struct error_handler
    {
        template <typename, typename, typename>
        struct result { typedef void type; };

        error_handler(Iterator first, Iterator last): first(first), last(last) {}

        template <typename Message, typename What>
        void operator()(Message const& message,What const& what,Iterator err_pos) const {
            int line;
            Iterator line_start = get_pos(err_pos, line);
            if (err_pos != last) {
                std::cout << message << what << " line " << line << ':' << std::endl;
                std::cout << get_line(line_start) << std::endl;
                for (; line_start != err_pos; ++line_start)
                    std::cout << ' ';
                std::cout << '^' << std::endl;
            } else {
                std::cout << "Unexpected end of file. ";
                std::cout << message << what << " line " << line << std::endl;
            }
        }

        Iterator get_pos(Iterator err_pos, int& line) const {
            line = 1;
            Iterator i = first;
            Iterator line_start = first;
            while (i != err_pos)
            {
                bool eol = false;
                if (i != err_pos && *i == '\r') { // CR
                    eol = true;
                    line_start = ++i;
                }
                if (i != err_pos && *i == '\n') { // LF
                    eol = true;
                    line_start = ++i;
                }
                if (eol) ++line;
                else ++i;
            }
            return line_start;
        }

        std::string get_line(Iterator err_pos) const {
            Iterator i = err_pos;
            // position i to the next EOL
            while (i != last && (*i != '\r' && *i != '\n')) ++i;
            return std::string(err_pos, i);
        }

        Iterator first;
        Iterator last;
        std::vector<Iterator> iters;
    };


    template<typename Iterator>
    struct grammar : qi::grammar<Iterator,ast::Program(),ascii::space_type> {
        grammar(error_handler<Iterator>& error_handler) : grammar::base_type(program) {
            using boost::spirit::qi::lit;
            using boost::spirit::qi::lexeme;
            using boost::spirit::ascii::char_;
            using boost::spirit::ascii::string;
            using boost::spirit::repository::confix;

            program         = *statement >> qi::eoi;
            statement       = //qstring
                          //| identifier
            #ifdef TEST
                        /*|*/ testEscape
            #endif
            ;

            uop             = char_("-+~");
            binop           = string("<<") | string(">>") | char_("-+*/%&|^");
            expr            = term >> *exprTail;
            exprTail        = binop > term;
            term            = *uop > value;
            value           = ('(' > expr > ')')
                            | identifier
                            | number;
            number          = qi::as<unsigned>()[lexeme[qi::uint_ | (lit('$') > qi::hex) | (lit('%') > qi::bin)]];
            identifier      = qi::as_string[lexeme[char_("a-zA-Z_") >> *char_("0-9a-zA-Z_")]];
            qstring         = lexeme['"' > qi::as_string[*((
                                  string("\\\\") | string("\\\"") | string("\\n") | string("\\t") | string("\\r") |
                                  string("\\v") | qi::as_string[string("\\x")>char_("0-9a-fA-F")>char_("0-9a-fA-F")] |
                                  char_
                              ) - ('"' | qi::eol))] > '"'];

            testEscape      = lit("$$test$$") > (testParseString | testParseNumber | testParseValue | testParseTerm | testParseExpr);
            testParseString = lit("qstring") > (qstring % ",");
            testParseNumber = lit("number") > (number % ",");
            testParseValue  = lit("value") > (value % ",");
            testParseTerm   = lit("term") > (term % ",");
            testParseExpr   = lit("expr") > (expr % ",");

            program.name("program");
            statement.name("statement");
            uop.name("unary operator");
            binop.name("binary operator");
            expr.name("expression");
            exprTail.name("expression");
            term.name("term");
            value.name("value");
            number.name("number");
            identifier.name("identifier");
            qstring.name("string");

            typedef ph::function<parser::error_handler<Iterator>> error_handler_function;
            using namespace qi::labels;
            qi::on_error<qi::fail>(program,error_handler_function(error_handler)("Error: Expecting ", _4, _3));
//                std::cout << "Expected " << _4 << /*" at offset " << (ph::construct<Iterator>(_3)-_1) <<*/ " here '" << ph::construct<std::string>(_3,_2) << "'\n"
//            );
        }

        //qi::rule<Iterator> whitespace = char_(" \t") | qi::eol | (';' > *(char_ - qi::eol) > qi::eol);
        qi::rule<Iterator,ast::Program(),ascii::space_type> program;
        qi::rule<Iterator,ast::Statement(),ascii::space_type> statement;

        qi::rule<Iterator,std::string(),ascii::space_type> uop;
        qi::rule<Iterator,std::string(),ascii::space_type> binop;
        qi::rule<Iterator,ast::Expr(),ascii::space_type> expr;
        qi::rule<Iterator,ast::ExprTail(),ascii::space_type> exprTail;
        qi::rule<Iterator,ast::Term(),ascii::space_type> term;
        qi::rule<Iterator,ast::Value(),ascii::space_type> value;
        qi::rule<Iterator,unsigned(),ascii::space_type> number;

        qi::rule<Iterator,ast::Identifier(),ascii::space_type> identifier;
        qi::rule<Iterator,ast::QString(),ascii::space_type> qstring;

        qi::rule<Iterator,ast::Test(),ascii::space_type> testEscape;
        qi::rule<Iterator,ast::TestParseString(),ascii::space_type> testParseString;
        qi::rule<Iterator,ast::TestParseNumber(),ascii::space_type> testParseNumber;
        qi::rule<Iterator,ast::TestParseValue(),ascii::space_type> testParseValue;
        qi::rule<Iterator,ast::TestParseTerm(),ascii::space_type> testParseTerm;
        qi::rule<Iterator,ast::TestParseExpr(),ascii::space_type> testParseExpr;
    };

    bool parse(std::string storage,ast::Program& out) {
        using boost::spirit::ascii::space;
        using boost::spirit::qi::parse;

        typedef std::string::const_iterator Iterator;
        Iterator iter = storage.begin(),end = storage.end();
        error_handler<Iterator> error_handler(iter, end);
        grammar<std::string::const_iterator> xu(error_handler);
        if (qi::phrase_parse(iter,end,xu,space,out)) {
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
