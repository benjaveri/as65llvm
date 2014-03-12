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

    bool Rule::_but::parse(ParseContext& pc) const {
        ParseContext cpc(pc);
        auto pos = pc.source->getPos();
        if (b.get().parse(cpc)) { pc.source->setPos(pos); return false; }
        if (!a.get().parse(cpc)) { pc.source->setPos(pos); return false; }
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

    bool Rule::_anychar::parse(ParseContext& pc) const {
        auto pos = pc.source->getPos();
        pc.skip();
        auto start = pc.source->getPos();
        if (Source::eoi == pc.source->get()) {
            pc.source->setPos(pos);
            return false;
        }
        pc.stack.push(Source::Range(pc.source,start,pc.source->getPos()));
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    bool Rule::_not::parse(ParseContext& pc) const {
        ParseContext cpc(pc);
        auto pos = pc.source->getPos();
        if (a.get().parse(cpc)) { pc.source->setPos(pos); return false; }
        pc.stack.append(cpc.stack);
        return true;
    }

    //////////////////////////////////////////////////////////////////////////

    bool Rule::_andpred::parse(ParseContext& pc) const {
        ParseContext cpc(pc);
        auto pos = pc.source->getPos();
        if (a.get().parse(cpc)) {
            pc.source->setPos(pos);
            pc.stack.append(cpc.stack);
            return true;
        } else {
            pc.source->setPos(pos);
            return false;
        }
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

    bool Rule::trace = false;
    bool Rule::setTrace(bool enable) {
        bool old = trace;
        trace = enable;
        return old;
    }

    bool Rule::followRef = false;
    bool Rule::setFollowRef(bool enable) {
        bool old = followRef;
        followRef = enable;
        return old;
    }

    int Rule::count = 0;
    std::wstring Rule::nextId() {
        std::wstringstream s;
        s << L"rule" << ++count;
        return s.str();
    }

    //
    // rule application
    //
    bool Rule::parse(ParseContext& pc) const {
        struct Z: public boost::static_visitor<bool> {
            ParseContext& pc;
            Z(ParseContext& _):pc(_) {}
            bool operator()(const _uninitialized& x) const { return x.parse(pc); }
            bool operator()(const _succ& x) const { return x.parse(pc); }
            bool operator()(const _or& x) const { return x.parse(pc); }
            bool operator()(const _but& x) const { return x.parse(pc); }
            bool operator()(const _repsep& x) const { return x.parse(pc); }
            bool operator()(const _lexeme& x) const { return x.parse(pc); }
            bool operator()(const _nocase& x) const { return x.parse(pc); }
            bool operator()(const _lit& x) const { return x.parse(pc); }
            bool operator()(const _char& x) const { return x.parse(pc); }
            bool operator()(const _anychar& x) const { return x.parse(pc); }
            bool operator()(const _not& x) const { return x.parse(pc); }
            bool operator()(const _andpred& x) const { return x.parse(pc); }
            bool operator()(const _cast& x) const { return x.parse(pc); }
            bool operator()(const _fwd& x) const { return x.parse(pc); }
            bool operator()(const _eps& x) const { return x.parse(pc); }
            bool operator()(const _fail& x) const { return x.parse(pc); }
            bool operator()(const _symtab& x) const { return x.parse(pc); }
        };
        bool rv = boost::apply_visitor(Z(pc),subclass);
        if (!rv && failHandler) { rv = failHandler.get()(pc); }
        if (trace) { // todo - make pos work for files
            std::wcout << "parse " << id << " returned " << (rv ? "true" : "false") << "; pos=" << pc.source->getPos().s << "\n";
            std::wcout.flush();
        }
        return rv;
    }

    //
    // toString
    //
    std::wstring Rule::toString() const {
        struct Z: public boost::static_visitor<std::wstring> {
            std::wstring operator()(const _uninitialized& x) const { return x.toString(); }
            std::wstring operator()(const _succ& x) const { return x.toString(); }
            std::wstring operator()(const _or& x) const { return x.toString(); }
            std::wstring operator()(const _but& x) const { return x.toString(); }
            std::wstring operator()(const _repsep& x) const { return x.toString(); }
            std::wstring operator()(const _lexeme& x) const { return x.toString(); }
            std::wstring operator()(const _nocase& x) const { return x.toString(); }
            std::wstring operator()(const _lit& x) const { return x.toString(); }
            std::wstring operator()(const _char& x) const { return x.toString(); }
            std::wstring operator()(const _anychar& x) const { return x.toString(); }
            std::wstring operator()(const _cast& x) const { return x.toString(); }
            std::wstring operator()(const _not& x) const { return x.toString(); }
            std::wstring operator()(const _andpred& x) const { return x.toString(); }
            std::wstring operator()(const _fwd& x) const { return x.toString(); }
            std::wstring operator()(const _eps& x) const { return x.toString(); }
            std::wstring operator()(const _fail& x) const { return x.toString(); }
            std::wstring operator()(const _symtab x) const { return x.toString(); }
        };
        return L"["+ id +L"]"+boost::apply_visitor(Z(),subclass);
    }

    //////////////////////////////////////////////////////////////////////////

    Rule Int(unsigned long radix) { // case sensitivity handled by source
        char match[] = "0-9a-z";
        if (radix > 9) {
            match[5] = 'a'+radix-10;
        } else {
            match[2] = '0'+radix;
            match[3] = 0;
        }
        return (+Char(match))([radix](Stack& stack)->bool{
            auto x = stack.back<Stack>().aggregateRange().get<std::string>();
            unsigned long i = 0;
            for (auto it=x.begin(); it!=x.end(); it++) {
                int digit = (*it>'9') ? (*it-'a'+10):(*it-'0');
                i = (i*radix)+digit;
            }
            stack.drop();
            stack.push(i);
            return true;
        });
    }

    //////////////////////////////////////////////////////////////////////////

    Rule Bin() { return Int(2); }
    Rule Oct() { return Int(8); }
    Rule Hex() { return Int(16); }

}
