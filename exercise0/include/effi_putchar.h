/**
 * \file
 * declares the test function
 */

#ifndef INCLUDED___EFFI___P0_HELLO___EFFI_PUTCHAR_H
#define INCLUDED___EFFI___P0_HELLO___EFFI_PUTCHAR_H


/**
 * \brief external function to allow for testing, basically a putchar forward
 * \param c the character to print
 * \attention multiple implementations exist, sometimes including test hooks
 * \attention this is the only function your code should call for prints,
 * 		everything else is untested
 */
void effi_putchar(char c);

#endif // INCLUDED___EFFI___P0_HELLO___EFFI_PUTCHAR_H
