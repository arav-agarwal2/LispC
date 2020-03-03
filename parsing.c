#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>


long eval(mpc_ast_t* t);

int main(int argc, char ** argv) {
    mpc_parser_t* NumLit = mpc_new("number");
    mpc_parser_t* Bool = mpc_new("bool");
    mpc_parser_t* BinOp = mpc_new("operator");
    mpc_parser_t* Expr = mpc_new("expr");
    mpc_parser_t* LispC = mpc_new("lispc");
    mpca_lang(MPCA_LANG_DEFAULT, 
            "\
            number : /-?[0-9]+.?[0-9]*/ ;\
            bool : /True|False/ ;\
            operator : '+' | '-' | '*' | '/' | '>' | '<' | '%' | /and/ | /or/ ;\
            expr : <bool> | <number> | '(' <expr> <operator> <expr> ')' ;\
            lispc : /^/ <expr>+ /$/ ; \
            ", 
            NumLit, Bool, BinOp, Expr, LispC );
    printf("Lispy First Attempt\n");
    printf("C-c To Exit\n");
    while(1) {
        char* input_prompt = readline(">> ");
        add_history(input_prompt);
        mpc_result_t r;
        if (mpc_parse("<stdin>", input_prompt, LispC, &r)) {
            mpc_ast_print(r.output);
            long result = eval(r.output);
            printf("%li\n", result);
            mpc_ast_delete(r.output);
        } else {
            mpc_err_print(r.error);
            mpc_err_delete(r.error);
        }
        free(input_prompt);
    }
    mpc_cleanup(5, NumLit, Bool, BinOp, Expr, LispC);
}

long eval(mpc_ast_t* t) {
    if(t->children && strstr(t->children[0]->tag, "regex")) {
        printf("Found: %s %s \n",t->children[1]->tag, t->children[1]->contents);
        return eval(t->children[1]);
    }
    else if (strstr(t->tag, "number")) {
        return atof(t->contents);
    }
    else if(strstr(t->tag, "bool")) {
        if(!strcmp(t->contents, "True")) {
            return 1;
        }
        else {
            return 0;
        }
    }
    else {
        if(!strstr(t->children[2]->tag, "expr")) {
            if(!strcmp(t->children[2]->contents, "+")){
                return eval(t->children[1]) + eval(t->children[3]);
            }
            else if(!strcmp(t->children[2]->contents, "*")){
                return eval(t->children[1]) * eval(t->children[3]);
            }
            else if(!strcmp(t->children[2]->contents, "-")){
                return eval(t->children[1]) - eval(t->children[3]);
            }
            else if(!strcmp(t->children[2]->contents, "/")){
                return eval(t->children[1]) / eval(t->children[3]);
            }
            else if(!strcmp(t->children[2]->contents, ">")){
                return eval(t->children[1]) > eval(t->children[3]);
            }
            else if(!strcmp(t->children[2]->contents, "<")){
                return eval(t->children[1]) < eval(t->children[3]);
            }
            else if(!strcmp(t->children[2]->contents, "%")){
                return eval(t->children[1]) % eval(t->children[3]);
            }
            else if(!strcmp(t->children[2]->contents, "and")){
                return eval(t->children[1]) && eval(t->children[3]);
            }
            else if(!strcmp(t->children[2]->contents, "or")){
                return eval(t->children[1]) || eval(t->children[3]);
            }
            else {
                printf("Unsupported operation: %s", t->children[2]->contents);
                return 1;
            }
        }
    }
    return -1;
}
