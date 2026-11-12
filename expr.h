#ifndef EXPR_H
#define EXPR_H

#include <stddef.h>

#include "ds_status.h"
#include "queue.h"

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

ds_status expr_check_brackets(const char *text, size_t *error_pos);
ds_status expr_tokenize(const char *text, queue *tokens, size_t *error_pos);
ds_status expr_to_postfix(queue *infix, queue *postfix, size_t *error_pos);
ds_status expr_eval_postfix(queue *postfix, double *result);

#endif
