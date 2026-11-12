#include "expr.h"
#include <assert.h>
#include <math.h>

static double evaluate(const char *text)
{
	queue *infix;
	queue *postfix;
	double result;
	size_t error;

	assert(queue_init(&infix, sizeof(expr_token)) == DS_OK);
	assert(queue_init(&postfix, sizeof(expr_token)) == DS_OK);
	assert(expr_tokenize(text, infix, &error) == DS_OK);
	assert(expr_to_postfix(infix, postfix, &error) == DS_OK);
	assert(expr_eval_postfix(postfix, &result) == DS_OK);

	queue_destroy(infix);
	queue_destroy(postfix);
	return result;
}

int main(void)
{
	size_t error;

	assert(expr_check_brackets("(1 + 2)", &error) == DS_OK);
	assert(expr_check_brackets("(1 + 2", &error) == DS_SYNTAX);
	assert(fabs(evaluate("2 + 3 * 4") - 14.0) < 1e-12);
	assert(fabs(evaluate("-3 + 4") - 1.0) < 1e-12);
	assert(fabs(evaluate("2 * -(1 + 1)") + 4.0) < 1e-12);

	return 0;
}
