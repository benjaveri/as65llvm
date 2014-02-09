#ifndef _quark_rule_h
#define _quark_rule_h

#include "source.h"

#include <vector>
#include <list>
#include <iostream>
#include <sstream>
#include <functional>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/any.hpp>

namespace quark {
    //
    // artifact stack
    //
    struct Stack {
        std::list<boost::any> stack;

        void push(boost::any e) { stack.push_back(e); }
        template<typename T> T front() { return boost::any_cast<T>(stack.front()); }
        template<typename T> T back() { return boost::any_cast<T>(stack.back()); }
        void drop() { stack.pop_back(); }
        void clear() { stack.clear(); }

        // assuming front & back are ranges, return the aggregate range
        Source::Range aggregateRange() {
            auto r0 = front<Source::Range>();
            auto r1 = back<Source::Range>();
            return Source::Range(r0.source,r0.begin,r1.end);
        }

        void append(const Stack& other) {
            stack.insert(stack.end(),other.stack.begin(),other.stack.end());
        }
    };

    //
    // parse context (binds source, skipper and artifact stack)
    //
    struct Rule;
    struct ParseContext {
        Source* source; // source being parsed
        typedef boost::variant<
            int,
            boost::recursive_wrapper<Rule>
        > skipperType;
        skipperType skipper;      // whitespace skipper
        Stack stack;    // artifact stack

        //
        // construction
        //
        ParseContext(): source(NULL),skipper(0) {}
        ParseContext(Source* src): source(src),skipper(0) {}
        ParseContext(Source* src,const Rule& skip): source(src),skipper(skip) {}
        ParseContext(Source* src,const skipperType& skip): source(src),skipper(skip) {}
        ParseContext(const ParseContext& pc): source(pc.source),skipper(pc.skipper) {}

        //
        // invoke skipper
        //
        void skip();
    };

    //
    // generic rule
    //
    struct Rule {
        //
        // subclasses
        //

        struct _succ {
            boost::recursive_wrapper<Rule> a,b;
            _succ(const Rule& A,const Rule& B): a(A),b(B) {}
            bool parse(ParseContext& pc) const;
            std::wstring toString() const { return L">>("+a.get().toString()+L","+b.get().toString()+L")"; }
        };
        struct _or {
            boost::recursive_wrapper<Rule> a,b;
            _or(const Rule& A,const Rule& B): a(A),b(B) {}
            bool parse(ParseContext& pc) const;
            std::wstring toString() const { return L"|("+a.get().toString()+L","+b.get().toString()+L")"; }
        };
        struct _repsep {
            boost::recursive_wrapper<Rule> a,b;
            unsigned long min,max;
            bool incl;
            _repsep(const Rule& A,const Rule& B,unsigned long n,unsigned long x,bool i): a(A),b(B),min(n),max(x),incl(i) {}
            bool parse(ParseContext& pc) const;
            std::wstring toString() const { std::wstringstream s; s<<L"repsep("<<a.get().toString()<<L","<<b.get().toString()<<L","<<min<<L","<<max<<L","<<incl<<L")"; return s.str(); }
        };
        struct _lexeme {
            boost::recursive_wrapper<Rule> a;
            _lexeme(const Rule& A): a(A) {}
            bool parse(ParseContext& pc) const;
            std::wstring toString() const { return L"lexeme("+a.get().toString()+L")"; }
        };
        struct _nocase {
            boost::recursive_wrapper<Rule> a;
            _nocase(const Rule& A): a(A) {}
            bool parse(ParseContext& pc) const;
            std::wstring toString() const { return L"nocase("+a.get().toString()+L")"; }
        };
        struct _lit {
            std::vector<int> match;

            // assign c-style strings
            _lit(const char* sz) { while (*sz) match.push_back(*sz++); }
            _lit(const wchar_t* sz) { while (*sz) match.push_back(*sz++); }
            template<typename T> _lit(const T& a) { match.assign(a.begin(), a.end()); }

            bool parse(ParseContext& pc) const;
            std::wstring toString() const {
                std::wstring s; s.assign(match.begin(),match.end());
                return L"lit("+s+L")";
            }
        };
        struct _cast {
            typedef std::function<bool(Stack&)> function;
            boost::recursive_wrapper<Rule> a;
            function f;
            _cast(const Rule& A,function F): a(A),f(F) {}

            bool parse(ParseContext& pc) const;
            std::wstring toString() const { return L"cast("+a.get().toString()+L")"; }
        };
        struct _char {
            std::vector<int> match;
            struct node { int min,max; node(int n,int x):min(n),max(x){} };
            std::vector<node> quick; // yes, linear search is not best - optimize later (try: sort list by min; use binary search)
            void prepare();

        public:
            // assign c-style strings
            _char(const char* sz) { while (*sz) match.push_back(*sz++); prepare(); }
            _char(const wchar_t* sz) { while (*sz) match.push_back(*sz++); prepare(); }
            template<typename T> _char(const T&a) { match.assign(a.begin(), a.end()); prepare(); }

            bool parse(ParseContext& pc) const;
            std::wstring toString() const {
                std::wstring s; s.assign(match.begin(),match.end());
                return L"char("+s+L")";
            }
        };
        struct _eps {
            bool parse(ParseContext& pc) const { return true; };
            std::wstring toString() const { return L"eps()"; }
        };
        struct _fail {
            bool parse(ParseContext& pc) const { return false; };
            std::wstring toString() const { return L"fail()"; }
        };

        boost::variant<
            _succ,
            _or,
            _repsep,
            _lexeme,
            _nocase,
            _lit,
            _char,
            _cast,
            _eps,
            _fail
        > subclass;

        //
        // interfaces
        //
        template<typename X> Rule(const X& sub): subclass(sub) {}
        bool parse(ParseContext& pc) const;
        std::wstring toString() const;

        //
        // rule generators
        //
        Rule operator>>(const Rule& other) { return Rule(_succ(*this,other)); }
        Rule operator|(const Rule& other) { return Rule(_or(*this,other)); }
        Rule operator%(const Rule& other) { return Rule(_repsep(*this,other,1,-1,false)); }
        Rule operator!() { return Rule(_repsep(*this,Rule(Rule::_eps()),0,1,false)); }
        Rule operator+() { return Rule(_repsep(*this,Rule(Rule::_eps()),1,~0,false)); }
        Rule operator*() { return Rule(_repsep(*this,Rule(Rule::_eps()),0,~0,false)); }
        Rule operator()(_cast::function f) { return Rule(_cast(*this,f)); }
    };

    // matches a literal string (std::[w]string, [w]char*, vector<>)
    template<typename T> inline Rule Lit(T s) { return Rule(Rule::_lit(s)); }
    template<typename T> inline Rule Char(T s) { return Rule(Rule::_char(s)); }

    // matches child, with some lexical exceptions
    inline Rule Lexeme(const Rule& child) { return Rule(Rule::_lexeme(child)); }
    inline Rule NoCase(const Rule& child) { return Rule(Rule::_nocase(child)); }

    // non-consuming rules
    inline Rule Eps() { return Rule(Rule::_eps()); }
    inline Rule Fail() { return Rule(Rule::_fail()); }

    // repsep
    inline Rule RepSep(Rule A,Rule B,int min,int max,bool incl) { return Rule(Rule::_repsep(A,B,min,max,incl)); }

    // compound rules
    Rule Int();
    Rule Hex();
}

#endif
