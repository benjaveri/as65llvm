#include "base.h"
#include "parser.h"
#ifdef TEST
#include "tests/allTests.h"
#endif

int main(int argc, const char * argv[])
{
#ifdef TEST
    test::AllTests all;
    if (!all.run()) return -1;
#endif

    //parser::parse();
    return 0;
}

