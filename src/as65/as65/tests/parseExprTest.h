#include "../base.h"
#include "../ast.h"
#include "../parser.h"

namespace test {
    struct ParseExprTest {
        bool run() {
            std::cout << "ParseExprTest\n";

            const char* tests[] = {
                "x,y",
                "33,9,$10,%1001,-3",
                "2++2,2+-2,2--2",
                "--3,+$a,10-$A,(4)",
                "-(-4),3*2-5,3+2*-5,3*(-5+2),3+3-3-3,~$f,~(255^($f0|%1111))&12",
                "zorro-3+(quadratic)",
                NULL
            };

            for (const char **testcase=tests; *testcase; testcase++) {
                std::string input = std::string("$$test$$ expr ")+*testcase;
                ast::Program output;
                if (!parser::parse(input,output)) {
                    std::cout<<"test case: '"<<*testcase<<"'\n";
                    return false;
                }

                std::cout << "skipping ref until we have an evaluator\n";
                /*
                if (std::vector<ast::Statement>* s = boost::get<std::vector<ast::Statement>>(&output)) {
                    if (s->size() != 1) return false;
                    if (ast::Test* t = boost::get<ast::Test>(&s->at(0))) {
                        if (ast::TestParseExpr* p = boost::get<ast::TestParseExpr>(t)) {
                            //for (auto it = p->list.begin(); it != p->list.end(); it++) std::cout << *it << "\n";
                            //if (p->list.at(0)!=3) return false;
                            //if (p->list.at(1)!=9) return false;
                            //if (p->list.at(2)!=16) return false;
                            //if (p->list.at(3)!=9) return false;
                            std::cout << "ndy\n"; return false;
                        }
                    }
                }
                */
            }
            return true;
        }
    };
}
