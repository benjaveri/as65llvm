#ifndef _quark_rule_h
#define _quark_rule_h

#include "source.h"
#include "symtab.h"
#include "context.h"

#include <vector>
#include <list>
#include <iostream>
#include <sstream>
#include <functional>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/any.hpp>

namespace quark {
    struct Rule {
        //
        // subclasses
        //
        struct _uninitialized {
            bool parse(ParseContext& pc) const { return false; };
            std::wstring toString() const { return L"uninitialized()"; }
        };
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
        struct _but {
            boost::recursive_wrapper<Rule> a,b;
            _but(const Rule& A,const Rule& B): a(A),b(B) {}
            bool parse(ParseContext& pc) const;
            std::wstring toString() const { return L"-("+a.get().toString()+L","+b.get().toString()+L")"; }
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
            _lit(char ch) { match.push_back(ch); }
            _lit(wchar_t ch) { match.push_back(ch); }
            _lit(int ch) { match.push_back(ch); }
            _lit(const char* sz) { while (*sz) match.push_back(*sz++); }
            _lit(const wchar_t* sz) { while (*sz) match.push_back(*sz++); }
            _lit(const std::string& a) { match.assign(a.begin(), a.end()); }
            _lit(const std::wstring& a) { match.assign(a.begin(), a.end()); }
            _lit(const std::vector<int>& a) { match.assign(a.begin(), a.end()); }

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
        struct _not {
            boost::recursive_wrapper<Rule> a;
            _not(const Rule& A): a(A) {}

            bool parse(ParseContext& pc) const;
            std::wstring toString() const { return L"not("+a.get().toString()+L")"; }
        };
        struct _andpred {
            boost::recursive_wrapper<Rule> a;
            _andpred(const Rule& A): a(A) {}

            bool parse(ParseContext& pc) const;
            std::wstring toString() const { return L"andpred("+a.get().toString()+L")"; }
        };
        struct _fwd {
            const Rule *p;
            _fwd(const Rule* P): p(P) {}

            bool parse(ParseContext& pc) const { return p->parse(pc); }
            std::wstring toString() const { return followRef ? L"fwd("+p->toString()+L")" : L"fwd("+p->id+L")"; }
        };
        struct _char {
            std::vector<int> match;
            struct node { int min,max; node(int n,int x):min(n),max(x){} };
            std::vector<node> quick; // yes, linear search is not best - optimize later (try: sort list by min; use binary search)
            void prepare();

        public:
            // assign c-style strings
            _char(int ch) { match.push_back(ch); }
            _char(const char* sz) { while (*sz) match.push_back(*sz++); prepare(); }
            _char(const wchar_t* sz) { while (*sz) match.push_back(*sz++); prepare(); }
            _char(const std::string& a) { match.assign(a.begin(), a.end()); prepare(); }
            _char(const std::wstring& a) { match.assign(a.begin(), a.end()); prepare(); }
            _char(const std::vector<int>& a) { match.assign(a.begin(), a.end()); prepare(); }

            bool parse(ParseContext& pc) const;
            std::wstring toString() const {
                std::wstring s; s.assign(match.begin(),match.end());
                return L"char("+s+L")";
            }
        };
        struct _anychar {
            bool parse(ParseContext& pc) const;
            std::wstring toString() const { return L"anychar()"; }
        };
        struct _eps {
            bool parse(ParseContext& pc) const { return true; };
            std::wstring toString() const { return L"eps()"; }
        };
        struct _fail {
            bool parse(ParseContext& pc) const { return false; };
            std::wstring toString() const { return L"fail()"; }
        };
        struct _symtab {
            SymbolTable* psymtab;

            _symtab(SymbolTable& s): psymtab(&s) {}

            bool parse(ParseContext& pc) const { return psymtab->parse(pc); }
            std::wstring toString() const { return L"symtab()"; }
        };

        static bool trace;
        static bool setTrace(bool enable);
        static bool followRef;
        static bool setFollowRef(bool enable);
        static int count;
        static std::wstring nextId();

        std::wstring id;
        boost::variant<
            _uninitialized,
            _succ,
            _or,
            _but,
            _repsep,
            _lexeme,
            _nocase,
            _lit,
            _char,
            _andpred,
            _anychar,
            _not,
            _cast,
            _fwd,
            _eps,
            _fail,
            _symtab
        > subclass;
        boost::optional<std::function<bool(ParseContext&)>> failHandler;

