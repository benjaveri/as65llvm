#include "parser.h"

namespace parser {
    Grammar grammar;

    bool parse(std::string storage,ast::Program& out) {
        StringSource src(storage);
        return grammar.parse(src,&out);
    }
};