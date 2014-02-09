#include "base.h"
#include "parser.h"
#ifdef TEST
#include "tests/allTests.h"
#endif



#include "quark/quark.h"
#include "quark/test/test.h"


int main(int argc, const char * argv[])
{
#ifdef TEST
    quark::test::run();

#if 0
    test::AllTests all;
    if (!all.run()) return -1;
#endif
#endif

    /*
    quark::WideStringSource s1(L"123456");
    auto p = s1.getPos();
    std::wcout << s1.get() << s1.get() << s1.get();
    s1.setPos(p);
    std::wcout << s1.get() << s1.get() << s1.get();


    struct MyGrammar: public quark::Grammar {
        MyGrammar() {
            //start = quark::Lit(L"xdf")[quark::Rule<std::wstring>()];
        }

        quark::Rule<std::wstring> start;
    };
    struct MySkipper: public quark::Grammar {
        MySkipper() {
            //space = quark::Lit(L"xdf")[quark::Rule<std::wstring>()];
        }
        quark::Rule<std::string> space;
    };

    quark::WideStringSource s2(L"123456");
    MySkipper space;
    std::wstring out;
    MyGrammar g;
    g.parse(g.start,s2,space,&out);
    */



    /*
    quark::WStringSource src(L"Hello There");
    std::wcout << src.pop().value << src.pop().value << L"\n";

    quark::WStringSource src2(L"lllllx");
    auto tok = quark::Tokenizer<int,quark::WStringSource,long,wchar_t>(src2);
    tok.addToken(L"lll",300);
    tok.addToken(L"ll",200);
    tok.addToken(L"l",100);
    std::wcout << tok.pop().value << tok.pop().value << L"remain: " << src2.pop().value << L"\n";
    */

    //parser::parse();
    return 0;
}

