#include "../base.h"
#include "../ast.h"
#include "../parser.h"

namespace test {
    struct ParseNumberTest {
        struct TestCase {
            std::string input;
            unsigned expected[8];
        };

        bool run() {
            std::cout << "ParseNumberTest\n";

            TestCase tests[] = {
                { "33", {33} },
                { "$10", {16} },
                { "%1001", {9} },
                { "3,9,$55,%10000001", {3,9,0x55,0x81} }
            };

            for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
                TestCase& test = tests[i];
                std::string input = std::string("$$test$$ number ")+test.input;
                ast::Program output;
                if (!parser::parse(input,output)) {
                    std::cout << "test case: '" << test.input << "'\n";
                    return false;
                }

                if (std::vector<ast::Statement>* s = boost::get<std::vector<ast::Statement>>(&output)) {
                    if (s->size() != 1) return false;
                    if (ast::Test* t = boost::get<ast::Test>(&s->at(0))) {
                        if (ast::TestParseNumber* p = boost::get<ast::TestParseNumber>(t)) {
                            for (int j = 0; j < p->list.size(); j++) {
                                if (p->list.at(j) != test.expected[j]) return false;
                            }
                        } else return false;
                    } else return false;
                } else return false;
            }

            return true;
        }
    };
}
