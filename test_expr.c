#include "expr.h"
#include <assert.h>
#include <math.h>

static double evaluate(const char *text)
{
	ExprQueue *infix;
	ExprQueue *postfix;
	double result;
	size_t error;

	infix = expr_queue_create();
	postfix = expr_queue_create();
	assert(infix != NULL);
	assert(postfix != NULL);
	assert(expr_tokenize(text, infix, &error) == DS_OK);
	assert(expr_to_postfix(infix, postfix, &error) == DS_OK);
	assert(expr_eval_postfix(postfix, &result) == DS_OK);

	expr_queue_destroy(infix);
	expr_queue_destroy(postfix);
	return result;
}

static DsStatus evaluate_status(const char *text)
{
	ExprQueue *infix;
	ExprQueue *postfix;
	double result;
	size_t error;
	DsStatus status;

	infix = expr_queue_create();
	postfix = expr_queue_create();
	assert(infix != NULL);
	assert(postfix != NULL);

	status = expr_tokenize(text, infix, &error);
	if (status == DS_OK) {
		status = expr_to_postfix(infix, postfix, &error);
	}
	if (status == DS_OK) {
		status = expr_eval_postfix(postfix, &result);
	}

	expr_queue_destroy(infix);
	expr_queue_destroy(postfix);
	return status;
}

int main(void)
{
	size_t error;

	assert(expr_check_brackets("(1 + 2)", &error) == DS_OK);
	assert(expr_check_brackets("(1 + 2", &error) == DS_SYNTAX);
	assert(fabs(evaluate("2 + 3 * 4") - 14.0) < 1e-12);
	assert(fabs(evaluate("-3 + 4") - 1.0) < 1e-12);
	assert(fabs(evaluate("2 * -(1 + 1)") + 4.0) < 1e-12);
	assert(expr_check_brackets("1 + )", &error) == DS_SYNTAX);
	assert(evaluate_status("4 / 0") == DS_DIVZERO);
	assert(evaluate_status("1 +") == DS_SYNTAX);

	return 0;
}
