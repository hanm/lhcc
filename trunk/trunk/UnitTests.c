#include "CuTest.h"
#include <stdio.h>
#include "hcc.h"

struct block {
	struct block *next;
	char *limit;
	char *avail;
};

union align {
	long l;
	char *p;
	double d;
	int (*f)(void);
};

union header {
	struct block b;
	union align a;
};

CuSuite* atomstringtestgetsuite();
CuSuite* macrotestgetsuite();

void run(void) 
{
	CuString *output = CuStringNew();
	CuSuite* suite = CuSuiteNew();

	CuSuiteAddSuite(suite, atomstringtestgetsuite());
    CuSuiteAddSuite(suite, macrotestgetsuite());

	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	printf("%s\n", output->buffer);
}

#if defined(_HCC_UT)
//
// entry point
//
int main()
{
    printf("%d \n", sizeof(union header));
	run();
	return 0;
}
#endif