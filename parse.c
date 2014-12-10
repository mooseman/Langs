

/*  parse.c  */ 

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "lice.h"
#include "lexer.h"
#include "conv.h"
#include "init.h"
#include "parse.h"
#include "opt.h"


static ast_t       *parse_expression(void);

static ast_t	   *parse_select_stmt(void); 
static ast_t 	   *parse_from_stmt(void);   
static ast_t	   *parse_where_stmt(void); 

static ast_t       *parse_expression_conditional(void);

static ast_t       *parse_expression_comma(void);
static ast_t       *parse_statement_compound(void);
static ast_t       *parse_statement(void);


table_t *parse_typedefs = &SENTINEL_TABLE;

static bool parse_type_check(lexer_token_t *token);


static void parse_semantic_lvalue(ast_t *ast) {
    switch (ast->type) {
        case AST_TYPE_VAR_LOCAL:
        case AST_TYPE_VAR_GLOBAL:
        case AST_TYPE_DEREFERENCE:
        case AST_TYPE_STRUCT:
            return;
    }
    compile_error("expected lvalue, `%s' isn't a valid lvalue", ast_string(ast));
}


/*  Expecting a punctuation char.  */ 
void parse_expect(char punct) {
    lexer_token_t *token = lexer_next();
    if (!lexer_ispunct(token, punct))
        compile_error("expected `%c`, got %s instead", punct, lexer_token_string(token));
}


/*  Identifier check  */ 
static bool parse_identifer_check(lexer_token_t *token, const char *identifier) {
    return token->type == LEXER_TOKEN_IDENTIFIER && !strcmp(token->string, identifier);
}


/*  TO DO - get rid of "offset" and simplify here.  */ 
long parse_evaluate(ast_t *ast);
int parse_evaluate_offsetof(ast_t *ast, int offset) {
    if (ast->type == AST_TYPE_STRUCT)
        return parse_evaluate_offsetof(ast->structure, ast->ctype->offset + offset);
    return parse_evaluate(ast) + offset;
}


long parse_evaluate(ast_t *ast) {
    long cond;

    switch (ast->type) {
        case AST_TYPE_LITERAL:
            if (ast_type_isinteger(ast->ctype))
                return ast->integer;
            compile_error("not a valid integer constant expression `%s'", ast_string(ast));
            break;

        case '+':             return parse_evaluate(ast->left) +  parse_evaluate(ast->right);
        case '-':             return parse_evaluate(ast->left) -  parse_evaluate(ast->right);
        case '*':             return parse_evaluate(ast->left) *  parse_evaluate(ast->right);
        case '/':             return parse_evaluate(ast->left) /  parse_evaluate(ast->right);
        case '<':             return parse_evaluate(ast->left) <  parse_evaluate(ast->right);
        case '>':             return parse_evaluate(ast->left) >  parse_evaluate(ast->right);

        case AST_TYPE_AND:    return parse_evaluate(ast->left) && parse_evaluate(ast->right);
        case AST_TYPE_OR:     return parse_evaluate(ast->left) || parse_evaluate(ast->right);
        case AST_TYPE_EQUAL:  return parse_evaluate(ast->left) == parse_evaluate(ast->right);
        case AST_TYPE_LEQUAL: return parse_evaluate(ast->left) <= parse_evaluate(ast->right);
        case AST_TYPE_GEQUAL: return parse_evaluate(ast->left) >= parse_evaluate(ast->right);
        case AST_TYPE_NEQUAL: return parse_evaluate(ast->left) != parse_evaluate(ast->right);
                
        default:
        error:
            compile_error("not a valid integer constant expression `%s'", ast_string(ast));
    }
    return -1;
}


bool parse_next(int ch) {
    lexer_token_t *token = lexer_next();
    if (lexer_ispunct(token, ch))
        return true;
    lexer_unget(token);
    return false;
}


static ast_t *parse_generic(char *name) {
    ast_t *ast = table_find(ast_localenv ? ast_localenv : ast_globalenv, name);

    if (!ast || ast->ctype->type == TYPE_FUNCTION)
        return ast_designator(name, ast);

    return ast;
}


