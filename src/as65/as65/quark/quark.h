#ifndef _quark_h
#define _quark_h

/*
T
O    Rule operators
D
O   OPERATOR            ACTION                              GENERATED ARTIFACTS
    A >> B              match A then B                      A & B pushed sequentially
    A | B               match A or B                        A or B pushed
*   A - B               match A but not B                   A
    A(f)                apply f on A if A matches           f() operates on stack directly

*   -A                  dont match A                        none
    !A                  optionally match A                  Stack pushed
    *A                  match zero or more of A             Stack pushed
    +A                  one or more of A matched            Stack pushed
    A % B               one+ A separated by B               Stack pushed for A only
    &A                  matches but dont consume A          A

    Lit(x)              match x literally                   Source::Range
    Eps()               definite match (no consumption)     none
    Fail()              match nothing (no consumption)      none
    RepSep(A,B,n,x,i)   match A separated by B minimum      Stack pushed for A (and B
                        of n times and up to x times        if i is true)
*   Char(x)             regex character                     Source::Range
*   Int()               [0-9]+                              unsigned long

 */


#include "source.h"
#include "grammar.h"
#include "rule.h"

#endif
