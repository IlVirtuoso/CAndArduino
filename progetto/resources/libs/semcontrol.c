#ifndef SEMCONTROL_H
#include "semcontrol.h"
#endif

int releaseSem(int semId, int semNum)
{
    struct sembuf sops;
    switch (semNum)
    {
    case 0:
        debug("sem MASTER_SEM at %d", semctl(semId, semNum, GETVAL));
        break;

    case 1:
        debug("sem PLAYER_SEM at %d", semctl(semId, semNum, GETVAL));
        break;
    default:
        if (strcmp(processSign, "Master"))
            debug("sem PLAYER:%d at %d", (semNum - 2), semctl(semId, semNum, GETVAL));
        else

            debug("sem PIECE:%d at %d", (semNum - 2), semctl(semId, semNum, GETVAL));
        break;
    }

    sops.sem_num = semNum;
    sops.sem_op = 1;
    sops.sem_flg = 0;
    return semop(semId, &sops, 1);
}

int waitzeroSem(int semId, int semNum)
{
    struct sembuf sops;
    switch (semNum)
    {
    case 0:
        debug("sem MASTER_SEM at %d", semctl(semId, semNum, GETVAL));
        break;

    case 1:
        debug("sem PLAYER_SEM at %d", semctl(semId, semNum, GETVAL));

    default:
        if (strcmp(processSign, "Master"))
            debug("sem PLAYER:%d at %d", (semNum - 2), semctl(semId, semNum, GETVAL));
        else

            debug("sem PIECE:%d at %d", (semNum - 2), semctl(semId, semNum, GETVAL));
        break;
    }

    sops.sem_num = semNum;
    sops.sem_op = 0;
    sops.sem_flg = 0;
    return semop(semId, &sops, 1);
}

int reserveSem(int semId, int semNum)
{
    struct sembuf sops;
    switch (semNum)
    {
    case 0:
        debug("sem MASTER_SEM at %d", semctl(semId, semNum, GETVAL));
        break;

    case 1:
        debug("sem PLAYER_SEM at %d", semctl(semId, semNum, GETVAL));
    default:
        if (strcmp(processSign, "Master"))
            debug("sem PLAYER:%d at %d", (semNum - 2), semctl(semId, semNum, GETVAL));
        else

            debug("sem PIECE:%d at %d", (semNum - 2), semctl(semId, semNum, GETVAL));
        break;
    }

    sops.sem_num = semNum;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    return semop(semId, &sops, 1);
}

int initsemReserved(int semId, int semNum)
{
    union semun semun;
    switch (semNum)
    {
    case 0:
        debug("sem MASTER_SEM at %d", 0);
        break;

    case 1:
        debug("sem PLAYER_SEM at %d", 0);
    default:
        if (strcmp(processSign, "Master"))
            debug("sem PLAYER:%d at %d", (semNum - 2), semctl(semId, semNum, GETVAL));
        else

            debug("sem PIECE:%d at %d", (semNum - 2), semctl(semId, semNum, GETVAL));
        break;
    }

    semun.val = 0;
    return semctl(semId, semNum, SETVAL, semun);
}

int initsemAvailable(int semId, int semNum)
{
    union semun semun;
    switch (semNum)
    {
    case 0:
        debug("sem MASTER_SEM at %d", 1);
        break;

    case 1:
        debug("sem PLAYER_SEM at %d", 1);

    default:
        debug("sem PIECE:%d setted %d", (semNum - 2), 1);
        break;
    }

    semun.val = 1;
    return semctl(semId, semNum, SETVAL, semun);
}
