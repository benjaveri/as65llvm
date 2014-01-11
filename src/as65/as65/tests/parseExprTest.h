#include "../base.h"
#include "../ast.h"
#include "../parser.h"

namespace test {
    struct ParseExprTest {
        struct TestCase {
            std::string input;
            Result expected[8];
        };

        bool run() {
            std::cout << "ParseExprTest\n";

            TestCase tests[] = {
                { "x,y", {10000,20000} },
                { "33,9,$10,%1001,-3", {33,9,16,9,-3} },
                { "2++2,2+-2,2--2", {4,0,4} },
                { "--3,+$a,10-$A,(4)", {3,10,0,4} },
                { "-(-4),3*2-5,3+2*-5,3*(-5+2),3+3-3-3,~$f,~(255^($f0|%1111))&12", {4,1,-7,-9,0,~15,12} },
                { "x-3+(((y)))", {29997} },
                { "2<<2,8>>2,1<<60>>60", {8,2,1} },
            };

            for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
                // parse test case
                TestCase& test = tests[i];
                std::string input = std::string("$$test$$ expr ")+test.input;
                ast::Program output;
                if (!parser::parse(input,output)) {
                    return fail(test.input,"failed to parse");
                }

                // set up evaluation context
                Evaluator E;
                E.symtab["x"]=10000;
                E.symtab["y"]=20000;

                // evaluate result(s)
                if (std::vector<ast::Statement>* s = boost::get<std::vector<ast::Statement>>(&output)) {
                    if (s->size() != 1) return fail(test.input,"unexpected number of statements");
                    if (ast::Test* t = boost::get<ast::Test>(&s->at(0))) {
                        if (ast::TestParseExpr* p = boost::get<ast::TestParseExpr>(t)) {
                            for (int j = 0; j < p->list.size(); j++) {
                                auto ans = E.evalExpr(p->list.at(j));
                                //std::cout << ans << "?=" << test.expected[j] << "\n";
                                if (ans != test.expected[j]) return fail(test.input,"result mismatch");
                            }
                        } else return fail(test.input,"test result type mismatch");
                    } else return fail(test.input,"test type mismatch");
                } else return fail(test.input,"statement type mismatch");
            }
            return true;
        }

        bool fail(std::string testcase,std::string message) {
            std::cout << "FAILURE: test case "<< testcase << ": " << message << "\n";
            return false;
        }
    };
}
