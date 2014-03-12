#ifndef _quark_grammar_h
#define _quark_grammar_h

#include "rule.h"

// remove
#include <iostream>

namespace quark {
    struct Grammar {
        Rule start;
        Rule skipper;

        template<typename T>
        bool parse(Source& source,T *out) {
            //std::wcout << L"parse: " << start.toString() << L"\n";
            ParseContext pc;
            pc.source = &source;
            bool rv = start.parse(pc);
            if (rv) { *out = pc.stack.back<T>(); }
            return rv;
        }
    };
}

#endif
