#include "quark.h"

namespace quark {

    //////////////////////////////////////////////////////////////////////////

    void ParseContext::skip() {
        struct Z: public boost::static_visitor<void> {
            Source *src;
            Z(Source* _):src(_) {}
            void operator()(const int& x) const {} // no skipper
            void operator()(const Rule& x) const {
                ParseContext spc(src);
                x.parse(spc);
            }
        };
        boost::apply_visitor(Z(source),skipper);
    }

    //////////////////////////////////////////////////////////////////////////

    bool Rule::_succ::parse(ParseContext& pc) const {
        ParseContext cpc(pc);
        auto pos = pc.source->getPos();
        if (!a.get().parse(cpc)) { pc.source->setPos(pos); return false; }
        if (!b.get().parse(cpc)) { pc.source->setPos(pos); return false; }
        pc.stack.append(cpc.stack);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    bool Rule::_or::parse(ParseContext& pc) const {
        ParseContext cpc(pc);
        auto pos = pc.source->getPos();
        if (!a.get().parse(cpc)) {
            pc.source->setPos(pos);
            if (!b.get().parse(cpc)) { pc.source->setPos(pos); return false; }
        }
        pc.stack.append(cpc.stack);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    bool Rule::_repsep::parse(ParseContext& pc) const {
        ParseContext cpc(pc);
        auto pos = pc.source->getPos();
        int count = 0;
        while (count<max) {
            if (!a.get().parse(cpc)) {
                if (count < min) { pc.source->setPos(pos); return false; }
                break;
            }
            count++;
            if (count<max) {
                ParseContext bpc(cpc);
                if (!b.get().parse(bpc)) { pc.source->setPos(pos); return false; }
                if (incl) cpc.stack.append(bpc.stack);
            }

        }
        pc.stack.push(cpc.stack); // push the stack
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    bool Rule::_lexeme::parse(ParseContext& pc) const {
        ParseContext cpc(pc.source); // no skipper
        auto pos = pc.source->getPos();
        if (!a.get().parse(cpc)) { pc.source->setPos(pos); return false; }
        pc.stack.append(cpc.stack);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    bool Rule::_nocase::parse(ParseContext& pc) const {
        NoCaseSource ncs(pc.source);
        ParseContext cpc(&ncs,pc.skipper);
        auto pos = cpc.source->getPos();
        if (!a.get().parse(cpc)) { cpc.source->setPos(pos); return false; }
        pc.stack.append(cpc.stack);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    bool Rule::_lit::parse(ParseContext& pc) const {
        ParseContext cpc(pc);
        auto pos = pc.source->getPos();
        pc.skip();
        auto start = pc.source->getPos();
        for (auto it = match.begin(); it!=match.end(); it++) {
            int ch = pc.source->get();
            if (ch != int(*it)) { pc.source->setPos(pos); return false; }
        }
        pc.stack.push(Source::Range(pc.source,start,pc.source->getPos()));
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    void Rule::_char::prepare() {
        for (size_t i=0; i<match.size();) {
            if (((i+2)<match.size()) && (match[i+1]=='-')) {
                quick.push_back(node(match[i+0],match[i+2]));
                i+=3;
            } else {
                quick.push_back(node(match[i],match[i]));
                i++;
            }
        }
    }
    
    bool Rule::_char::parse(ParseContext& pc) const {
        auto pos = pc.source->getPos();
        pc.skip();
        auto start = pc.source->getPos();
        int ch = pc.source->get();
        for (auto it=quick.begin(); it!=quick.end(); it++) {
            if ((ch>=it->min) && (ch<=it->max)) {
                pc.stack.push(Source::Range(pc.source,start,pc.source->getPos()));
                return true;
            }
        }
        pc.source->setPos(pos);
        return false;
    }

    //////////////////////////////////////////////////////////////////////////

    bool Rule::_cast::parse(ParseContext& pc) const {
        ParseContext cpc(pc);
        auto pos = pc.source->getPos();
        if (!a.get().parse(cpc)) { pc.source->setPos(pos); return false; }
        if (!f(cpc.stack)) { pc.source->setPos(pos); return false; }
        pc.stack.append(cpc.stack);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    //
    // rule application
    //
    bool Rule::parse(ParseContext& pc) const {
        struct Z: public boost::static_visitor<bool> {
            ParseContext& pc;
            Z(ParseContext& _):pc(_) {}
            bool operator()(const _succ& x) const { return x.parse(pc); }
            bool operator()(const _or& x) const { return x.parse(pc); }
            bool operator()(const _repsep& x) const { return x.parse(pc); }
            bool operator()(const _lexeme& x) const { return x.parse(pc); }
            bool operator()(const _nocase& x) const { return x.parse(pc); }
            bool operator()(const _lit& x) const { return x.parse(pc); }
            bool operator()(const _char& x) const { return x.parse(pc); }
            bool operator()(const _cast& x) const { return x.parse(pc); }
            bool operator()(const _eps& x) const { return x.parse(pc); }
            bool operator()(const _fail& x) const { return x.parse(pc); }
        };
        return boost::apply_visitor(Z(pc),subclass);
    }

    //
    // toString
    //
    std::wstring Rule::toString() const {
        struct Z: public boost::static_visitor<std::wstring> {
            std::wstring operator()(const _succ& x) const { return x.toString(); }
            std::wstring operator()(const _or& x) const { return x.toString(); }
            std::wstring operator()(const _repsep& x) const { return x.toString(); }
            std::wstring operator()(const _lexeme& x) const { return x.toString(); }
            std::wstring operator()(const _nocase& x) const { return x.toString(); }
            std::wstring operator()(const _lit& x) const { return x.toString(); }
            std::wstring operator()(const _char& x) const { return x.toString(); }
            std::wstring operator()(const _cast& x) const { return x.toString(); }
            std::wstring operator()(const _eps& x) const { return x.toString(); }
            std::wstring operator()(const _fail& x) const { return x.toString(); }
        };
        return boost::apply_visitor(Z(),subclass);
    }

    //////////////////////////////////////////////////////////////////////////

    Rule Int() {
        return (+Char("0-9"))([](Stack& stack)->bool{
            auto x = stack.back<Stack>().aggregateRange().get<std::string>();
            unsigned long i = 0;
            for (auto it=x.begin(); it!=x.end(); it++) i=(i*10)+(*it-'0');
            stack.drop();
            stack.push(i);
            return true;
        });
    }

    //////////////////////////////////////////////////////////////////////////

    Rule Hex() { // case sensitivity handled by source
        return (+Char("0-9a-f"))([](Stack& stack)->bool{
            auto x = stack.back<Stack>().aggregateRange().get<std::string>();
            unsigned long i = 0;
            for (auto it=x.begin(); it!=x.end(); it++) i=(i*16)+((*it>'9')?(*it-'a'+10):(*it-'0'));
            stack.drop();
            stack.push(i);
            return true;
        });
    }

}
