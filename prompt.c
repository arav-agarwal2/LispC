#include <stdio.h>
#include <stdlib.h>

#include <editline/readline.h>
#include <editline/history.h>

int main(int argc, char ** argv) {

    printf("Lispy First Attempt\n");
    printf("C-c To Exit\n");
    while(1) {
        // readline: Seems like a more fun line reading tool, though I won't be 
        // able to use this for actual tooling haha.
        char* input_prompt = readline(">> ");
        // As I'm essentially following along here, I'm gonna comment out more than
        // I'd normally. 
        // New Library: editline! (Installed through libedit-dev)
        // This allows for generally nicer prompt design, such as handling 
        // <>^V and keeping histories alive
        //
        // add_history: This adds our prompt to my history (which is surprisingly like the normal one)
        add_history(input_prompt);
        printf("You typed in: %s\n", input_prompt);
        free(input_prompt);
    }
}
