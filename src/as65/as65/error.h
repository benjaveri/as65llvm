#ifndef _error_h
#define _error_h

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
}

#endif