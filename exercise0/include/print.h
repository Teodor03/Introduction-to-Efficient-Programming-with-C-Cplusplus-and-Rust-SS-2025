/**
 * \file
 * declares the functions to be altered by you
 */

#ifndef INCLUDED___EFFI___P0_HELLO___PRINT_H
#define INCLUDED___EFFI___P0_HELLO___PRINT_H

#include <stddef.h>

/**
 * \brief prints the string "Hello World!\n"
 */
void print_helloworld(void);

/**
 * \brief prints a string together with its average character
 * \param string the input string
 */
void print_buggy(const char *string);

/**
 * \brief print the characters in the string sorted by ASCII value
 * \param string the input string
 */
void print_leaky(const char *string);
/**
 * \brief print a triangle of characters
 * 
 * a
 * aa
 * aaa
 * aaaa
 * aaaaa
 * \param c the character to print
 * \param num_lines the number of lines (equivalent to the max number of columns)
 */
void print_very_slowly(char c, size_t num_lines);


/**
 * \brief print a number using effi_putchar
 * \param number the number to print
 * \attention this method need not be altered as it should be correct
 * 		however, there still is an example test case so you can still modify it if you want
 */
void print_number(long long number);

#endif // INCLUDED___EFFI___P0_HELLO___PRINT_H
