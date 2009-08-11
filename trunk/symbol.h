#ifndef __HCC_SYMBOL_H
#define __HCC_SYMBOL_H

struct symbol 
{
	char *name;
	int scope;
	struct symbol* up;
};


struct symbol_table
{
	int level;
    struct symbol_table* previous;
    struct entry 
    {
		struct symbol sym;
		struct entry *link;
	} *buckets[256]; // [tag] - choose a better value for bucket size
	
    struct symbol* all_symbols;
};

struct symbol_table* make_symbol_table(int arena);
void enter_scope(void);
void exit_scope(void);
struct symbol* add_symbol(const char* name, struct symbol_table** table, int scope_level, int arena);
struct symbol* find_symbol(const char* name, struct symbol_table* table);

#endif