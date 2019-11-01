

#include "musthave_include.h"


#define CONDITION_JUMP( ZNAK )                               \
{                                                            \
    stack_type a = 0;                                        \
    stack_type b = 0;                                        \
    assert(!stack_pop(&cpu_stack, &a));                      \
    assert(!stack_pop(&cpu_stack, &b));                      \
    if (b ZNAK a)                                            \
    {                                                        \
        int jump_to = get_int_from_buff(code, i + 1);        \
        i = jump_to - 1;                                     \
    }                                                        \
    else                                                     \
        i += sizeof(int);                                    \
    /*printf("Jumping to %d + 1\n\n", i);*/                 \
}

const int CPU_REGISTER_COUNT = 4;
const int RAM_SIZE = 1024;

int run_binary_file(char* bin_name)
{
    stack_t cpu_stack = {};
    STACKCONSTRUCT(&cpu_stack);

    stack_t return_stack = {};
    STACKCONSTRUCT(&return_stack);

    stack_type cpu_register[CPU_REGISTER_COUNT] = {};

    stack_type ram_mem[RAM_SIZE] = {};

    int bin_file_size = 0;

    char* code = file_read(bin_name, &bin_file_size);

    #define LOCAL_COMMAND_DEF(command, to_do, to_write) \
        case CPU_##command:                             \
           /* printf("\n-----\nDoing %s\n-----\n", #command);*/       \
            to_do                                       \
            break;

    for(int i = 0; i < bin_file_size; ++i)
    {
        switch (code[i])
        {
        #include "cpu_commands.h"

        default:
            printf("----------\nCPU got wrong command, check assembler for bugs\n----------\n");
        }
    }

    #undef LOCAL_COMMAND_DEF
    #ifdef _DEBUG
    if (cpu_stack.size > 0)
        while(cpu_stack.size > 0)
        {
            stack_type result = 0;
            stack_pop(&cpu_stack, &result);
            printf("\nCPU STACK POP RESULT: "OUT_TYPE"\n", result);
        }
    else
        printf("\nEMPTY CPU STACK\n");

    printf("\nREGISTER DATA:\n");
    for(int i = 0; i < CPU_REGISTER_COUNT; ++i)
        printf("%cx == "OUT_TYPE"\n", 'a' + i, cpu_register[i]);
    putchar('\n');
    #endif
    stack_destruct(&cpu_stack);
    stack_destruct(&return_stack);
    free(code);
}

stack_type get_num_from_buff(char* code, int code_index)
{
    stack_type value = 0;
    for (int i = 0; i < sizeof(value); ++i)
        *(((char*)(&value)) + i) = code[code_index + i];
    return value;
}

int get_int_from_buff(char* code, int code_index)
{
    int value = 0;
    for (int i = 0; i < sizeof(value); ++i)
        *(((char*)(&value)) + i) = code[code_index + i];
    return value;
}



