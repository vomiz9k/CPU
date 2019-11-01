LOCAL_COMMAND_DEF(EXIT,
    {
        printf("\nEnd.\n\n\n");
        i = bin_file_size;
    },)

LOCAL_COMMAND_DEF(PUSH,
    {
        switch (code[i + 1])
        {
            case NEXT_NUM:

                stack_push(&cpu_stack, get_num_from_buff(code, i + 2));
                i += sizeof(stack_type);

            break;

            case NEXT_REG:

                stack_push(&cpu_stack, cpu_register[code[i + 2] - 1]);
                i++;

            break;

            case NEXT_RAM:

                if (code[i + 2] == NEXT_NUM)
                {
                    stack_push(&cpu_stack, ram_mem[get_int_from_buff(code, i + 3)]);
                    i += sizeof(int) + 1;
                }

                else if (code[i + 2] == NEXT_REG)
                {
                    stack_push(&cpu_stack, ram_mem[(int)cpu_register[code[i + 3] - 1]]);
                    i += 2;
                }

            break;

            default:

                printf("Something wrong with push param, check asm for bugs.\n");

            break;
        }

        i++;
    },

    {
        char* curr_pos = strtok(NULL, " \t");

        if (curr_pos && is_register(curr_pos))
        {
            put_to_buff_char(command_buffer, printed_chars++, NEXT_REG);
            put_to_buff_char(command_buffer, printed_chars++, *curr_pos - 'a' + 1);
        }

        else if ((ram_param = is_RAM(curr_pos)) >= 0)
        {
            put_to_buff_char(command_buffer, printed_chars++, NEXT_RAM);

            if (ram_param == NEXT_NUM)
            {
                char* val = strtok(curr_pos, "[]");;
                put_to_buff_char(command_buffer, printed_chars++, NEXT_NUM);

                put_to_buff_int(command_buffer, printed_chars, val);
                printed_chars += sizeof(int);
            }

            else
            {
                put_to_buff_char(command_buffer, printed_chars++, NEXT_REG);
                put_to_buff_char(command_buffer, printed_chars++, ram_param);
            }
        }

        else
        {
            put_to_buff_char(command_buffer, printed_chars++, NEXT_NUM);

            put_to_buff_num(command_buffer, printed_chars, curr_pos);

            printed_chars += sizeof(stack_type);
        }
    })

LOCAL_COMMAND_DEF(POP,
    {
        switch (code[i + 1])
        {
            case NEXT_NOTHING:
            {
                stack_type trash = 0;
                assert(!stack_pop(&cpu_stack, &trash));
            }
            break;

            case NEXT_REG:

                assert(!stack_pop(&cpu_stack, &(cpu_register[code[i + 2] - 1])));
                i++;

            break;

            case NEXT_RAM:

                if (code[i + 2] == NEXT_NUM)
                {
                    assert(!stack_pop(&cpu_stack, &(ram_mem[get_int_from_buff(code, i + 3)])));
                    i += sizeof(int) + 1;
                }

                else if (code[i + 2] == NEXT_REG)
                {
                    assert(!stack_pop(&cpu_stack, &(ram_mem[(int)cpu_register[code[i + 3] - 1]])));
                    i += 2;
                }

            break;

            default:

                printf("Something wrong with PUSH, check asm for bugs.\n");

            break;
        }
        i += 1;
    },

    {
        char* curr_pos = strtok(NULL, " \t");

        if (curr_pos && is_register(curr_pos))
        {
            put_to_buff_char(command_buffer, printed_chars++, NEXT_REG);
            put_to_buff_char(command_buffer, printed_chars++, *curr_pos - 'a' + 1);
        }

        else if (curr_pos && (ram_param = is_RAM(curr_pos)) >= 0)
        {
            put_to_buff_char(command_buffer, printed_chars++, NEXT_RAM);

            if (ram_param == NEXT_NUM)
            {
                char* val = strtok(curr_pos, "[]");
                put_to_buff_char(command_buffer, printed_chars++, NEXT_NUM);
                put_to_buff_int(command_buffer, printed_chars, val);
                printed_chars += sizeof(int);
            }
            else
            {
                put_to_buff_char(command_buffer, printed_chars++, NEXT_REG);
                put_to_buff_char(command_buffer, printed_chars++, ram_param);
            }
        }

        else
        {
            put_to_buff_char(command_buffer, printed_chars++, NEXT_NOTHING);
        }
    })

LOCAL_COMMAND_DEF(JMP,
    {
        int jump_to = get_int_from_buff(code, i + 1); //0;

        /*for (int j = 0; j < sizeof(int); ++j)
            *((char*)(&jump_to) + j) = code[i + 1 + j];*/

        i = jump_to - 1;
    },

    {
        make_jump(asm_text, labels, command_buffer, &printed_chars, &label_counter, i);
    })

