#ifndef _quark_grammar_h
#define _quark_grammar_h

#include "rule.h"

// remove
#include <iostream>

namespace quark {
    template<typename T>
    struct Grammar {
        Rule start;

        Grammar(const Rule& _):start(_) {}

        bool parse(Source& source,T *out) {
            std::wcout << start.toString();

            ParseContext pc;
            pc.source = &source;
            bool rv = start.parse(pc);
            if (rv) { *out = pc.stack.back<T>(); }
            return rv;
        }
    };
}

#endif
