#include "base.h"

#include "ast.h"

namespace ast {
    //
    // unary operators
    //
    UOP uopTable[U_MAX] = {
        { U_POS, L"+", [](Result a){ return +a; } },
        { U_NEG, L"-", [](Result a){ return -a; } },
        { U_NOT, L"~", [](Result a){ return ~a; } },
    };

    //
    // binary operators
    //
    BINOP binopTable[B_MAX] = {
        { B_ANCHOR, L"",   0,   NULL },
        { B_ADD,    L"+",  300, [](Result a,Result b) { return a+b; } },
        { B_SUB,    L"-",  300, [](Result a,Result b) { return a-b; } },
        { B_MUL,    L"*",  400, [](Result a,Result b) { return a*b; } },
        { B_DIV,    L"/",  400, [](Result a,Result b) { return a/b; } },
        { B_MOD,    L"%",  400, [](Result a,Result b) { return a%b; } },
        { B_AND,    L"&",  120, [](Result a,Result b) { return a&b; } },
        { B_OR,     L"|",  100, [](Result a,Result b) { return a|b; } },
        { B_XOR,    L"^",  110, [](Result a,Result b) { return a^b; } },
        { B_SHL,    L"<<", 200, [](Result a,Result b) { return (b < 0) ? (a>>((-b)&63)) : (a<<(b&63)); } },
        { B_SHR,    L">>", 200, [](Result a,Result b) { return (b < 0) ? (a<<((-b)&63)) : (a>>(b&63)); } },
    };

    //
    // instructions
    //
    IOP iopTable[OP_MAX] = {
        { OP_ADC, L"adc" },
        { OP_AND, L"and" },
        { OP_ASL, L"asl" },
        { OP_BCC, L"bcc" },
        { OP_BCS, L"bcs" },
        { OP_BEQ, L"beq" },
        { OP_BIT, L"bit" },
        { OP_BMI, L"bmi" },
        { OP_BNE, L"bne" },
        { OP_BPL, L"bpl" },
        { OP_BRK, L"brk" },
        { OP_BVC, L"bvc" },
        { OP_BVS, L"bvs" },
        { OP_CLC, L"clc" },
        { OP_CLD, L"cld" },
        { OP_CLI, L"cli" },
        { OP_CLV, L"clv" },
        { OP_CMP, L"cmp" },
        { OP_CPX, L"cpx" },
        { OP_CPY, L"cpy" },
        { OP_DB,  L"db"  },
        { OP_DEC, L"dec" },
        { OP_DEX, L"dex" },
        { OP_DEY, L"dey" },
        { OP_EOR, L"eor" },
        { OP_ESC, L"esc" },
        { OP_INC, L"inc" },
        { OP_INX, L"inx" },
        { OP_INY, L"iny" },
        { OP_JMP, L"jmp" },
        { OP_JSR, L"jsr" },
        { OP_LDA, L"lda" },
        { OP_LDX, L"ldx" },
        { OP_LDY, L"ldy" },
        { OP_LSR, L"lsr" },
        { OP_NOP, L"nop" },
        { OP_ORA, L"ora" },
        { OP_PHA, L"pha" },
        { OP_PHP, L"php" },
        { OP_PLA, L"pla" },
        { OP_PLP, L"plp" },
        { OP_ROL, L"rol" },
        { OP_ROR, L"ror" },
        { OP_RTI, L"rti" },
        { OP_RTS, L"rts" },
        { OP_SBC, L"sbc" },
        { OP_SEC, L"sec" },
        { OP_SED, L"sed" },
        { OP_SEI, L"sei" },
        { OP_STA, L"sta" },
        { OP_STX, L"stx" },
        { OP_STY, L"sty" },
        { OP_TAX, L"tax" },
        { OP_TAY, L"tay" },
        { OP_TSX, L"tsx" },
        { OP_TXA, L"txa" },
        { OP_TXS, L"txs" },
        { OP_TYA, L"tya" }
    };
}
