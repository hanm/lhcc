/***************************************************************

Copyright (c) 2008-2010 Michael Liang Han

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

****************************************************************/

#include "ast.h"
#include "error.h"

/* Semantic check for declarations */

static void sc_declaration_specifiers(t_ast_declaration_specifier* spec)
{   
    t_ast_list* qualifiers;
    t_ast_type_qualifier* q;
    int flag = 0;

    assert(spec);   

    qualifiers = spec->type_qualifier_list;
    while(!HCC_AST_LIST_IS_END(qualifiers))
    {
        /* TODO - check duplicates need a smart way*/
        q = (t_ast_type_qualifier*)qualifiers->item;
        if (q->kind = AST_TYPE_CONST)
        {
            flag |= 0x01;
        }
        else if (q->kind == AST_TYPE_VOLATILE)
        {
            flag |= 0x10;
        }
    }
}

/* http://www.mers.byu.edu/docs/standardC/declare.html */

/* specification of declaration checking */
/*
Outer Declaration

You write an outer declaration as one of the declarations that make up a translation unit. 
An outer declaration is one that is not contained within another declaration or function definition:
You can omit the declarator only for a structure, union, or enumeration declaration that declares a tag. 
You must write a name within the declarator of any other outer declarator.
Variable array can't be declared externally (at file scope)

Storage specifiers : extern, static, typdef (auto, and register is forbidden)
Storage specifiers are omittable (implicit external declaration assumed extern)

Type specifiers are omittable (implicit assumed as int)
*/

/*
 semantic check for global declarations
 */
static void sc_outer_declaration(t_ast_declaration* declr)
{
    t_ast_declaration_specifier* specifiers;
    t_ast_storage_specifier* storage_specifier;
    
    assert(declr);

    specifiers = declr->declr_specifiers;
    storage_specifier = specifiers->storage_specifier;

    if (storage_specifier && 
        (storage_specifier->kind == AST_STORAGE_AUTO ||
            specifiers->storage_specifier->kind == AST_STORAGE_REGISTER))
    {
        semantic_error("global declaration can't have register or auto storage specifier!", &specifiers->coord);
    }    
}

void semantic_check(t_ast_translation_unit* translation_unit)
{
    t_ast_list *ext_declr_list;
	t_ast_external_declaration* ext_declr;

    assert(translation_unit);

    ext_declr_list = translation_unit->ext_declaration_list;

    while(!HCC_AST_LIST_IS_END(ext_declr_list))
    {
		ext_declr = ext_declr_list->item;
        ext_declr_list = ext_declr_list->next;

		if (ext_declr->fun_def)
		{
			/* check function */
		}
		else
		{
			sc_outer_declaration(ext_declr->u.declr);
		}
    }
}