        //
        // creation & implicit promotion
        //
        Rule(): subclass(_uninitialized()) { id = nextId(); }
        Rule(int ch): subclass(_char(ch)) { id = nextId(); }
        Rule(std::string s): subclass(_lit(s)) { id = nextId(); }
        Rule(std::wstring s): subclass(_lit(s)) { id = nextId(); }
        Rule(std::vector<int> s): subclass(_lit(s)) { id = nextId(); }
        Rule(SymbolTable& s): subclass(_symtab(s)) { id = nextId(); }
        Rule(const _uninitialized& x): subclass(x) { id = nextId(); }
        Rule(const _succ& x): subclass(x) { id = nextId(); }
        Rule(const _or& x): subclass(x) { id = nextId(); }
        Rule(const _but& x): subclass(x) { id = nextId(); }
        Rule(const _repsep& x): subclass(x) { id = nextId(); }
        Rule(const _lexeme& x): subclass(x) { id = nextId(); }
        Rule(const _nocase& x): subclass(x) { id = nextId(); }
        Rule(const _lit& x): subclass(x) { id = nextId(); }
        Rule(const _char& x): subclass(x) { id = nextId(); }
        Rule(const _anychar& x): subclass(x) { id = nextId(); }
        Rule(const _not& x): subclass(x) { id = nextId(); }
        Rule(const _andpred& x): subclass(x) { id = nextId(); }
        Rule(const _cast& x): subclass(x) { id = nextId(); }
        Rule(const _fwd& x): subclass(x) { id = nextId(); }
        Rule(const _eps& x): subclass(x) { id = nextId(); }
        Rule(const _fail& x): subclass(x) { id = nextId(); }
        Rule(const _symtab& x): subclass(x) { id = nextId(); }

        //
        // interfaces
        //
        bool parse(ParseContext& pc) const;
        std::wstring toString() const;

        //
        // naming
        //
        void name(std::string n) { id.assign(n.begin(),n.end()); }
        void name(std::wstring n) { id = n; }
        void fail(std::function<bool(ParseContext&)> f) { failHandler.reset(f); }

        //
        // rule generators
        //
        Rule operator>>(const Rule& other) { return Rule(_succ(*this,other)); }
        Rule operator|(const Rule& other) { return Rule(_or(*this,other)); }
        Rule operator-(const Rule& other) { return Rule(_but(*this,other)); }
        Rule operator%(const Rule& other) { return Rule(_repsep(*this,other,1,-1,false)); }
        Rule operator!() { return Rule(_repsep(*this,Rule(Rule::_eps()),0,1,false)); }
        Rule operator+() { return Rule(_repsep(*this,Rule(Rule::_eps()),1,~0,false)); }
        Rule operator*() { return Rule(_repsep(*this,Rule(Rule::_eps()),0,~0,false)); }
        Rule operator-() { return Rule(_not(*this)); }
        Rule operator&() { return Rule(_not(*this)); }
        Rule operator()(_cast::function f) { return Rule(_cast(*this,f)); }
    };

    // matches a literal string (std::[w]string, [w]char*, vector<>)
    template<typename T> inline Rule Lit(T s) { return Rule(Rule::_lit(s)); }
    inline Rule Char() { return Rule(Rule::_anychar()); }
    template<typename T> inline Rule Char(T s) { return Rule(Rule::_char(s)); }

    // matches child, with some lexical exceptions
    inline Rule Lexeme(const Rule& child) { return Rule(Rule::_lexeme(child)); }
    inline Rule NoCase(const Rule& child) { return Rule(Rule::_nocase(child)); }

    // non-consuming rules
    inline Rule Eps() { return Rule(Rule::_eps()); }
    inline Rule Fail() { return Rule(Rule::_fail()); }

    // repsep
    inline Rule RepSep(Rule& A,Rule& B,int min,int max,bool incl) { return Rule(Rule::_repsep(A,B,min,max,incl)); }

    // symbol table
    inline Rule SymTab(SymbolTable& s) { return Rule(Rule::_symtab(s)); }

    // forward reference
    inline Rule Ref(const Rule& A) { return Rule(Rule::_fwd(&A)); }

    // compound rules
    Rule Int(unsigned long radix=10);
    Rule Bin();
    Rule Oct();
    Rule Hex();
}

#endif
