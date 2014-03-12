#ifndef _quark_context_h
#define _quark_context_h

#include "source.h"

#include <vector>
#include <list>
#include <iostream>
#include <sstream>
#include <functional>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <boost/any.hpp>

namespace quark {
    //
    // artifact stack
    //
    struct Stack {
        std::list<boost::any> stack;

        void push(boost::any e) { stack.push_back(e); }
        template<typename T> T front() { return boost::any_cast<T>(stack.front()); }
        template<typename T> T back() { return boost::any_cast<T>(stack.back()); }
        void drop() { stack.pop_back(); }
        void clear() { stack.clear(); }

        // assuming front & back are ranges, return the aggregate range
        Source::Range aggregateRange() {
            auto r0 = front<Source::Range>();
            auto r1 = back<Source::Range>();
            return Source::Range(r0.source,r0.begin,r1.end);
        }

        void append(const Stack& other) {
            stack.insert(stack.end(),other.stack.begin(),other.stack.end());
        }
    };

    //
    // parse context (binds source, skipper and artifact stack)
    //
    struct Rule;
    struct ParseContext {
        Source* source; // source being parsed
        typedef boost::variant<
        int,
        boost::recursive_wrapper<Rule>
        > skipperType;
        skipperType skipper;      // whitespace skipper
        Stack stack;    // artifact stack

        //
        // construction
        //
        ParseContext(): source(NULL),skipper(0) {}
        ParseContext(Source* src): source(src),skipper(0) {}
        ParseContext(Source* src,const Rule& skip): source(src),skipper(skip) {}
        ParseContext(Source* src,const skipperType& skip): source(src),skipper(skip) {}
        ParseContext(const ParseContext& pc): source(pc.source),skipper(pc.skipper) {}

        //
        // context helpers
        //
        void skip();
    };
}

#endif