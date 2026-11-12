#include "expr.h"
#include <ctype.h>
#include <stdlib.h>

static int is_operator(char character)
{
	return character == '+' || character == '-' ||
		   character == '*' || character == '/';
}

ds_status expr_check_brackets(const char *text, size_t *error_pos)
{
	size_t depth = 0;
	size_t index;

	if (text == NULL) {
		return DS_INVALID;
	}

	for (index = 0; text[index] != '\0'; ++index) {
		if (text[index] == '(') {
			++depth;
		} else if (text[index] == ')') {
			if (depth == 0) {
				if (error_pos != NULL) {
					*error_pos = index;
				}
				return DS_SYNTAX;
			}
			--depth;
		}
	}

	if (depth != 0) {
		if (error_pos != NULL) {
			*error_pos = index;
		}
		return DS_SYNTAX;
	}

	if (error_pos != NULL) {
		*error_pos = 0;
	}
	return DS_OK;
}

ds_status expr_tokenize(const char *text, queue *tokens, size_t *error_pos)
{
	size_t index = 0;
	int expect_value = 1;
	char *end;
	expr_token token;

	if (text == NULL || tokens == NULL) {
		return DS_INVALID;
	}

	while (text[index] != '\0') {
		if (isspace((unsigned char)text[index])) {
			++index;
			continue;
		}

		if (text[index] == '(') {
			if (!expect_value) {
				return DS_SYNTAX;
			}
			token = (expr_token){ EXPR_LEFT_PAREN, 0.0, 0 };
			if (queue_push(tokens, &token) != DS_OK) {
				return DS_NOMEM;
			}
			++index;
			continue;
		}

		if (text[index] == ')') {
			if (expect_value) {
				if (error_pos != NULL) {
					*error_pos = index;
				}
				return DS_SYNTAX;
			}
			token = (expr_token){ EXPR_RIGHT_PAREN, 0.0, 0 };
			if (queue_push(tokens, &token) != DS_OK) {
				return DS_NOMEM;
			}
			++index;
			expect_value = 0;
			continue;
		}

		/* Un signe placé juste avant un nombre lui appartient. */
		if (expect_value && (text[index] == '+' || text[index] == '-') &&
			(isdigit((unsigned char)text[index + 1]) || text[index + 1] == '.')) {
			token.kind = EXPR_NUMBER;
			token.operator = 0;
			token.value = strtod(text + index, &end);
			if (end == text + index) {
				return DS_SYNTAX;
			}
			if (queue_push(tokens, &token) != DS_OK) {
				return DS_NOMEM;
			}
			index = (size_t)(end - text);
			expect_value = 0;
			continue;
		}

		if (isdigit((unsigned char)text[index]) || text[index] == '.') {
			if (!expect_value) {
				if (error_pos != NULL) {
					*error_pos = index;
				}
				return DS_SYNTAX;
			}
			token.kind = EXPR_NUMBER;
			token.operator = 0;
			token.value = strtod(text + index, &end);
			if (end == text + index) {
				return DS_SYNTAX;
			}
			if (queue_push(tokens, &token) != DS_OK) {
				return DS_NOMEM;
			}
			index = (size_t)(end - text);
			expect_value = 0;
			continue;
		}

		if (is_operator(text[index])) {
			if (expect_value) {
				if (text[index] != '-') {
					return DS_SYNTAX;
				}
				token = (expr_token){ EXPR_OPERATOR, 0.0, 'u' };
			} else {
				token = (expr_token){ EXPR_OPERATOR, 0.0, text[index] };
			}
			if (queue_push(tokens, &token) != DS_OK) {
				return DS_NOMEM;
			}
			++index;
			expect_value = 1;
			continue;
		}

		if (error_pos != NULL) {
			*error_pos = index;
		}
		return DS_SYNTAX;
	}

	if (expect_value && index != 0) {
		return DS_SYNTAX;
	}

	return DS_OK;
}

