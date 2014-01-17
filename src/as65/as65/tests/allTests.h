#ifndef _all_h
#define _all_h

#include "../base.h"
#ifdef TEST
#include "parseStringTest.h"
#include "parseNumberTest.h"
#include "parseValueTest.h"
#include "parseTermTest.h"
#include "parseExprTest.h"
#include "parseInstructionTest.h"
#include "codeTest.h"

namespace test {
    struct AllTests {
        bool run() {
            std::cout << "=========================================\n";
            std::cout << "\tRunning All Tests\n";
            std::cout << "=========================================\n";

            {
                ParseNumberTest t;
                if (!t.run()) return fail("ParseNumberTest");
            }
            {
                ParseValueTest t;
                if (!t.run()) return fail("ParseValueTest");
            }
            {
                ParseTermTest t;
                if (!t.run()) return fail("ParseTermTest");
            }
            {
                ParseExprTest t;
                if (!t.run()) return fail("ParseExprTest");
            }
            {
                ParseStringTest t;
                if (!t.run()) return fail("ParseStringTest");
            }
            {
                ParseInstructionTest t;
                if (!t.run()) return fail("ParseInstructionTest");
            }
            {
                CodeTest t;
                if (!t.run()) return fail("CodeTest");
            }

            std::cout << "All tests passed\n";
            return true;
        }

        bool fail(const char* message) {
            std::cout << "Test failed: " << message << "\n";
            return false;
        }
    };
}

#endif
#endif
