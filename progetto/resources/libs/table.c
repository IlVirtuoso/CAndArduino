#ifndef TABLE_H
#include "table.h"
#endif

int setid(cell *shared_table, int x, int y, char id, int previous_x, int previous_y)
{
    if ((tab(shared_table, x, y)->id == EMPTY || tab(shared_table, x, y)->id == FLAG) && (semctl(sem_table, x * y + y, GETVAL) == 1)){
        if(getid(shared_table,x,y) == EMPTY){
            debug("Accesso alla cella X:%d Y:%d",x,y);
            if(reserveSem(sem_table,x*SO_BASE + y)){
                error("error in semop while taking the new cell",errno);
            }
            tab(shared_table,x,y)->id = id;
            if(previous_x == -1 && previous_y == -1){
                debug("Nessun Semaforo da sbloccare Causa Posizionamento");
            }
            else{
                if(releaseSem(sem_table,previous_x * SO_BASE + previous_y)){
                    error("error in semop while releasing previous position",errno);
                }
                tab(shared_table,previous_x,previous_y)->id = EMPTY;
            }
            return 1;
        }
        else if(getid(shared_table,x,y) == FLAG){
            debug("cattura bandiera X:%d Y:%d",x,y);
            if(reserveSem(sem_table,x*SO_BASE + y)){
                error("error in semop while taking new cell",errno);
            }
            return -1;
        }
    }
   return 0;
}



char getid(cell *shared_table, int x, int y)
{
    return tab(shared_table, x, y)->id;
}

cell *tab(cell *shared_table, int x, int y)
{
    return (&(*(shared_table + x * SO_BASE + y)));
}

void placeflag(cell *shared_table, int x, int y)
{
    if (strcmp(processSign, "Master"))
    {
        tab(shared_table, x, y)->id = FLAG;
    }
}

void removeflag(cell *shared_table, int x, int y)
{
    if (strcmp(processSign, "Master"))
    {
        tab(shared_table, x, y)->id = EMPTY;
    }
}

void table_start()
{
    int i, j;
    sem_table_key = ftok("./makefile", 'a');
    if ((table = shmget(IPC_PRIVATE, sizeof(cell) * SO_BASE * SO_ALTEZZA, IPC_CREAT | 0666)) > 0)
    {
        debug("Memoria Condivisa Inizializzata");
    }
    else
    {
        error("Errore nell'inizializzazione del segmento di memoria", EKEYREJECTED);
    }
    if ((sem_table = semget(sem_table_key, SO_BASE * SO_ALTEZZA, IPC_CREAT | IPC_EXCL | 0600)) == -1)
    {
        error("Error nella creazione della tabella dei semafori", errno);
    }
    else
    {
        logg("Tabella semafori iniziata");
    }
    for (i = 0; i < SO_ALTEZZA; i++)
    {
        for (j = 0; j < SO_BASE; j++)
        {
            if (initsemAvailable(sem_table, i * SO_BASE + j) == -1)
            {
                error("error nel controllo della sem table", errno);
            }
        }
    }
    logg("tabella semafori configurata");
}

position search(cell *shared_table, int b, int h, char target, int itera)
{
    int x = 1, y = 0, n = 0;
    char flag = 1, z = 0, sign = 1;
    position pos;
    pos.x = b;
    pos.y = h;
    while (flag && n < (SO_BASE * SO_ALTEZZA))
    {
        if (z == 0 && sign)
        {
            b++;
            y++;
        } /* +x */
        else if (z == 1 && sign)
        {
            h++;
            y++;
        } /* +y */
        else if (z == 0 && !sign)
        {
            b--;
            y++;
        } /* -x */
        else if (z == 1 && !sign)
        {
            h--;
            y++;
        } /* -y */

        if (y == x)
        {
            z++;
            y = 0;
        }

        if (z == 2)
        {
            sign = !sign;
            z = 0;
            x++;
            y = 0;
        }

        if (b >= 0 && b < SO_BASE)
        {
            if (h >= 0 && h < SO_ALTEZZA)
            {
                if (getid(shared_table, b, h) == target)
                {
                   if(itera == 1){
                        pos.x = b;
                        pos.y = h;
                        return pos;
                    }else
                    {
                        itera--;
                    }
                    
                   
                }
                n++;
            }
        }
    }
    return pos;
}

int getDistance(int x, int y, int x_targ, int y_targ)
{
    return ABSOLUTE(x - x_targ) + ABSOLUTE(y - y_targ);
}

int reachable(int moves, int x, int y, int x_targ, int y_targ)
{
    int res;
    if ((res = getDistance(x, y, x_targ, y_targ)) <= moves)
    {
        return res;
    }
    else
    {
        return 0;
    }
}

/*End Of Life*/
