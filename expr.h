#ifndef EXPR_H
#define EXPR_H

#include <stddef.h>

#include "ds_status.h"
typedef enum {
	EXPR_NUMBER,
	EXPR_OPERATOR,
	EXPR_LEFT_PAREN,
	EXPR_RIGHT_PAREN
} expr_token_kind;

typedef struct {
	expr_token_kind kind;
	double value;
	char operator;
} expr_token;

typedef struct ExprQueue ExprQueue;

ExprQueue *expr_queue_create(void);
void expr_queue_destroy(ExprQueue *queue);
int expr_queue_is_empty(const ExprQueue *queue);
size_t expr_queue_size(const ExprQueue *queue);
DsStatus expr_queue_push(ExprQueue *queue, const expr_token *token);
DsStatus expr_queue_pop(ExprQueue *queue, expr_token *token);

DsStatus expr_check_brackets(const char *text, size_t *error_pos);
DsStatus expr_tokenize(const char *text, ExprQueue *tokens, size_t *error_pos);
DsStatus expr_to_postfix(ExprQueue *infix, ExprQueue *postfix, size_t *error_pos);
DsStatus expr_eval_postfix(ExprQueue *postfix, double *result);

#endif
