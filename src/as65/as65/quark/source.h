#ifndef _quark_filter_h
#define _quark_filter_h

#include <string>
#include <stack>
#include <map>
#include <vector>
#include <boost/variant.hpp>
#include <boost/optional.hpp>
#include <stdio.h>

namespace quark {
    //
    // base source
    //
    struct Source {
        typedef union { size_t s; struct { fpos_t p; long r,c; } f; } PosT;
        struct Range {
            Source *source;
            PosT begin,end;

            //Range(){}
            Range(Source* s,const Source::PosT& b,const Source::PosT& e):source(s),begin(b),end(e) {}

            template<typename T>
            T get() const { return source->getRange<T>(*this); }
        };

        const static int eoi = -1;
        const static int invalid = -2;

        virtual int get() = 0; // -1 for eof
        virtual const PosT getPos() = 0;
        virtual void setPos(const PosT& p) = 0;
        virtual int toLower(int ch) = 0;

        template<typename T>
        T getRange(const Range &r) {
            auto v = getRangeV(r);
            T x; x.append(v.begin(),v.end());
            return x;
        }
        virtual std::vector<int> getRangeV(const Range &r) = 0;
    };

    //
    // std::[w]string source
    //
    template<class StringT,typename CharT>
    struct BasicStringSource: public Source {
        StringT ref;
        size_t idx;

        BasicStringSource(StringT _):ref(_),idx(0) {}

        virtual int get() { return (idx<ref.size()) ? int(ref.at(idx++)) : eoi; }
        virtual const PosT getPos() { PosT p; p.s=idx; return p; }
        virtual void setPos(const PosT& p) { idx = p.s; }
        virtual int toLower(int ch) { return ((ch>='A')&&(ch<='Z')) ? (ch+'a'-'A') : ch;}

        virtual std::vector<int> getRangeV(const Range &r) {
            auto pos = getPos();
            std::vector<int> v;
            setPos(r.begin);
            while (getPos().s != r.end.s) v.push_back(get());
            setPos(pos);
            return v;
        }
    };
    typedef BasicStringSource<std::string,char> StringSource;
    typedef BasicStringSource<std::wstring,wchar_t> WideStringSource;

    //
    // utf8 file source
    //
    struct UTF8FileSource: public Source {
        FILE *f;
        std::string filename;
        long row,col;

        UTF8FileSource(std::string _):filename(_),row(1),col(1),f(NULL) {
            f = fopen(filename.c_str(),"rb");
        }
        ~UTF8FileSource() {
            if (f) fclose(f);
        }

    protected:
        const int _rawget() {
            unsigned char _utf8lut[] = {
                0xfc,0x01,5,
                0xf8,0x03,4,
                0xf0,0x07,3,
                0xe0,0x0f,2,
                0xc0,0x1f,1,
                0x00
            };

            if (feof(f)) return eoi;
            int ch = fgetc(f);
            // trivial reject utf8 escape
            if (!(ch & 0x80)) return ch;
            // find appropriate escape
            for (auto p=_utf8lut; *p; p+=3) {
                if ((ch&*p)==*p) {
                    ch &= p[1];
                    for (int i=0; i < p[2]; i++) {
                        if (feof(f)) return invalid;
                        int c2 = fgetc(f);
                        if ((c2&0xc0)!=0x80) return invalid;
                        ch = (ch<<6)|(c2&0x3f);
                    }
                    return ch;
                }
            }
            // invalid encoding
            return invalid;
        }
    public:
        int get() {
            int ch = _rawget();
            switch (ch) {
                case 10: row++; col = 1; break;
                case 13: {
                    // gobble CR+LF as an atom (overlong utf8 LF wont work, e.g. c0 0a)
                    int c2 = fgetc(f);
                    if (c2 != 10) ungetc(c2,f);
                    row++; col = 1; break;
                }
                default: col++; break;
            }
            return ch;
        }
        const PosT getPos() const {
            PosT pos;
            fgetpos(f,&pos.f.p);
            pos.f.r = row;
            pos.f.c = col;
            return pos;
        }
        void setPos(const PosT& pos) {
            fsetpos(f,&pos.f.p);
            row = pos.f.r;
            col = pos.f.c;
        }

        virtual std::vector<int> getRangeV(const Range &r) {
            auto pos = getPos();
            std::vector<int> v;
            setPos(r.begin);
            while (getPos().f.p != r.end.f.p) v.push_back(get());
            setPos(pos);
            return v;
        }
    };

    //
    // case insensitive source wrapper
    //
    struct NoCaseSource: public Source {
        Source *source;

        NoCaseSource(Source *ps):source(ps) {}

        virtual int get() { return source->toLower(source->get()); }
        virtual const PosT getPos() { return source->getPos(); }
        virtual void setPos(const PosT& p) { return source->setPos(p); }
        virtual int toLower(int ch) { return source->toLower(ch); }
        virtual std::vector<int> getRangeV(const Range &r) {
            std::vector<int> w;
            auto v = source->getRangeV(r);
            for (auto it=v.begin(); it!=v.end(); it++) w.push_back(source->toLower(*it));
            return w;
        }
    };
}

#endif
