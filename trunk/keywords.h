/* Define NULL pointer value */
#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

typedef struct keyword
{
	char *name;
	int len;
	int token;
} tKW;

static tKW kwA[] =
{
	{"auto", 4, TK_AUTO},
    {NULL,       0, TK_ID}
};

static tKW kwB[] = 
{
	{"break", 5, TK_BREAK},
    {NULL,       0, TK_ID}
};

static tKW kwC[] = 
{
	{"case",     4, TK_CASE},
	{"char",     4, TK_CHAR},
	{"const",    5, TK_CONST},
	{"continue", 8, TK_CONTINUE},
    {NULL,       0, TK_ID}
};

static tKW kwD[] =
{
	{"default", 7, TK_DEFAULT},
	{"do",      2, TK_DO},
	{"double",  6, TK_DOUBLE},
    {NULL,       0, TK_ID}
};

static tKW kwE[] =
{
	{"else",   4, TK_ELSE},
	{"enum",   4, TK_ENUM},
	{"extern", 6, TK_EXTERN},
    {NULL,       0, TK_ID}
};

static tKW kwF[] =
{
	{"float", 5, TK_FLOAT},
	{"for",   3, TK_FOR},
    {NULL,       0, TK_ID}
};

static tKW kwG[] = 
{
	{"goto", 4, TK_GOTO},
    {NULL,       0, TK_ID}
};

static tKW kwH[] = 
{
    {NULL,       0, TK_ID}
};

static tKW kwI[] = 
{
	{"if",  2, TK_IF},
	{"int", 3, TK_INT},
    {NULL,       0, TK_ID}
};

static tKW kwJ[] = 
{
    {NULL,       0, TK_ID}
};

static tKW kwK[] = 
{
    {NULL,       0, TK_ID}
};

static tKW kwL[] = 
{
	{"long", 4,	TK_LONG},
    {NULL,       0, TK_ID}
};

static tKW kwM[] = 
{
    {NULL,       0, TK_ID}
};

static tKW kwN[] = 
{
    {NULL,       0, TK_ID}
};

static tKW kwO[] = 
{
    {NULL,       0, TK_ID}
};

static tKW kwP[] = 
{
    {NULL,       0, TK_ID}
};

static tKW kwQ[] = 
{
    {NULL,       0, TK_ID}
};

static tKW kwR[] = 
{
	{"register", 8, TK_REGISTER},
	{"return",   6, TK_RETURN},
    {NULL,       0, TK_ID}
};

static tKW kwS[] = 
{
	{"short",  5, TK_SHORT},
	{"signed", 6, TK_SIGNED},
	{"sizeof", 6, TK_SIZEOF},
	{"static", 6, TK_STATIC},
	{"struct", 6, TK_STRUCT},
	{"switch", 6, TK_SWITCH},
    {NULL,       0, TK_ID}
};

static tKW kwT[] = 
{
	{"typedef", 7, TK_TYPEDEF},
    {NULL,       0, TK_ID}
};

static tKW kwU[] = 
{
	{"union",    5, TK_UNION},
	{"unsigned", 8, TK_UNSIGNED},
    {NULL,       0, TK_ID}
};

static tKW kwV[] = 
{
	{"void",     4, TK_VOID},
	{"volatile", 8, TK_VOLATILE},
    {NULL,       0, TK_ID}
};

static tKW kwW[] = 
{
	{"while", 5, TK_WHILE },
    {NULL,       0, TK_ID}
};

static tKW kwX[] = 
{
    {NULL,       0, TK_ID}
};

static tKW kwY[] = 
{
    {NULL,       0, TK_ID}
};

static tKW kwZ[] = 
{
    {NULL,       0, TK_ID}
};

static tKW* kw_table[] =
{
    kwA, kwB, kwC, kwD, kwE, kwF, kwG,
    kwH, kwI, kwJ, kwK, kwL, kwM, kwN,
    kwO, kwP, kwQ, kwR, kwS, kwT,
    kwU, kwV, kwW, kwX, kwY, kwZ
};