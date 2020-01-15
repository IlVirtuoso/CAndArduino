#define _GNU_SOURCE
#ifndef MONITOR_H
#include "monitor.h"
#endif

void display(cell *shared_table)
{
    int i, j;
    for (i = 0; i < SO_ALTEZZA; i++)
    {
        if (i < 10)
            printf(" 0%d", i);
        else
            printf(" %d", i);
    }
    printf("\n");
    for (i = 0; i < SO_BASE; i++)
    {
        if (i < 10)
            printf("0%d", i);
        else
            printf("%d", i);
        for (j = 0; j < SO_ALTEZZA; j++)
        {
            if (getid(shared_table, i, j) == EMPTY)
            {
                printf(" %c", '-');
            }
            else
                printf(" %c", getid(shared_table, i, j));
        }
        printf("\n");
    }
    printf("\n");
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
