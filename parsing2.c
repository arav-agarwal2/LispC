#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <editline/readline.h>
#include <editline/history.h>



typedef struct {
    int type;
    double num;
    int err;
} lval;

lval eval(mpc_ast_t* t);
enum { LVAL_NUM, LVAL_ERR };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

lval lval_num(double x) {
    lval v;
    v.type = LVAL_NUM;
    v.num = x;
    return v;
}

lval lval_err(int x) {
    lval v;
    v.type = LVAL_ERR;
    v.err = x;
    return v;
}

void lval_print(lval v) {
    switch (v.type) {
        case LVAL_NUM: printf("%f\n", v.num); break;
        case LVAL_ERR:
                       if (v.err == LERR_DIV_ZERO) {
                           printf("Error: Division By Zero!\n");

                       }
                       if (v.err == LERR_BAD_OP)   {
                           printf("Error: Invalid Operator!\n");

                       }
                       if (v.err == LERR_BAD_NUM)  {
                           printf("Error: Invalid Number!\n");

                       }
                       break;

    }

}


int main(int argc, char ** argv) {
    mpc_parser_t* Number   = mpc_new("number");
    mpc_parser_t* Operator = mpc_new("operator");
    mpc_parser_t* Expr     = mpc_new("expr");
    mpc_parser_t* Lispy    = mpc_new("lispy");

    mpca_lang(MPCA_LANG_DEFAULT,
            "                                                     \
            number   : /-?[0-9]+.?[0-9]*/ ;                             \
            operator : '+' | '-' | '*' | '/' | '%' | '^' | /min/ | /max/ ;                  \
            expr     : <number> | '(' <operator> <expr>+ ')' ;  \
            lispy    : /^/ <operator> <expr>+ /$/ ;             \
            ",
            Number, Operator, Expr, Lispy);

    puts("Lispy Version 0.0.0.0.2");
    puts("Press Ctrl+c to Exit\n");

    while (1) {

        char* input = readline("lispy> ");
        add_history(input);

        mpc_result_t r;
        if (mpc_parse("<stdin>", input, Lispy, &r)) {

            lval result  = eval(r.output);
            lval_print(result);
            mpc_ast_delete(r.output);

        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);

        }

        free(input);

    }

    mpc_cleanup(4, Number, Operator, Expr, Lispy);

    return 0;
}

lval eval_op(lval x, char* op, lval y) {
    if (x.type == LVAL_ERR) { return x;  }
    if (y.type == LVAL_ERR) { return y;  }

    /* Otherwise do maths on the number values */
    if (strcmp(op, "+") == 0) { return lval_num(x.num + y.num);  }
    if (strcmp(op, "-") == 0) { return lval_num(x.num - y.num);  }
    if (strcmp(op, "*") == 0) { return lval_num(x.num * y.num);  }
    if (strcmp(op, "/") == 0) {
        /* If second operand is zero return error */
        return y.num == 0
            ? lval_err(LERR_DIV_ZERO)
            : lval_num(x.num / y.num);

    }

    return lval_err(LERR_BAD_OP);
}

lval eval(mpc_ast_t* t) {
    if (strstr(t->tag, "number")) {
        /* Check if there is some error in conversion */
        errno = 0;
        long x = strtof(t->contents, NULL);
        return errno != ERANGE ? lval_num(x) : lval_err(LERR_BAD_NUM);

    }

    char* op = t->children[1]->contents;
    lval x = eval(t->children[2]);

    int i = 3;
    while (strstr(t->children[i]->tag, "expr")) {
        x = eval_op(x, op, eval(t->children[i]));
        i++;

    }

    return x;
}

