#include "expr.h"

#include <ctype.h>
#include <stdlib.h>

struct ExprQueue {
    expr_token *data;
    size_t head;
    size_t size;
    size_t capacity;
};

static int is_operator(char character)
{
    return character == '+' || character == '-' ||
           character == '*' || character == '/';
}

static int precedence(char operator)
{
    if (operator == 'u') {
        return 3;
    }
    if (operator == '*' || operator == '/') {
        return 2;
    }
    return 1;
}

static DsStatus grow_tokens(ExprQueue *queue)
{
    expr_token *data;
    size_t index;

    data = malloc(queue->capacity * 2 * sizeof(*data));
    if (data == NULL) {
        return DS_NOMEM;
    }

    for (index = 0; index < queue->size; ++index) {
        data[index] = queue->data[(queue->head + index) % queue->capacity];
    }

    free(queue->data);
    queue->data = data;
    queue->head = 0;
    queue->capacity *= 2;
    return DS_OK;
}

ExprQueue *expr_queue_create(void)
{
    ExprQueue *queue = calloc(1, sizeof(*queue));

    if (queue == NULL) {
        return NULL;
    }

    queue->capacity = 8;
    queue->data = malloc(queue->capacity * sizeof(*queue->data));
    if (queue->data == NULL) {
        free(queue);
        return NULL;
    }

    return queue;
}

void expr_queue_destroy(ExprQueue *queue)
{
    if (queue != NULL) {
        free(queue->data);
        free(queue);
    }
}

int expr_queue_is_empty(const ExprQueue *queue)
{
    return queue == NULL || queue->size == 0;
}

size_t expr_queue_size(const ExprQueue *queue)
{
    return queue == NULL ? 0 : queue->size;
}

DsStatus expr_queue_push(ExprQueue *queue, const expr_token *token)
{
    DsStatus status;
    size_t position;

    if (queue == NULL || token == NULL) {
        return DS_BADARG;
    }

    if (queue->size == queue->capacity) {
        status = grow_tokens(queue);
        if (status != DS_OK) {
            return status;
        }
    }

    position = (queue->head + queue->size) % queue->capacity;
    queue->data[position] = *token;
    ++queue->size;
    return DS_OK;
}

DsStatus expr_queue_pop(ExprQueue *queue, expr_token *token)
{
    if (queue == NULL || token == NULL) {
        return DS_BADARG;
    }

    if (queue->size == 0) {
        return DS_EMPTY;
    }

    *token = queue->data[queue->head];
    queue->head = (queue->head + 1) % queue->capacity;
    --queue->size;
    return DS_OK;
}

