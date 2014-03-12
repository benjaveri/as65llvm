#ifndef _quark_symtab_h
#define _quark_symtab_h

#include "context.h"

#include <vector>
#include <map>
#include <boost/any.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace quark {
    struct SymbolTable {
    protected:
        struct node;
        typedef boost::variant<
            int,
            boost::recursive_wrapper<node>
        > ref;
        struct node {
            int match;
            boost::optional<boost::any> value;
            std::map<int,ref> children;
        };
        ref root;

    public:
        template<typename T>
        bool add(T vec,boost::any result) {
            return false;
        }

        bool parse(ParseContext& pc) const {
            return false;
        }
    };
}

#endif
