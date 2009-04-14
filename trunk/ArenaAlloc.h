#ifndef __HCC_ALLOC_H
#define __HCC_ALLOC_H

void* hcc_alloc(unsigned long n, unsigned a);
void hcc_free(unsigned a);

#define ALLOC(p,a) ((p) = hcc_alloc(sizeof *(p), (a)))
#define CALLOC(p,a) memset(ALLOC((p),(a)), 0, sizeof *(p))

#endif