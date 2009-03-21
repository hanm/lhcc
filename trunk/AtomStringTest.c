#include "CuTest.h"
#include "Hcc.h"
#include <string.h>

#ifndef NULL 
#define NULL (void*)0
#endif

void testatomstring(CuTest *tc) 
{
	char* input = "hello";
	const char* output1 = atom_string(input);
	const char* output2 = atom_string(input);

	CuAssertStrEquals(tc, input, output1);
	CuAssertStrEquals(tc, output1, output2);
	CuAssertPtrEquals(tc, output1, output2);
}

CuSuite* atomstringtestgetsuite() 
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, testatomstring);
	return suite;
}