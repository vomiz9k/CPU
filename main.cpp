#include "musthave_include.h"
#define _DEBUG


int main()
{
    int check_asm = get_binary_file("fibbonacci.txt", "output.bin");
    if (!check_asm)
        run_binary_file("output.bin");
    else
        return check_asm;
}
