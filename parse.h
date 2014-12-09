

/*  parse.h  */  

#ifndef LICE_PARSE_HDR
#define LICE_PARSE_HDR
#include "ast.h"



/*
 * Function: parse_run
 *  Main entry point for the parser.
 *
 * Returns:
 *  Will produce a list of AST toplevel expressions which can be handed
 *  down to the code generator one at a time.
 */
list_t *parse_run(void);

/*
 * Function: parse_init
 *  Main initialization for the global parser context.
 *
 * Remarks:
 *  This must be called before calling <parse_run>.
 */
void parse_init(void);

/*
 * Function: parse_expect
 *  When expecting language punctuation, this function is assumed to
 *  ensure that the following lexer state does indeed contain that
 *  punctuator.
 *
 * Parameters:
 *  punct   - A character literal of the punctuation.
 *
 * Remarks:
 *  If the passed punctuator isn't resolved as the current lexer state
 *  a lexer error is raised.
 */
void parse_expect(char punct);

/*
 * Function: parse_next
 *  Reads the next token in the token stream and determines if it's
 *  the token specified by the argument, ungetting the token if it is,
 *  ignoring it if it isn't.
 *
 * Parameters:
 *  ch - The token to check
 *
 * Returns:
 *  true if the token passed matches the next token in the token stream,
 *  false otherwise.
 *
 * Remarks:
 *  This will advance lexer state only if the token specified as a
 *  parameter isn't determined as the next token in the token stream.
 */
bool parse_next(int ch);

/*
 * Function: parse_expression_evaluate
 *  Reads a conditional expression and evaluates it as an integer constant
 *  expression if it one.
 *
 * Returns:
 *  An integer constant value of the evaluation of the integer constant
 *  expression.
 *
 * Remarks:
 *  Will raise a compilation error if it isn't a valid integer constant
 *  expression.
 */
int parse_expression_evaluate(void);


/* TODO: remove */
void parse_semantic_assignable(data_type_t *to, data_type_t *from);
ast_t       *parse_expression_assignment(void);

#endif



