#include "../base.h"
#include "../ast.h"
#include "../parser.h"
#include "../eval.h"

namespace test {
    struct ParseInstructionTest {
        struct TestCase {
            std::string name;
            std::string input;
            bool mustParse;
        };

        bool run() {
            std::cout << "ParseInstructionTest\n";

            TestCase tests[] = {
                {
                    "solo label",
                    "label:",
                    true
                },
                {
                    "dot label",
                    ".label:",
                    true
                },
                {
                    "keyword label",
                    "nop:",
                    false
                },
                {
                    "solo nop",
                    "nop",
                    true
                },
                {
                    "solo imm",
                    "lda #$55",
                    true
                },
                {
                    "solo abs",
                    "ldx $5678",
                    true
                },
                {
                    "solo absx",
                    "lda 1234,x",
                    true
                },
                {
                    "solo absy",
                    "ldy %1001,y",
                    true
                },
                {
                    "solo indx",
                    "lda (34,x)",
                    true
                },
                {
                    "solo indy",
                    "lda (34),y",
                    true
                },
                {
                    "solo label",
                    "beq .label",
                    true
                },
                {
                    "solo keyword label",
                    "beq beq",
                    false
                },
                {
                    "multiliner",
                    "lda #0\nlab: ldx #0\nldy 12+34,x",
                    true
                },
                {
                    "some nops",
                    "nop\nnop\nnop",
                    true
                },
            };

            for (int i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
                // parse test case
                TestCase& test = tests[i];
                std::string input = std::string(test.input);
                ast::Program output;
                if (!parser::parse(input,output)) {
                    if (test.mustParse) return fail(test.name,"failed to parse");
                    continue;
                } else {
                    if (!test.mustParse) return fail(test.name,"parsed but shouldn't");
                }

                // set up evaluation context
                Evaluator E;
                //E.symtab["x"]=10000;
                //E.symtab["y"]=20000;

                // evaluate result(s)
                std::cout << test.name << " ---------------------\ngiven:\n" << test.input << "\nresult:\n";
                if (std::vector<ast::Statement>* s = boost::get<std::vector<ast::Statement>>(&output)) {
                    for (auto it=s->begin(); it!=s->end(); it++) {
                        if (ast::Ins* i = boost::get<ast::Ins>(&*it)) {
                            std::wstring line = i->toString();
                            std::wcout << line << "\n";

//                            if (ast::TestParseExpr* p = boost::get<ast::TestParseExpr>(t)) {
//                                for (int j = 0; j < p->list.size(); j++) {
//                                    auto ans = E.evalExpr(p->list.at(j));
//                                    //std::cout << ans << "?=" << test.expected[j] << "\n";
//                                    if (ans != test.expected[j]) return fail(test.name,"result mismatch");
//                                }
//                            } else return fail(test.name,"test result type mismatch");
                        } else return fail(test.name,"test type mismatch");
                    }
                } else return fail(test.name,"statement type mismatch");
            }
            return true;
        }

        bool fail(std::string name,std::string message) {
            std::cout << "FAILURE: test case "<< name << ": " << message << "\n";
            return false;
        }
    };
}