LOCAL_COMMAND_DEF(JA,
    {
        CONDITION_JUMP( > );
    },

    {
        make_jump(asm_text, labels, command_buffer, &printed_chars, &label_counter, i);
    })

LOCAL_COMMAND_DEF(JAE,
    {
        CONDITION_JUMP( >= );
    },

    {
        make_jump(asm_text, labels, command_buffer, &printed_chars, &label_counter, i);
    })

LOCAL_COMMAND_DEF(JB,
    {
        CONDITION_JUMP( < );
    },

    {
        make_jump(asm_text, labels, command_buffer, &printed_chars, &label_counter, i);
    })

LOCAL_COMMAND_DEF(JBE,
    {
        CONDITION_JUMP( <= );
    },

    {
        make_jump(asm_text, labels, command_buffer, &printed_chars, &label_counter, i);
    })

LOCAL_COMMAND_DEF(JE,
    {
        CONDITION_JUMP( == );
    },

    {
        make_jump(asm_text, labels, command_buffer, &printed_chars, &label_counter, i);
    })

LOCAL_COMMAND_DEF(JNE,
    {
        CONDITION_JUMP( != );
    },

    {
        make_jump(asm_text, labels, command_buffer, &printed_chars, &label_counter, i);
    })

LOCAL_COMMAND_DEF(CALL,
    {
        int jump_to = get_int_from_buff(code, i + 1);

        /*for (int j = 0; j < sizeof(int); ++j)
            *((char*)(&jump_to) + j) = code[i + 1 + j];*/

        stack_push(&return_stack, (stack_type)(i + sizeof(int)));
        i = jump_to - 1;
    },

    {
        make_jump(asm_text, labels, command_buffer, &printed_chars, &label_counter, i);
    })

LOCAL_COMMAND_DEF(RETURN,
    {
        stack_type new_i = 0;
        assert(!stack_pop(&return_stack, &new_i));

        i = (int)(new_i);
    },)


LOCAL_COMMAND_DEF(ADD,
    {
        stack_type a = 0;
        stack_type b = 0;
        assert(!stack_pop(&cpu_stack, &a));
        assert(!stack_pop(&cpu_stack, &b));
        stack_push(&cpu_stack, b + a);
    },)

LOCAL_COMMAND_DEF(SUB,
    {
        stack_type a = 0;
        stack_type b = 0;
        assert(!stack_pop(&cpu_stack, &a));
        assert(!stack_pop(&cpu_stack, &b));
        stack_push(&cpu_stack, b - a);
    },)

LOCAL_COMMAND_DEF(PROD,
    {
        stack_type a = 0;
        stack_type b = 0;
        assert(!stack_pop(&cpu_stack, &a));
        assert(!stack_pop(&cpu_stack, &b));
        stack_push(&cpu_stack, b * a);
    },)

LOCAL_COMMAND_DEF(DIV,
    {
        stack_type a = 0;
        stack_type b = 0;
        assert(!stack_pop(&cpu_stack, &a));
        assert(!stack_pop(&cpu_stack, &b));
        stack_push(&cpu_stack, b / a);
    },)

LOCAL_COMMAND_DEF(SQRT,
    {
        stack_type a = 0;
        assert(!stack_pop(&cpu_stack, &a));
        stack_push(&cpu_stack, sqrt(a));
    },)

LOCAL_COMMAND_DEF(SIN,
    {
        stack_type a = 0;
        assert(!stack_pop(&cpu_stack, &a));
        stack_push(&cpu_stack, sin(a));
    },)

LOCAL_COMMAND_DEF(ARCSIN,
    {
        stack_type a = 0;
        assert(!stack_pop(&cpu_stack, &a));
        stack_push(&cpu_stack, asin(a));
    },)

LOCAL_COMMAND_DEF(POWER,
    {
        stack_type a = 0;
        stack_type b = 0;
        assert(!stack_pop(&cpu_stack, &a));
        assert(!stack_pop(&cpu_stack, &b));
        stack_push(&cpu_stack, pow(b, a));
    },)

LOCAL_COMMAND_DEF(LOG,
    {
        stack_type a = 0;
        stack_type b = 0;
        assert(!stack_pop(&cpu_stack, &a));
        assert(!stack_pop(&cpu_stack, &b));
        stack_push(&cpu_stack, log(b)/log(a));
    },)

