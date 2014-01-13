#include "ast.h"

namespace ast {
    //
    // unary operators
    //
    Result upos(const Result a) { return +a; }
    Result uneg(const Result a) { return -a; }
    Result unot(const Result a) { return ~a; }
    UOP uopTable[U_MAX] = {
            { upos },
            { uneg },
            { unot },
    };

    //
    // binary operators
    //
    Result badd(const Result a,const Result b) { return a+b; }
    Result bsub(const Result a,const Result b) { return a-b; }
    Result bmul(const Result a,const Result b) { return a*b; }
    Result bdiv(const Result a,const Result b) { return a/b; }
    Result bmod(const Result a,const Result b) { return a%b; }
    Result bshl(const Result a,const Result b) { return (b < 0) ? (a>>((-b)&63)) : (a<<(b&63)); }
    Result bshr(const Result a,const Result b) { return (b < 0) ? (a<<((-b)&63)) : (a>>(b&63)); }
    Result band(const Result a,const Result b) { return a&b; }
    Result bor(const Result a,const Result b) { return a|b; }
    Result bxor(const Result a,const Result b) { return a^b; }
    BINOP binopTable[B_MAX] = {
            {   0, NULL },
            { 300, badd },
            { 300, bsub },
            { 400, bmul },
            { 400, bdiv },
            { 400, bmod },
            { 120, band },
            { 100, bor },
            { 110, bxor },
            { 200, bshl },
            { 200, bshr },
    };

    //
    // instructions
    //
    IOP iopTable[OP_MAX] = {

    };
}