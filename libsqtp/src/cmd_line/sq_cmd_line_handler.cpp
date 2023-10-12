#include "cmd_line/sq_cmd_line_handler.h"
#include <string.h>
#include <stdlib.h>

namespace sq
{
    sq_cmd_line_handler::sq_cmd_line_handler(/* args */)
    {
    }
    
    sq_cmd_line_handler::~sq_cmd_line_handler()
    {
    }
    void sq_cmd_line_handler::run()
    {
        static char *line = (char *)NULL;
        while (true)
        {
            if (line)
            {
                free(line);
                line = NULL;
            }
            line = readline(">");
            if (line && *line)
                add_history(line);
            if (strcmp("quit", line) == 0 ||
                strcmp("q", line) == 0)
            {
                exit(0);
            }
            else
            {
                handle_cmd(line);
            }
        }
    }
} // namespace name
