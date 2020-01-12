#define _GNU_SOURCE
#ifndef MONITOR_H
#include "monitor.h"
#endif

int x;
int y;
void display(cell *shared_table)
{
    for (x = 0; x < SO_ALTEZZA; x++)
    {
        if (x == 0)
        {
            for (y = 0; y < SO_BASE; y++)
            {
                if (y == 0)
                {
                    if (SO_ALTEZZA > 10)
                        printf("---%d-", y);
                    else
                        printf("-%d-", y);
                }
                else
                    printf("%d-", y % 10);
            }
            printf("\n");
        }
        for (y = 0; y < SO_BASE; y++)
        {
            if (y == 0)
            {

                if (SO_ALTEZZA > 10 && x < 10)
                    printf("%d |%c|", x, getid(shared_table, x, y));
                else if (SO_ALTEZZA <= 10 || x > 9)
                    printf("%d|%c|", x, getid(shared_table, x, y));
            }
            else
            {
                printf("%c|", getid(shared_table, x, y));
            }
        }
        printf("\n");
    }
}

void display_sem()
{
    int x, y;
    for (x = 0; x < SO_ALTEZZA; x++)
    {
        for (y = 0; y < SO_BASE; y++)
        {
            printf("|%d| ", semctl(sem_table, x * SO_BASE + y, GETVAL));
        }
        printf("\n");
    }
}
