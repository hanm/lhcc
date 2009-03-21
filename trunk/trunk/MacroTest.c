#include "CuTest.h"
#include "Hcc.h"

void testmacroroundup(CuTest *tc) 
{
	int x = 8, y = 9, z = 17;
    int n = 8;

    CuAssertIntEquals(tc, ROUNDUP(x,n), ROUNDUP_(x,n));
    CuAssertIntEquals(tc, ROUNDUP(y,n), ROUNDUP_(y,n));
    CuAssertIntEquals(tc, ROUNDUP(z,n), ROUNDUP_(z,n));

    CuAssertIntEquals(tc, ROUNDUP(x,n), 8);
    CuAssertIntEquals(tc, ROUNDUP(y,n), 16);
    CuAssertIntEquals(tc, ROUNDUP(z,n), 24);

}

CuSuite* macrotestgetsuite() 
{
	CuSuite* suite = CuSuiteNew();
    SUITE_ADD_TEST(suite, testmacroroundup);
	return suite;
}