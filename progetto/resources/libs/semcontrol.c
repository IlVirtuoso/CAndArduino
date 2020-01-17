#ifndef SEMCONTROL_H
#include "semcontrol.h"
#endif

int releaseSem(int semId, int semNum)
{
    struct sembuf sops;

    sops.sem_num = semNum;
    sops.sem_op = 1;
    sops.sem_flg = 0;
    return semop(semId, &sops, 1);
}

int waitzeroSem(int semId, int semNum)
{
    struct sembuf sops;

    sops.sem_num = semNum;
    sops.sem_op = 0;
    sops.sem_flg = 0;
    return semop(semId, &sops, 1);
}

int reserveSem(int semId, int semNum)
{
    struct sembuf sops;

    sops.sem_num = semNum;
    sops.sem_op = -1;
    sops.sem_flg = 0;
    return semop(semId, &sops, 1);
}

int initsemReserved(int semId, int semNum)
{
    union semun semun;

    semun.val = 0;
    return semctl(semId, semNum, SETVAL, semun);
}

int initsemAvailable(int semId, int semNum)
{
    union semun semun;

    semun.val = 1;
    return semctl(semId, semNum, SETVAL, semun);
}

int reserveSemNoWait(int semId, int semNum)
{
    struct sembuf sops;
    struct timespec moved, remain;
    moved.tv_nsec = 10;
    moved.tv_sec = 0;
    nanosleep(&moved,&remain);
    sops.sem_num = semNum;
    sops.sem_op = -1;
    sops.sem_flg = IPC_NOWAIT;
    return semop(semId, &sops, 1);
}