#include "../base.h"
#include "../ast.h"
#include "../parser.h"
#include "../eval.h"

#include <boost/variant.hpp>

namespace test {
    namespace ph = boost::phoenix;

    typedef Evaluator::Result Result;

    struct ParseTermTest {
        struct TestCase {
            std::string input;
            Result expected[8];
        };

        bool run() {
            std::cout << "ParseTermTest\n";

            TestCase tests[] = {
                { "33", {33} },
                { "+33", {33} },
                { "-33,--33,+-33,---33,+-33", {-33,33,-33,-33,-33} },
                { "x,~x,-~+~-x", {10000,~Result(10000),-~+~-Result(10000)} },
                //{ "(expr)", {16} }, -- skip this here since it implicitly tests higher level constructs
            };

            for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
                // parse test case
                TestCase& test = tests[i];
                std::string input = std::string("$$test$$ term ")+test.input;
                ast::Program output;
                if (!parser::parse(input,output)) {
                    return fail(test.input,"failed to parse");
                }

                // set up evaluation context
                Evaluator E;
                E.symtab["x"]=10000;

                // evaluate result(s)
                if (std::vector<ast::Statement>* s = boost::get<std::vector<ast::Statement>>(&output)) {
                    if (s->size() != 1) return fail(test.input,"unexpected number of statements");
                    if (ast::Test* t = boost::get<ast::Test>(&s->at(0))) {
                        if (ast::TestParseTerm* p = boost::get<ast::TestParseTerm>(t)) {
                            for (int j = 0; j < p->list.size(); j++) {
                                auto ans = E.evalTerm(p->list.at(j));
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
