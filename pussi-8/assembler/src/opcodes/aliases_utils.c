#include "aliases_utils.h"

#include <stdarg.h>
#include <string.h>

#include "queue/queue.h"

struct instruction *instruction_helper(int line, int real_line, char *opcode,
                                       int args_num, ...)
{
    struct token *out_opcode = token_create(OPCODE, opcode, strlen(opcode));

    if (out_opcode == NULL)
    {
        token_destroy(out_opcode, 1);
        return NULL;
    }

    struct instruction *instruction =
        instruction_create(line, real_line, out_opcode);
    if (instruction == NULL)
    {
        token_destroy(out_opcode, 1);
        return NULL;
    }

    va_list args;
    va_start(args, args_num);

    for (int i = 0; i < args_num; i++)
    {
        struct token *arg = va_arg(args, struct token *);
        if (queue_enqueue(instruction->args_queue, arg))
        {
            instruction_destroy(instruction);
            return NULL;
        }
    }

    va_end(args);

    return instruction;
}
