#include "base.h"

#if 0
#include "ast.h"

namespace ast {
    //
    // unary operators
    //
    UOP uopTable[U_MAX] = {
        { U_POS, "+", [](Result a){ return +a; } },
        { U_NEG, "-", [](Result a){ return -a; } },
        { U_NOT, "~", [](Result a){ return ~a; } },
    };

    //
    // binary operators
    //
    BINOP binopTable[B_MAX] = {
        { B_ANCHOR, "",   0,   NULL },
        { B_ADD,    "+",  300, [](Result a,Result b) { return a+b; } },
        { B_SUB,    "-",  300, [](Result a,Result b) { return a-b; } },
        { B_MUL,    "*",  400, [](Result a,Result b) { return a*b; } },
        { B_DIV,    "/",  400, [](Result a,Result b) { return a/b; } },
        { B_MOD,    "%",  400, [](Result a,Result b) { return a%b; } },
        { B_AND,    "&",  120, [](Result a,Result b) { return a&b; } },
        { B_OR,     "|",  100, [](Result a,Result b) { return a|b; } },
        { B_XOR,    "^",  110, [](Result a,Result b) { return a^b; } },
        { B_SHL,    "<<", 200, [](Result a,Result b) { return (b < 0) ? (a>>((-b)&63)) : (a<<(b&63)); } },
        { B_SHR,    ">>", 200, [](Result a,Result b) { return (b < 0) ? (a<<((-b)&63)) : (a>>(b&63)); } },
    };

    //
    // instructions
    //
    IOP iopTable[OP_MAX] = {
        { OP_ADC, "adc" },
        { OP_AND, "and" },
        { OP_ASL, "asl" },
        { OP_BCC, "bcc" },
        { OP_BCS, "bcs" },
        { OP_BEQ, "beq" },
        { OP_BIT, "bit" },
        { OP_BMI, "bmi" },
        { OP_BNE, "bne" },
        { OP_BPL, "bpl" },
        { OP_BRK, "brk" },
        { OP_BVC, "bvc" },
        { OP_BVS, "bvs" },
        { OP_CLC, "clc" },
        { OP_CLD, "cld" },
        { OP_CLI, "cli" },
        { OP_CLV, "clv" },
        { OP_CMP, "cmp" },
        { OP_CPX, "cpx" },
        { OP_CPY, "cpy" },
        { OP_DB,  "db"  },
        { OP_DEC, "dec" },
        { OP_DEX, "dex" },
        { OP_DEY, "dey" },
        { OP_EOR, "eor" },
        { OP_ESC, "esc" },
        { OP_INC, "inc" },
        { OP_INX, "inx" },
        { OP_INY, "iny" },
        { OP_JMP, "jmp" },
        { OP_JSR, "jsr" },
        { OP_LDA, "lda" },
        { OP_LDX, "ldx" },
        { OP_LDY, "ldy" },
        { OP_LSR, "lsr" },
        { OP_NOP, "nop" },
        { OP_ORA, "ora" },
        { OP_PHA, "pha" },
        { OP_PHP, "php" },
        { OP_PLA, "pla" },
        { OP_PLP, "plp" },
        { OP_ROL, "rol" },
        { OP_ROR, "ror" },
        { OP_RTI, "rti" },
        { OP_RTS, "rts" },
        { OP_SBC, "sbc" },
        { OP_SEC, "sec" },
        { OP_SED, "sed" },
        { OP_SEI, "sei" },
        { OP_STA, "sta" },
        { OP_STX, "stx" },
        { OP_STY, "sty" },
        { OP_TAX, "tax" },
        { OP_TAY, "tay" },
        { OP_TSX, "tsx" },
        { OP_TXA, "txa" },
        { OP_TXS, "txs" },
        { OP_TYA, "tya" }
    };
//         OP_ADC = 0, OP_AND, OP_ASL,
//         OP_BCC, OP_BCS, OP_BEQ, OP_BIT, OP_BMI, OP_BNE, OP_BPL, OP_BRK, OP_BVC, OP_BVS,
//         OP_CLC, OP_CLD, OP_CLI, OP_CLV, OP_CMP, OP_CPX, OP_CPY,
//         OP_DB, OP_DEC, OP_DEX, OP_DEY,
//         OP_EOR, OP_ESC,
//         OP_INC, OP_INX, OP_INY,
//         OP_JMP, OP_JSR,
//         OP_LDA, OP_LDX, OP_LDY, OP_LSR,
//         OP_NOP,
//         OP_ORA,
//         OP_PHA, OP_PHP, OP_PLA, OP_PLP,
//         OP_ROL,
//         OP_ROR, OP_RTI, OP_RTS,
//         OP_SBC, OP_SEC, OP_SED, OP_SEI, OP_STA, OP_STX, OP_STY,
//         OP_TAX, OP_TAY, OP_TSX, OP_TXA, OP_TXS, OP_TYA,

}

#endif
