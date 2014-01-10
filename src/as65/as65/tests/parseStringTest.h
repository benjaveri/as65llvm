#include "../base.h"
#include "../ast.h"
#include "../parser.h"

namespace test {
    struct ParseStringTest {
        bool run() {
            std::cout << "ParseStringTest\n";

            std::string input = "$$test$$ qstring \"x\", \"hello there!\", \"\\\"quoted\\\"\", \"\\t\\xff34\"";
            ast::Program output;
            if (!parser::parse(input,output)) return false;

            /*
            if (std::vector<ast::Statement>* s = boost::get<std::vector<ast::Statement>>(&output)) {
                if (s->size() != 1) return false;
                if (ast::Test* t = boost::get<ast::Test>(&s->at(0))) {
                    if (ast::TestParseString* p = boost::get<ast::TestParseString>(t)) {
                        //for (auto it = p->list.begin(); it != p->list.end(); it++) std::cout << *it << "\n";
                        //if (p->list.at(0)!="x") return false;
                        //if (p->list.at(1)!="hello there!") return false;
                        //if (p->list.at(2)!="\\\"quoted\\\"") return false;
                        std::cout << "ndy\n"; return false;
                    }
                }
            }
            */

            return true;
        }
    };
}