static ast_t *parse_number_integer(char *string) {
    char *p    = string;
    int   base = 10;

    if (!strncasecmp(string, "0x", 2)) {
        base = 16;
        p++;
        p++;
    } else if (!strncasecmp(string, "0b", 2)){
        if (!opt_extension_test(EXTENSION_BINARYCONSTANTS))
            compile_error("binary constants only supported in -std=lice or -std=gnuc");
        base = 2;
        p++;
        p++;
    } else if (string[0] == '0' && string[1] != '\0') {
        base = 8;
        p++;
    }

    char *digits = p;
    while (isxdigit(*p)) {
        if (base == 10 && isalpha(*p))
            compile_error("invalid character in decimal literal");
        if (base == 8 && !('0' <= *p && *p <= '7'))
            compile_error("invalid character in octal literal");
        if (base == 2 && (*p != '0' && *p != '1'))
            compile_error("invalid character in binary literal");
        p++;
    }

    if (!strcasecmp(p, "l"))
        return ast_new_integer(ast_data_table[AST_DATA_LONG], strtol(string, NULL, base));
    if (!strcasecmp(p, "ul") || !strcasecmp(p, "lu") || !strcasecmp(p, "u"))
        return ast_new_integer(ast_data_table[AST_DATA_ULONG], strtoul(string, NULL, base));
    if (!strcasecmp(p, "ll"))
        return ast_new_integer(ast_data_table[AST_DATA_LLONG], strtoll(string, NULL, base));
    if (!strcasecmp(p, "ull") || !strcasecmp(p, "llu"))
        return ast_new_integer(ast_data_table[AST_DATA_ULLONG], strtoull(string, NULL, base));
    if (*p != '\0')
        compile_error("invalid suffix for literal");

    long value = strtol(digits, NULL, base);
    return (value & ~(long)UINT_MAX)
                ? ast_new_integer(ast_data_table[AST_DATA_LONG], value)
                : ast_new_integer(ast_data_table[AST_DATA_INT], value);
}



static ast_t *parse_number(char *string) {
    return strpbrk(string, ".pP") ||
                /*
                 * 0xe.. is integer type, sadly it's hard to check for that
                 * without additonal logic.
                 */
                (strncasecmp(string, "0x", 2) && strpbrk(string, "eE"))
                    ? parse_number_floating(string)
                    : parse_number_integer(string);
}


static int parse_operation_reclassify(int punct) {
    switch (punct) {
        case LEXER_TOKEN_EQUAL:  return AST_TYPE_EQUAL;
        case LEXER_TOKEN_GEQUAL: return AST_TYPE_GEQUAL;
        case LEXER_TOKEN_LEQUAL: return AST_TYPE_LEQUAL;
        case LEXER_TOKEN_NEQUAL: return AST_TYPE_NEQUAL;
        case LEXER_TOKEN_AND:    return AST_TYPE_AND;
        case LEXER_TOKEN_OR:     return AST_TYPE_OR;
        default:
            break;
    }
    return punct;
}


static int parse_operation_compound_operator(lexer_token_t *token) {
    if (token->type != LEXER_TOKEN_PUNCT)
        return 0;

    switch (token->punct) {
        case LEXER_TOKEN_COMPOUND_ADD:    return '+';
        case LEXER_TOKEN_COMPOUND_AND:    return '&';
        case LEXER_TOKEN_COMPOUND_DIV:    return '/';
        case LEXER_TOKEN_COMPOUND_MUL:    return '*';
        case LEXER_TOKEN_COMPOUND_OR:     return '|';
        default:
            return 0;
    }
    return -1;
}


static ast_t *parse_expression_statement(void) {
    ast_t *ast = parse_statement_compound();
    parse_expect(')');
    data_type_t *type = ast_data_table[AST_DATA_VOID];
    if (list_length(ast->compound) > 0) {
        ast_t *last = list_tail(ast->compound);
        if (last)
            type = last->ctype;
    }
    ast->ctype = type;
    return ast;
}


static ast_t *parse_expression_primary(void) {
    lexer_token_t *token;

    if (!(token = lexer_next()))
        return NULL;

    if (lexer_ispunct(token, '(')) {        
        ast_t *ast = parse_expression();
        parse_expect(')');
        return ast;
    }

    switch (token->type) {
        case LEXER_TOKEN_IDENTIFIER:
            return parse_generic(token->string);
        case LEXER_TOKEN_NUMBER:
            return parse_number(token->string);
        case LEXER_TOKEN_CHAR:
            return ast_new_integer(ast_data_table[AST_DATA_INT], token->character);
        case LEXER_TOKEN_STRING:
            return ast_new_string(token->string);
        case LEXER_TOKEN_PUNCT:
            lexer_unget(token);
            return NULL;
        default:
            break;
    }

    compile_ice("parse_expression_primary");
    return NULL;
}


static ast_t *parse_expression_multiplicative(void) {
    ast_t *ast = ast_designator_convert(parse_expression_cast());
    for (;;) {
             if (parse_next('*')) ast = conv_usual('*', ast, ast_designator_convert(parse_expression_cast()));
        else if (parse_next('/')) ast = conv_usual('/', ast, ast_designator_convert(parse_expression_cast()));
        else if (parse_next('%')) ast = conv_usual('%', ast, ast_designator_convert(parse_expression_cast()));
        else break;
    }
    return ast;
}


