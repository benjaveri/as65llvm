#ifndef _parser_h
#define _parser_h

#include "base.h"
#include "ast.h"
#include "error.h"
#include "quark/quark.h"

namespace parser {
    using namespace quark;

    struct Grammar:public quark::Grammar {
        Grammar() {
            skipper = Char(" \t\v") | (Lit(';') >> *(Char() - eos) >> eos);


            //
            // symbols
            //
            addSymbols(usym,ast::uopTable,ast::U_MAX);
            addSymbols(bsym,ast::binopTable,ast::B_MAX);
            addSymbols(isym,ast::iopTable,ast::OP_MAX);

            //
            // grammar
            //


            eoi             = Lit(Source::eoi);
            eol             = Lit("\n"); // source turns /r/n into /n
            eos             = eol | eoi;
            number          = Lexeme(Int() | (Char('$') >> Hex()) | (Char('%') >> Bin()));
            identifier      = Lexeme(Char("a-zA-Z_.") >> *Char("0-9a-zA-Z_"));
            qstring         = Lexeme(Char('"')
                              >> *((
                                  Lit("\\\\") | Lit("\\\"") | Lit("\\n") |
                                      Lit("\\t") | Lit("\\r") | Lit("\\v") |
                                      (Lit("\\x")>>Char("0-9a-fA-F")>>Char("0-9a-fA-F")) |
                                      Char()) - (Char('"') | eol | eoi))
                              >> '"');


            value           = (Char('(') >> Ref(expr) >> ')')
                            | (identifier - isym)
                            | number;
            term            = *SymTab(usym) >> value;
            exprTail        = SymTab(bsym) >> term;
            expr            = term >> *exprTail;


            imm             = Char('#') >> expr;
            absx            = expr >> ',' >> 'x';
            absy            = expr >> ',' >> 'y';
            abs             = expr;
            indx            = Char('(') >> expr >> ',' >> 'x' >> ')';
            indy            = Char('(') >> expr >> ')' >> ',' >> 'y';
            operand         = (imm | indx | indy | absx | absy | abs);
            label           = (identifier - isym) >> ':';
            instruction     = (-label >> isym >> -operand >> &eos)
                            | (label >> &eos);



            statement       = instruction
#ifdef TEST
                            | Ref(testEscape)
#endif
                ;
            program         = NoCase(statement % eol) >> eoi;
            start           = program;


            testParseString = Lit("qstring") >> (qstring % Char(','));
            testParseNumber = Lit("number") >> (number % Char(','));
            testParseValue  = Lit("value") >> (value % Char(','));
            testParseTerm   = Lit("term") >> (term % Char(','));
            testParseExpr   = Lit("expr") >> (expr % Char(','));
            testEscape      = Lit("$$test$$") >> (testParseString | testParseNumber | testParseValue | testParseTerm | testParseExpr);


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
        }

        template<typename T,typename R> void addSymbols(T& sym,R* table,int count) {
            for (int i = 0; i < count; i++) {
                ast::Enum id = ast::Enum(i);
                assert(id==table[i].id); // sanity check
                sym.add(table[i].match,id);
            }
        }

        SymbolTable usym;
        SymbolTable bsym;
        SymbolTable isym;

        Rule program;
        Rule statement;

        Rule eoi,eol,eos;
        Rule instruction;
        Rule label;
        Rule  operand;
        Rule imm;
        Rule absx;
        Rule absy;
        Rule abs;
        Rule indx;
        Rule indy;

        Rule expr;
        Rule exprTail;
        Rule term;
        Rule value;
        Rule number;

        Rule identifier;
        Rule qstring;

        Rule testEscape;
        Rule testParseString;
        Rule testParseNumber;
        Rule testParseValue;
        Rule testParseTerm;
        Rule testParseExpr;
    };

    bool parse(std::string storage,ast::Program& out);
}

#endif
