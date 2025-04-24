#include "print.h"
#include "effi_putchar.h"

#include <stdio.h>

void effi_putchar(char c) {
    putchar(c);
}

int main() {
    printf("First step, print hello world:\n");
    print_helloworld();

    printf("Second step, print string with length:\n");
    print_buggy("This will crash");

    printf("Third step, print a sorted string:\n");
    print_leaky("zyxwvutsrqponmlkjihgfedcba");

    printf("Final step, print a wonderful triangle that screams at you:\n");
    print_very_slowly('A', 50);
}