static ast_t *parse_expression_additive(void) {
    ast_t *ast = parse_expression_multiplicative();
    for (;;) {
             if (parse_next('+')) ast = conv_usual('+', ast, parse_expression_multiplicative());
        else if (parse_next('-')) ast = conv_usual('-', ast, parse_expression_multiplicative());
        else break;
    }
    return ast;
}


/*  Parse a relational expression.  */ 
/*  This can be used in the SQL parser */  
static ast_t *parse_expression_relational(void) {
    ast_t *ast = parse_expression_shift();
    for (;;) {
             if (parse_next('<'))                ast = conv_usual('<',             ast, parse_expression_shift());
        else if (parse_next('>'))                ast = conv_usual('>',             ast, parse_expression_shift());
        else if (parse_next(LEXER_TOKEN_LEQUAL)) ast = conv_usual(AST_TYPE_LEQUAL, ast, parse_expression_shift());
        else if (parse_next(LEXER_TOKEN_GEQUAL)) ast = conv_usual(AST_TYPE_GEQUAL, ast, parse_expression_shift());
        else break;
    }
    return ast;
}


/*  Parse an equality expression.  */ 
/*  This can be used in the SQL parser.  */ 
static ast_t *parse_expression_equality(void) {
    ast_t *ast = parse_expression_relational();
    if (parse_next(LEXER_TOKEN_EQUAL))
        return conv_usual(AST_TYPE_EQUAL, ast, parse_expression_equality());
    if (parse_next(LEXER_TOKEN_NEQUAL))
        return conv_usual(AST_TYPE_NEQUAL, ast, parse_expression_equality());
    return ast;
}


/*  Logical AND  */ 
static ast_t *parse_expression_logand(void) {
    ast_t *ast = parse_expression_bitor();
    while (parse_next(LEXER_TOKEN_AND))
        ast = ast_new_binary(ast_data_table[AST_DATA_INT], AST_TYPE_AND, ast, parse_expression_bitor());
    return ast;
}


/*  Logical OR  */ 
static ast_t *parse_expression_logor(void) {
    ast_t *ast = parse_expression_logand();
    while (parse_next(LEXER_TOKEN_OR))
        ast = ast_new_binary(ast_data_table[AST_DATA_INT], AST_TYPE_OR, ast, parse_expression_logand());
    return ast;
}


/*  Assignment  */ 
ast_t *parse_expression_assignment(void) {
    ast_t         *ast   = parse_expression_logor();
    lexer_token_t *token = lexer_next();

    if (!token)
        return ast;

    if (lexer_ispunct(token, '?')) {
        ast_t *then = parse_expression_comma();
        parse_expect(':');
        ast_t *last = parse_expression_conditional();
        ast_t *operand = (opt_extension_test(EXTENSION_OMITOPCOND)) ? last : then;
        return ast_ternary(operand->ctype, ast, then, last);
    }

    int compound   = parse_operation_compound_operator(token);
    int reclassify = parse_operation_reclassify(compound);
    if (lexer_ispunct(token, '=') || compound) {
        ast_t *value = parse_expression_assignment();
        if (lexer_ispunct(token, '=') || compound)
            parse_semantic_lvalue(ast);

        ast_t *right = compound ? conv_usual(reclassify, ast, value) : value;
        if (conv_capable(right->ctype) && ast->ctype->type != right->ctype->type)
            right = ast_type_convert(ast->ctype, right);
        return ast_new_binary(ast->ctype, '=', ast, right);
    }

    lexer_unget(token);
    return ast;
}


/*  Comma  */ 
static ast_t *parse_expression_comma(void) {
    ast_t *ast = parse_expression_assignment();
    while (parse_next(',')) {
        ast_t *expression = parse_expression_assignment();
        ast = ast_new_binary(expression->ctype, ',', ast, expression);
    }
    return ast;
}


/*  Parse an expression  */  
static ast_t *parse_expression(void) {
    ast_t *ast = parse_expression_comma();
    if (!ast)
        compile_error("expression expected");
    return ast;
}


/*  Parse an optional expression. */  
static ast_t *parse_expression_optional(void) {
    return parse_expression_comma();
}


/*  Parse a condition.  */ 
static ast_t *parse_expression_condition(void) {
    ast_t *cond = parse_expression();
    if (ast_type_isfloating(cond->ctype))
        return ast_type_convert(ast_data_table[TYPE_BOOL], cond);
    return cond;
}



