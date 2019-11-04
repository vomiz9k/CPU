#include "musthave_include.h"

const int LABEL_COUNT = 64;

int get_binary_file(char* fin_name, char* fout_name)
{
    char command_buffer[10000] = {};

    int printed_chars = 0;

    FILE* fout_global = fopen(fout_name, "w");
    poem asm_text = files_to_poem(fin_name, fout_name);

    label labels[LABEL_COUNT] = {};
    int label_counter = 0;
    int curr_label = 0;

    int ram_param = 0;

    char* curr_command = 0;

    assert(labels && command_buffer && fout_global);

    //printf("STRCNT = %d\n", asm_text.string_count);

    #define LOCAL_COMMAND_DEF(command, args, to_do)                                                 \
    else if (curr_command != 0 && !strcmp(curr_command, #command))                                  \
    {                                                                                               \
                                                                                                    \
        put_to_buff_char(command_buffer, printed_chars++, CPU_##command);                           \
        if(args > 0)                                                                                \
        {                                                                                           \
            char* curr_pos = asm_text.index[i].start + sizeof_word(asm_text.index[i].start);        \
                                                                                                    \
            if (curr_pos && curr_pos[0] == '\"')                                                    \
            {                                                                                       \
                put_to_buff_char(command_buffer, printed_chars, NEXT_STR);                          \
                                                                                                    \
                assert(curr_pos = strtok(curr_pos, "\""));                                          \
                                                                                                    \
                put_to_buff_string(command_buffer, printed_chars + 1, curr_pos);                    \
                                                                                                    \
                printed_chars += sizeof_string(curr_pos);                                           \
                                                                                                    \
                put_to_buff_char(command_buffer, ++printed_chars, 0);                               \
            }                                                                                       \
            else                                                                                    \
            {                                                                                       \
                curr_pos = strtok(NULL, " \t");                                                     \
                                                                                                    \
                if(!curr_pos)                                                                       \
                    put_to_buff_char(command_buffer, printed_chars++, NEXT_NOTHING);                \
                                                                                                    \
                else if (curr_pos && is_register(curr_pos))                                         \
                {                                                                                   \
                    put_to_buff_char(command_buffer, printed_chars++, NEXT_REG);                    \
                    put_to_buff_char(command_buffer, printed_chars++, *curr_pos - 'a' + 1);         \
                }                                                                                   \
                                                                                                    \
                else if ((ram_param = is_RAM(curr_pos)) >= 0)                                       \
                {                                                                                   \
                    put_to_buff_char(command_buffer, printed_chars++, NEXT_RAM);                    \
                                                                                                    \
                    if (ram_param == NEXT_NUM)                                                      \
                    {                                                                               \
                        char* val = strtok(curr_pos, "[]");;                                        \
                        put_to_buff_char(command_buffer, printed_chars++, NEXT_NUM);                \
                                                                                                    \
                        put_to_buff_int(command_buffer, printed_chars, val);                        \
                        printed_chars += sizeof(int);                                               \
                    }                                                                               \
                                                                                                    \
                    else                                                                            \
                    {                                                                               \
                        put_to_buff_char(command_buffer, printed_chars++, NEXT_REG);                \
                        put_to_buff_char(command_buffer, printed_chars++, ram_param);               \
                    }                                                                               \
                }                                                                                   \
                                                                                                    \
                else                                                                                \
                {                                                                                   \
                    put_to_buff_char(command_buffer, printed_chars++, NEXT_NUM);                    \
                                                                                                    \
                    put_to_buff_num(command_buffer, printed_chars, curr_pos);                       \
                                                                                                    \
                    printed_chars += sizeof(stack_type);                                            \
                }                                                                                   \
            }                                                                                       \
        }                                                                                           \
        else if (args == -1)                                                                        \
            make_jump(labels, command_buffer, &printed_chars, &label_counter);         \
    }


    for(int i = 0; i < asm_text.string_count; ++i)
    {
        curr_command = strtok(asm_text.index[i].start, " \t");

        if (false)
            printf("Proizoshel trolling...\n");

        #include "cpu_commands.h"
        else if (curr_command && is_label(curr_command))
        {
            char* name = curr_command;
            *(name + sizeof_word(name) - 2) = 0;

            if ((curr_label = label_exists(name, labels, label_counter)) >= 0)
            {
                for(int j = 0; j < sizeof(int); ++j)
                    put_to_buff_char(command_buffer, labels[curr_label].jmp + j, *((char*)(&printed_chars) + j));

                labels[curr_label].jmp = printed_chars;
            }
            else
            {
                labels[label_counter].name = name;
                labels[label_counter++].jmp = printed_chars;
            }
        }
        else if (!check_syntax(asm_text.index[i]))
        {
            printf("Syntax error on the line %d. Unknown command: \"%s\"\n\n", i + 1, curr_command);
            return 1;
        }
        if (curr_command = strtok(NULL, " \t"))
        {
            printf("Syntax error on the line %d. Wrong arg: \"%s\"\n", i + 1, curr_command);
            return 1;
        }
    }

    #undef LOCAL_COMMAND_DEF
    fwrite(command_buffer, printed_chars, sizeof(char), fout_global);

    get_free(asm_text);
    fclose(fout_global);
    return 0;
}

bool check_syntax(indexes index)
{
    char empty_symbs[] = " \t";
    for(char* str = index.start; str < index.ending; ++str)
        if(!strchr(empty_symbs, *str))
            return false;
    return true;
}

int make_jump(label* labels, char* command_buffer, int* char_count, int* label_counter)
{
    assert(labels && command_buffer && char_count && label_counter);

    int printed_chars = *char_count;
    int curr_label = 0;

    char* curr_pos = strtok(NULL, " \t");

    if ((curr_label = label_exists(curr_pos, labels, *label_counter)) >= 0)
        for(int j = 0; j < sizeof(int); ++j)
            put_to_buff_char(command_buffer, printed_chars + j, *((char*)(&labels[curr_label].jmp) + j));

    else
    {
        labels[*label_counter].name = curr_pos;
        labels[*label_counter].jmp  = printed_chars;

        for(int j = 0; j < sizeof(int); ++j)
            put_to_buff_char(command_buffer, printed_chars + j, -1);

        (*label_counter)++;
    }

    printed_chars += sizeof(int);
    *char_count = printed_chars;
}

int is_RAM(char* str)
{
    assert(str);

    if(str[0] != '[')
        return -1;

    else if (is_register(str + 1))
        return *(str + 1) - 'a' + 1;

    else
        return 0;
}

bool is_register(char* str)
{
    assert(str);

    if ((str[0] == 'a' || str[0] == 'b' || str[0] == 'c' || str[0] == 'd') && str[1] == 'x')
        return true;
    else
        return false;
}

int label_exists(char* name, label* labels, int label_count)
{
    assert(name && labels);

    for(int i = 0; i < label_count; ++i)
        if (!strcmp(labels[i].name, name))
            return i;

    return -1;
}


int put_to_buff_string(char* buff, int printed_chars, char* str)
{
    assert(str && buff);

    sprintf(buff + printed_chars, "%s", str);
}


int put_to_buff_char(char* buff, int printed_chars, char c)
{
    assert(buff);

    buff[printed_chars] = c;
}

int put_to_buff_num(char* buff, int printed_chars, char* value_ptr)
{
    assert(buff && value_ptr);

    stack_type value = TRANSLATE_FUNC(value_ptr);

    for(int i =0; i < sizeof(value); ++i)
        put_to_buff_char(buff, printed_chars + i, *(((char*)(&value)) + i));
}

int put_to_buff_int(char* buff, int printed_chars, char* value_ptr)
{
    assert(buff && value_ptr);

    int value = atoi(value_ptr);

    for(int i =0; i < sizeof(value); ++i)
        put_to_buff_char(buff, printed_chars + i, *(((char*)(&value)) + i));
}

int sizeof_string(char* str)
{
    assert(str);

    int i = 0;
    for(i = 0; str[i] != 0; ++i) ;
    return i + 1;
}

int sizeof_word(char* str)
{
    assert(str);

    int i = 0;
    for(i = 0; str[i] != ' ' && str[i] != 0; ++i) ;
    return i + 1;
}

bool is_label(char* str)
{
    assert(str);

    if (str[sizeof_string(str) - 2] == ':' && *str != ':')
            return true;

    return false;
}
