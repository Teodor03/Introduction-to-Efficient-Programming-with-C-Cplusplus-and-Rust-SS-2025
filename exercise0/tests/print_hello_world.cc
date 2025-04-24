extern "C" {
#include "print.h"
}

#include <cstring>
#include <gtest/gtest.h>


const char *hello_world_string = "Hello World!\n";
size_t call_counter = 0;

extern "C" {
void effi_putchar(char c) {
    ASSERT_LT(call_counter, strlen(hello_world_string));
    EXPECT_EQ(c, hello_world_string[call_counter++]);
}
}


TEST(PrintHelloWorld, normal) {
    print_helloworld();
    EXPECT_EQ(call_counter, strlen(hello_world_string));
}
