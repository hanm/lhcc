#if 0
typedef int (__cdecl * _onexit_t)(void);

typedef unsigned long michael;
typedef michael kiki;
typedef kiki kosmos;

typedef wchar_t widechar;
typedef widechar WC;

int    __cdecl atexit(void (__cdecl *)(void));

typedef struct
{
    int a;
} my_struct, *ptr_struct;

typedef my_struct s1;

//int (__cdecl * _PtFuncCompare)(void *, const void *, const void *);
int foo(const void*, int (__cdecl * _PtFuncCompare)(void *, const void *, const void *), const void*);

//size_t  __cdecl strnlen(const char * _Str, size_t _MaxCount);

static size_t  strnlen_s(const char * _Str, size_t _MaxCount)
{
    return (_Str==0) ? 0 : strnlen(_Str, _MaxCount);
} // Added on 12/6. It can't parse function definition in header files yet.

// from stdlib?
#define NULL ((void *)0)
static size_t wcsnlen_s(const wchar_t * _Src, size_t _MaxCount)
{
    return (_Src == NULL) ? 0 : wcsnlen(_Src, _MaxCount);
}

// from trie.c
typedef struct _Trie {
   int foo;
} Trie;

Trie *trie_new(void)
{
	Trie *new_trie;

	new_trie = (Trie *) malloc(sizeof(Trie));

	if (new_trie == NULL) {
		return NULL;
	}

	return new_trie;
}

// from set.c
static const unsigned int set_primes[] = {
	193, 389, 769, 1543, 3079, 6151, 12289, 24593, 49157, 98317,
	196613, 393241, 786433, 1572869, 3145739, 6291469,
	12582917, 25165843, 50331653, 100663319, 201326611,
	402653189, 805306457, 1610612741,
};

static const int set_num_primes = sizeof(set_primes) / sizeof(int);

// from sqlite
int sqlite3_busy_handler(void*, int(*)(void*,int), void*);
#else

typedef struct theList tlist;

int sqlite3VdbeAddOpList(void*, int nOp, tlist const *aOp);
typedef  struct _MIDL_STUB_MESSAGE MIDL_STUB_MESSAGE,   *PMIDL_STUB_MESSAGE;
 static char zChars[] = "abcdefghijklmnopqrstuvwxyz""ABCDEFGHIJKLMNOPQRSTUVWXYZ""0123456789";
#define __$adt_remove_prop(adt,prop)
#endif

struct a;
struct a;
enum a; // report error

enum b;
enum b;
enum b; // ok, ok
struct b; // report error
int b; // ..... should also report error but..

struct a;
enum a {ENUM};

// FIXME - this actually failed the semantic checker!
// int main(){};