static ast_t *parse_expression_conditional(void) {
    ast_t *ast = parse_expression_logor();
    if (!parse_next('?'))
        return ast;
    ast_t *then = parse_expression_comma();
    parse_expect(':');
    ast_t *last = parse_expression_conditional();
    return ast_ternary(last->ctype, ast, then, last);
}


/*  Evaluate an expression.  */  
int parse_expression_evaluate(void) {
    return parse_evaluate(parse_expression_conditional());
}


/*  Type check.  */  
static bool parse_type_check(lexer_token_t *token) {
    if (token->type != LEXER_TOKEN_IDENTIFIER)
        return false;

    static const char *keywords[] = {
        "select",   "from",   "where",   "and",   "or",   "is",
        "in",   "not",  "null" 
    };

    for (size_t i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++)
        if (!strcmp(keywords[i], token->string))
            return true;

    if (table_find(parse_typedefs, token->string))
        return true;

    return false;
}


static ast_t *parse_statement_declaration_semicolon(void) {
    lexer_token_t *token = lexer_next();
    if (lexer_ispunct(token, ';'))
        return NULL;
    lexer_unget(token);
    list_t *list = list_create();
    parse_statement_declaration(list);
    return list_shift(list);
}


/*  Parse the SELECT clause                        */ 
/*  Possible args for select:  *,  single column,  */  
/*  comma-separated list of columns.               */   

static ast_t *parse_statement_select(void) {
    parse_expect('(');
    ast_localenv = table_create(ast_localenv);
    ast_t *init = parse_statement_declaration_semicolon();
    ast_t *cond = parse_expression_optional();
    if (cond && ast_type_isfloating(cond->ctype))
        cond = ast_type_convert(ast_data_table[AST_DATA_BOOL], cond);
    parse_expect(';');
    ast_t *step = parse_expression_optional();
    parse_expect(')');
    ast_t *body = parse_statement();
    ast_localenv = table_parent(ast_localenv);
    return ast_for(init, cond, step, body);
}


/*  Parse the FROM clause */ 
static ast_t *parse_statement_while(void) {
    parse_expect('(');
    ast_t *cond = parse_expression_condition();
    parse_expect(')');
    ast_t *body = parse_statement();
    return ast_while(cond, body);
}


/*  Parse the WHERE clause */  
static ast_t *parse_statement_do(void) {
    ast_t         *body  = parse_statement();
    lexer_token_t *token = lexer_next();

    if (!parse_identifer_check(token, "while"))
        compile_error("expected ‘while’ before ‘%s’ token", lexer_token_string(token));

    parse_expect('(');
    ast_t *cond = parse_expression_condition();
    parse_expect(')');
    parse_expect(';');

    return ast_do(cond, body);
}



static ast_t *parse_statement(void) {
    lexer_token_t *token = lexer_next();
    ast_t         *ast;
    
    if (parse_identifer_check(token, "select"))   return parse_statement_select();
    if (parse_identifer_check(token, "from"))     return parse_statement_from();
    if (parse_identifer_check(token, "where"))    return parse_statement_where();
    if (parse_identifer_check(token, "and"))      return parse_statement_and();
    if (parse_identifer_check(token, "or"))       return parse_statement_or();
    if (parse_identifer_check(token, "is"))       return parse_statement_is();
    if (parse_identifer_check(token, "in"))       return parse_statement_in();
    if (parse_identifer_check(token, "not"))      return parse_statement_not();
    if (parse_identifer_check(token, "null"))     return parse_statement_null();

    lexer_unget(token);

    ast = parse_expression_optional();
    parse_expect(';');

    return ast;
}


/*  Main routine for parser  */ 
list_t *parse_run(void) {
    list_t *list = list_create();
    for (;;) {
        if (!lexer_peek())
            return list;        
        else
            parse_declaration(list, &ast_variable_global);
    }
    return NULL;
}


/* Initialise parser  */ 
/* TO DO - need to simplify this.  */ 
void parse_init(void) {
    data_type_t *voidp = ast_pointer(ast_data_table[AST_DATA_VOID]);
    data_type_t *type  = ast_prototype(voidp, list_create(), true);
    data_type_t *addr  = ast_prototype(voidp, list_default(ast_data_table[AST_DATA_ULONG]), true);

    table_insert(ast_globalenv, "__builtin_va_start",       ast_variable_global(type, "__builtin_va_start"));
    table_insert(ast_globalenv, "__builtin_va_arg",         ast_variable_global(type, "__builtin_va_arg"));
    table_insert(ast_globalenv, "__builtin_return_address", ast_variable_global(addr, "__buitlin_return_address"));
}