DsStatus expr_check_brackets(const char *text, size_t *error_pos)
{
    size_t depth = 0;
    size_t index;

    if (text == NULL) {
        return DS_BADARG;
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

DsStatus expr_tokenize(const char *text, ExprQueue *tokens, size_t *error_pos)
{
    size_t index = 0;
    int expect_value = 1;
    char *end;
    expr_token token;

    if (text == NULL || tokens == NULL) {
        return DS_BADARG;
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
            if (expr_queue_push(tokens, &token) != DS_OK) {
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
            if (expr_queue_push(tokens, &token) != DS_OK) {
                return DS_NOMEM;
            }
            ++index;
            expect_value = 0;
            continue;
        }

        /* Un signe place juste avant un nombre lui appartient. */
        if (expect_value && (text[index] == '+' || text[index] == '-') &&
            (isdigit((unsigned char)text[index + 1]) || text[index + 1] == '.')) {
            token.kind = EXPR_NUMBER;
            token.operator = 0;
            token.value = strtod(text + index, &end);
            if (end == text + index) {
                return DS_SYNTAX;
            }
            if (expr_queue_push(tokens, &token) != DS_OK) {
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
            if (expr_queue_push(tokens, &token) != DS_OK) {
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
            if (expr_queue_push(tokens, &token) != DS_OK) {
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
    return expr_queue_size(tokens) == 0 ? DS_SYNTAX : DS_OK;
}

DsStatus expr_to_postfix(ExprQueue *infix, ExprQueue *postfix, size_t *error_pos)
{
    expr_token operators[128];
    size_t operator_count = 0;
    expr_token token;
    expr_token top;
    int right_associative;

    if (infix == NULL || postfix == NULL) {
        return DS_BADARG;
    }

    while (!expr_queue_is_empty(infix)) {
        if (expr_queue_pop(infix, &token) != DS_OK) {
            return DS_BADARG;
        }

        if (token.kind == EXPR_NUMBER) {
            if (expr_queue_push(postfix, &token) != DS_OK) {
                return DS_NOMEM;
            }
            continue;
        }

        if (token.kind == EXPR_LEFT_PAREN) {
            if (operator_count == 128) {
                return DS_NOMEM;
            }
            operators[operator_count++] = token;
            continue;
        }

        if (token.kind == EXPR_RIGHT_PAREN) {
            while (operator_count > 0 && operators[operator_count - 1].kind != EXPR_LEFT_PAREN) {
                --operator_count;
                if (expr_queue_push(postfix, &operators[operator_count]) != DS_OK) {
                    return DS_NOMEM;
                }
            }
            if (operator_count == 0) {
                if (error_pos != NULL) {
                    *error_pos = 0;
                }
                return DS_SYNTAX;
            }
            --operator_count;
            continue;
        }

        right_associative = token.operator == 'u';
        while (operator_count > 0 && operators[operator_count - 1].kind == EXPR_OPERATOR) {
            top = operators[operator_count - 1];
            if (precedence(top.operator) < precedence(token.operator) ||
                (right_associative && precedence(top.operator) == precedence(token.operator))) {
                break;
            }
            --operator_count;
            if (expr_queue_push(postfix, &top) != DS_OK) {
                return DS_NOMEM;
            }
        }
        if (operator_count == 128) {
            return DS_NOMEM;
        }
        operators[operator_count++] = token;
    }

    while (operator_count > 0) {
        --operator_count;
        top = operators[operator_count];
        if (top.kind == EXPR_LEFT_PAREN) {
            if (error_pos != NULL) {
                *error_pos = 0;
            }
            return DS_SYNTAX;
        }
        if (expr_queue_push(postfix, &top) != DS_OK) {
            return DS_NOMEM;
        }
    }

    return DS_OK;
}

DsStatus expr_eval_postfix(ExprQueue *postfix, double *result)
{
    double values[128];
    size_t value_count = 0;
    expr_token token;
    double left;
    double right;

    if (postfix == NULL || result == NULL) {
        return DS_BADARG;
    }

    while (!expr_queue_is_empty(postfix)) {
        if (expr_queue_pop(postfix, &token) != DS_OK) {
            return DS_BADARG;
        }

        if (token.kind == EXPR_NUMBER) {
            if (value_count == 128) {
                return DS_NOMEM;
            }
            values[value_count++] = token.value;
            continue;
        }

        if (token.operator == 'u') {
            if (value_count == 0) {
                return DS_SYNTAX;
            }
            values[value_count - 1] = -values[value_count - 1];
            continue;
        }

        if (value_count < 2) {
            return DS_SYNTAX;
        }
        right = values[--value_count];
        left = values[--value_count];

        if (token.operator == '/' && right == 0.0) {
            return DS_DIVZERO;
        }
        switch (token.operator) {
        case '+':
            values[value_count++] = left + right;
            break;
        case '-':
            values[value_count++] = left - right;
            break;
        case '*':
            values[value_count++] = left * right;
            break;
        case '/':
            values[value_count++] = left / right;
            break;
        default:
            return DS_SYNTAX;
        }
    }

    if (value_count != 1) {
        return DS_SYNTAX;
    }
    *result = values[0];
    return DS_OK;
}
