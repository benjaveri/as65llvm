#include "../base.h"
#include "../ast.h"
#include "../parser.h"

#include <boost/variant.hpp>
#include <boost/spirit/include/phoenix_object.hpp>

namespace test {
    namespace ph = boost::phoenix;

    struct ParseValueTest {
        struct TestCase {
            std::string input;
            boost::variant<
                unsigned,
                std::string
            > expected[8];
        };

        bool run() {
            std::cout << "ParseValueTest\n";

            TestCase tests[] = {
                { "33", {33} },
                { "x", {"x"} },
                //{ "(expr)", {16} }, -- skip this here since it implicitly tests higher level constructs
            };

            for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
                TestCase& test = tests[i];
                std::string input = std::string("$$test$$ value ")+test.input;
                ast::Program output;
                if (!parser::parse(input,output)) {
                    return fail(test.input,"failed to parse");
                }

                if (std::vector<ast::Statement>* s = boost::get<std::vector<ast::Statement>>(&output)) {
                    if (s->size() != 1) return fail(test.input,"unexpected number of statements");
                    if (ast::Test* t = boost::get<ast::Test>(&s->at(0))) {
                        if (ast::TestParseValue* p = boost::get<ast::TestParseValue>(t)) {
                            for (int j = 0; j < p->list.size(); j++) {
                                if (unsigned *ans = boost::get<unsigned>(&p->list.at(j))) {
                                    if (unsigned *ref = boost::get<unsigned>(&test.expected[j])) {
                                        if (*ans != *ref) return fail(test.input,"number result mismatch");
                                    } else return fail(test.input,"type mismatch (1)");
                                } else if (ast::Identifier *ans = boost::get<ast::Identifier>(&p->list.at(j))) {
                                    if (std::string *ref = boost::get<std::string>(&test.expected[j])) {
                                        if (ans->name != *ref) return fail(test.input,"identifer name mismatch");
                                    } else return fail(test.input,"type mismatch (2)");
                                } else return fail(test.input,"type mismatch (3)");
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
