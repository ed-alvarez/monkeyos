#include "user.h"
#include "fs.h"

void main(void)
{
    while (1)
    {
    prompt:
        printf("> ");
        char cmdline[128];
        for (int i = 0;; i++)
        {
            char ch = getchar();
            putchar(ch);
            if (i == sizeof(cmdline) - 1)
            {
                printf("command line too long\n");
                goto prompt;
            }
            else if (ch == '\r')
            {
                printf("\n");
                cmdline[i] = '\0';
                break;
            }
            else
            {
                cmdline[i] = ch;
            }
        }

        if (strcmp(cmdline, "touch") == 0)
        {
            const char *filename = cmdline + 6;
            if (*filename == '\0')
                printf("touch: missing filename\n");
            else
                fs_touch(filename);
        }
        else if (strcmp(cmdline, "exit") == 0)
            exit();
        else
            printf("unknown command: %s\n", cmdline);
    }
}