#include "../quark.h"
#include <boost/variant.hpp>

namespace quark {
    namespace test {
        struct SimpleTest {
            bool run() {
                if (!litSimpleTest()) return false;
                if (!orSimpleTest()) return false;
                if (!succSimpleTest()) return false;
                if (!CharTest()) return false;
                return  true;
            }

            bool fail(std::string msg) {
                std::cout << "FAILURE: "<<msg<<"\n";
                return false;
            }

            bool litSimpleTest() {
                {
                    StringSource ss("xxx");
                    ParseContext pc;
                    pc.source = &ss;
                    auto reset = pc.source->getPos();
                    if (!Lit("x").parse(pc)) return fail("'x' parse failed");
                    pc.source->setPos(reset);
                    if (!Lit("xxx").parse(pc)) return fail("'xxx' parse failed");
                    pc.source->setPos(reset);
                    if (Lit("xxy").parse(pc)) return fail("'xxy' parse succeeded");
                }

                {
                    StringSource ss("123");
                    ParseContext pc(&ss);
                    Rule int_ = (Lit("1") >> Lit("2") >> Lit("3"))([](Stack&stack)->bool{
                        auto r = stack.aggregateRange().get<std::string>();
                        int i = 0;
                        for (auto it=r.begin(); it!=r.end(); it++) i=i*10+(*it-'0');
                        stack.clear();
                        stack.push(i);
                        return true;
                    });
                    //std::wcout << int_.toString() << "\n";
                    if (!int_.parse(pc)) return fail("'int_' parse failed");
                    auto v = pc.stack.back<int>();
                    if (v != 123) return fail("'int_' cast failed");
                }

                return true;
            }

            bool orSimpleTest() {
                Rule OR = +(Lit("x") | Lit("y") | Lit("z"));

                {
                    StringSource ss("xxx");
                    ParseContext pc;
                    pc.source = &ss;
                    if (!OR.parse(pc)) return fail("'xxx' parse failed");
                    auto x = pc.stack.back<Stack>();
                    auto v = x.aggregateRange().get<std::string>();
                    if (v!="xxx") return fail("'xxx' stack invalid");
                }
                {
                    StringSource ss("yxxz");
                    ParseContext pc;
                    pc.source = &ss;
                    if (!OR.parse(pc)) return fail("'yxxz' parse failed");
                    auto x = pc.stack.back<Stack>();
                    auto v = x.aggregateRange().get<std::string>();
                    if (v!="yxxz") return fail("'yxxz' stack invalid");
                }
                {
                    StringSource ss("zxxz");
                    ParseContext pc;
                    pc.source = &ss;
                    if (!OR.parse(pc)) return fail("'zxxz' parse failed");
                    auto x = pc.stack.back<Stack>();
                    auto v = x.aggregateRange().get<std::string>();
                    if (v!="zxxz") return fail("'zxxz' stack invalid");
                }
                {
                    StringSource ss("axx");
                    ParseContext pc;
                    pc.source = &ss;
                    if (OR.parse(pc)) return fail("'axx' parse succeeded");
                }
                return true;
            }

            bool succSimpleTest() {
                Rule OR = Lit("x") >> Lit("y");

                {
                    StringSource ss("xy");
                    ParseContext pc;
                    pc.source = &ss;
                    if (!OR.parse(pc)) return fail("'xy' parse failed");
                    auto r = pc.stack.aggregateRange().get<std::string>();
                    if (r!="xy") return fail("'xy' stack invalid");
                }
                {
                    StringSource ss(" x y");
                    ParseContext pc(&ss,Lit(" "));
                    if (!OR.parse(pc)) return fail("' x y' parse failed");
                    auto r = pc.stack.aggregateRange().get<std::string>();
                    if (r!="x y") return fail("' x y' stack invalid");
                }
                {
                    StringSource ss("zz");
                    ParseContext pc;
                    pc.source = &ss;
                    if (OR.parse(pc)) return fail("'zz' parse succeeded");
                }
                {
                    StringSource ss("xz");
                    ParseContext pc;
                    pc.source = &ss;
                    if (OR.parse(pc)) return fail("'xz' parse succeeded");
                }
                return true;
            }

            bool CharTest() {
                {
                    Rule rule = +Char("1234");
                    StringSource ss("1234");
                    ParseContext pc;
                    pc.source = &ss;
                    if (!rule.parse(pc)) return fail("'1234' parse failed");
                    auto x = pc.stack.back<Stack>();
                    auto r = x.aggregateRange().get<std::string>();
                    if (r!="1234") return fail("'1234' stack invalid");
                }
                {
                    Rule rule = +Char("1-4");
                    StringSource ss("1243");
                    ParseContext pc;
                    pc.source = &ss;
                    if (!rule.parse(pc)) return fail("'1243' parse failed");
                    auto x = pc.stack.back<Stack>();
                    auto r = x.aggregateRange().get<std::string>();
                    if (r!="1243") return fail("'1243' stack invalid");
                }
                {
                    Rule rule = +Char("0-9a-fA-F");
                    StringSource ss("12aB7fF");
                    ParseContext pc;
                    pc.source = &ss;
                    if (!rule.parse(pc)) return fail("'12aB7fF' parse failed");
                    auto x = pc.stack.back<Stack>();
                    auto r = x.aggregateRange().get<std::string>();
                    if (r!="12aB7fF") return fail("'12aB7fF' stack invalid");
                }
                {
                    Rule rule = Int();
                    StringSource ss("1234");
                    ParseContext pc;
                    pc.source = &ss;
                    if (!rule.parse(pc)) return fail("int '1234' parse failed");
                    auto r = pc.stack.back<unsigned long>();
                    if (r!=1234) return fail("int '1234' stack invalid");
                }
                {
                    Rule rule = NoCase(Hex());
                    StringSource ss("1fF4");
                    ParseContext pc;
                    pc.source = &ss;
                    if (!rule.parse(pc)) return fail("hex '1fF4' parse failed");
                    auto r = pc.stack.back<unsigned long>();
                    if (r!=8180) return fail("hex '1fF4' stack invalid");
                }
                return true;
            }
        };


        void run() {
            SimpleTest().run();
        }
    }
}
