#ifndef __HCC_TRACE
#define __HCC_TRACE

#ifdef HCC_TRACE_ENABLE
#define HCC_TRACE(msg, variable) printf(msg,variable)
#else
#define HCC_TRACE(msg, variable)
#endif

#endif