LOCAL_COMMAND_DEF(IN,
    {
        switch (code[i + 1])
        {
            case NEXT_REG:

                scanf(IN_TYPE, &(cpu_register[code[i + 2] - 1]));
                i++;

            break;

            case NEXT_RAM:

                if (code[i + 2] == NEXT_NUM)
                {
                    scanf(IN_TYPE, &(ram_mem[get_int_from_buff(code, i + 3)]));
                    i += sizeof(int) + 1;
                }

                else if (code[i + 2] == NEXT_REG)
                {
                    scanf(IN_TYPE, &(ram_mem[(int)cpu_register[code[i + 3] - 1]]));
                    i += 2;
                }

            break;

            case NEXT_NOTHING:
            {
                stack_type a = 0;
                scanf(IN_TYPE, &a);
                stack_push(&cpu_stack, a);
            }
            break;

            default:

                printf("Something went wrong with IN, check asm for bugs.\n");

            break;
        }
        i++;
    },

    {
        char* curr_pos = strtok(NULL, " \t");

        if (curr_pos && is_register(curr_pos))
        {
            put_to_buff_char(command_buffer, printed_chars++, NEXT_REG);
            put_to_buff_char(command_buffer, printed_chars++, *curr_pos - 'a' + 1);
        }
        else if (curr_pos && (ram_param = is_RAM(curr_pos)) >= 0)
        {
            put_to_buff_char(command_buffer, printed_chars++, NEXT_RAM);

            if (ram_param == NEXT_NUM)
            {
                char* val = strtok(curr_pos, "[]");
                put_to_buff_char(command_buffer, printed_chars++, NEXT_NUM);
                put_to_buff_int(command_buffer, printed_chars, val);
                printed_chars += sizeof(int);
            }
            else
            {
                put_to_buff_char(command_buffer, printed_chars++, NEXT_REG);
                put_to_buff_char(command_buffer, printed_chars++, ram_param);
            }
        }
        else
            put_to_buff_char(command_buffer, printed_chars++, NEXT_NOTHING);
    })

LOCAL_COMMAND_DEF(OUT,
    {
        switch (code[i + 1])
        {
            case NEXT_REG:

                printf(OUT_TYPE, cpu_register[code[i + 2] - 1]);
                i++;

            break;

            case NEXT_RAM:

                if (code[i + 2] == NEXT_NUM)
                {
                    printf(OUT_TYPE, ram_mem[get_int_from_buff(code, i + 3)]);
                    i += sizeof(int) + 1;
                }

                else if (code[i + 2] == NEXT_REG)
                {
                    printf(OUT_TYPE, ram_mem[(int)cpu_register[code[i + 3] - 1]]);
                    i += 2;
                }

            break;

            case NEXT_STR:

                printf("%s", code + i + 2);
                i += sizeof_string(code + i + 2);

            break;

            case NEXT_SLN:

                printf("%c", '\n');

            break;

            case NEXT_NOTHING:
            {
                stack_type a = 0;
                assert(!stack_pop(&cpu_stack, &a));
                printf(OUT_TYPE, a);
            }
            break;

            default:

                printf("Something went wrong with OUT, check asm for bugs.\n");

            break;
        }
        i++;
    },

    {
        char* need_out = asm_text.index[i].start + sizeof_word(asm_text.index[i].start);

        if (is_register(need_out))
        {
            put_to_buff_char(command_buffer, printed_chars, NEXT_REG);

            put_to_buff_char(command_buffer, ++printed_chars, need_out[0] - 'a' + 1);
            strtok(NULL, " \t");
        }

        else if ((ram_param = is_RAM(need_out)) >= 0)
        {
            put_to_buff_char(command_buffer, printed_chars++, NEXT_RAM);

            if (ram_param == NEXT_NUM)
            {
                char* val = strtok(need_out, "[]");
                put_to_buff_char(command_buffer, printed_chars, NEXT_NUM);

                put_to_buff_int(command_buffer, printed_chars + 1, val);

                printed_chars += sizeof(int);
            }
            else
            {
                put_to_buff_char(command_buffer, printed_chars++, NEXT_REG);
                put_to_buff_char(command_buffer, printed_chars++, ram_param);
                strtok(NULL, " \t");
            }
        }
        else if (need_out[0] == '\"')
        {
            put_to_buff_char(command_buffer, printed_chars, NEXT_STR);

            assert(need_out = strtok(NULL, "\""));

            put_to_buff_string(command_buffer, printed_chars + 1, need_out);

            printed_chars += sizeof_string(need_out) - 1;

            put_to_buff_char(command_buffer, ++printed_chars, 0);
        }

        else if (!strcmp(need_out, "\\n"))
        {
            put_to_buff_char(command_buffer, printed_chars, NEXT_SLN);
            strtok(NULL, " \t");
        }

        else
            put_to_buff_char(command_buffer, printed_chars, NEXT_NOTHING);

        printed_chars++;
    })






