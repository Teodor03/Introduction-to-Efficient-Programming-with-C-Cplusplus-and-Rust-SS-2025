// headers from exercise
#include "print.h"
#include "effi_putchar.h"

// headers from standard library
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>


void print_helloworld(void) {
    // TODO: empty
}

void print_buggy(const char *string) {
    long long counter = 0;
    size_t sum = 0;
    const char *copy = string;
    while (copy) {
        ++counter;
        sum += *copy++;
    }
    while (string)
        effi_putchar(*string++);

    effi_putchar(':');
    effi_putchar(' ');
    print_number(sum);
    effi_putchar('/');
    print_number(counter);
    effi_putchar('=');
    effi_putchar(counter != 0 ? sum / counter : '0');
    effi_putchar('\n');
}

void print_leaky(const char *string) {
    size_t length = strlen(string);
    char *memory = (char*)malloc(length + 1);

    // try commenting out this line and see what valgrind tells you!
    memory[length] = 0;

    // insertion sort. No benchmark to save you y'all from the horrors that is qsort
    for (size_t i = 0; i < length; ++i) {
        size_t index = 0;
        for (; index < i && memory[index] < string[i]; ++index)
            ;
        memmove(memory + index + 1, memory + index, i - index);
        memory[index] = string[i];
    }

    while (*memory)
        effi_putchar(*memory++);
}

char *append_character(char *buffer, char c) {
    size_t length = strlen(buffer);
    char *new_buffer = (char*)malloc(length + 2); // extra char, null-byte
    assert(new_buffer);
    memcpy(new_buffer, buffer, length);
    new_buffer[length] = c;
    new_buffer[length + 1] = 0;
    free(buffer);
    return new_buffer;
}

void print_very_slowly(char c, size_t num_lines) {
    char *buffer = malloc(1);
    *buffer = 0;
    for (size_t i = 0; i < num_lines; ++i) {
        for (size_t j = 0; j < i; ++j) {
            buffer = append_character(buffer, c);
        }
        buffer = append_character(buffer, '\n');
    }

    // Question for studies: why is this copy needed?
    char *copy = buffer;
    while(*copy)
        effi_putchar(*copy++);
    free(buffer);
}


/**
 * this method need not be altered as it should be correct
 */
void print_number(long long number) {
    if (((unsigned long long) number) == 0x8000000000000000ULL) { // assume two's complement
        // -9223372036854775808
        effi_putchar('-');
        effi_putchar('9');
        effi_putchar('2');
        effi_putchar('2');
        effi_putchar('3');
        effi_putchar('3');
        effi_putchar('7');
        effi_putchar('2');
        effi_putchar('0');
        effi_putchar('3');
        effi_putchar('6');
        effi_putchar('8');
        effi_putchar('5');
        effi_putchar('4');
        effi_putchar('7');
        effi_putchar('7');
        effi_putchar('5');
        effi_putchar('8');
        effi_putchar('0');
        effi_putchar('8');
        return;
    }
    if (number < 0) {
        effi_putchar('-');
        number = -number;
    }

    long long max_divisor = 1000000000000000000LL;
    bool printing_number = false;
    while (max_divisor != 0) {
        long long result = number / max_divisor;
        if (result != 0) {
            printing_number = true;
            effi_putchar('0' + result); // technically UB as ASCII is not defined
        } else if (printing_number) {
            effi_putchar('0');
        }
        number %= max_divisor;
        max_divisor /= 10;
    }
    if (!printing_number) {
        effi_putchar('0');
    }
}
