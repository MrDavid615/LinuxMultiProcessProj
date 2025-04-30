#ifndef __RW_H__
#define __RW_H__

#include <stdio.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>

int initState = 0;

typedef struct rwlock {
    int unique;
    int shared;
} rwlock;

union semun {
    int val;                // SETVAL用的值
    struct semid_ds *buf;   // IPC_STAT, IPC_SET用的缓冲区
    unsigned short *array;  // GETALL, SETALL用的数组
};	// 用于追加参数的一个结构体，必须由用户定义


int rwlock_init(rwlock* rw) {
    int ret = 0;
    rw->unique = semget(1, 1, 0666|IPC_CREAT);
    rw->shared = semget(2, 1, 0666|IPC_CREAT);
    if(rw->unique < 0 && rw->shared < 0) return -1;

    if(initState == 0) {
        initState = 1;
        union semun arg;
        arg.val = 1;
        semctl(rw->unique, 0, SETVAL, arg);
        arg.val = 0;
        semctl(rw->shared, 0, SETVAL, arg);
    }

    else return 0;
}


void* shm_init() {
    int id = shmget(1, 128, 0666|IPC_CREAT);
    return shmat(id, NULL, 0);
}

void myWrite(rwlock* rw, char* buf, const char* data) {
    struct sembuf sbf;
    sbf.sem_flg = 0;
    sbf.sem_num = 0;
    sbf.sem_op = -1;
    semop(rw->unique, &sbf, 1);

    int readerNum = semctl(rw->shared, 0, GETVAL);
    printf("Writer : there are %d readers now\n", readerNum);

    sbf.sem_op = 0;
    semop(rw->shared, &sbf, 1);
    printf("Writer : all readers have left, begin writing\n");

    strcpy(buf, data);
    sleep(3);

    printf("Writer : write over\n");
    sbf.sem_op = 1;
    semop(rw->unique, &sbf, 1);
}

void myRead(rwlock* rw, char* buf, char* data) {
    struct sembuf sbf;
    sbf.sem_flg = 0;
    sbf.sem_num = 0;
    sbf.sem_op = -1;
    semop(rw->unique, &sbf, 1);

    sbf.sem_op = 1;
    semop(rw->shared, &sbf, 1);

    sbf.sem_op = 1;
    semop(rw->unique, &sbf, 1);

    printf("Reader : I am reading\n");
    strcpy(data, buf);
    sleep(2);

    printf("Reader : read over\n");
    sbf.sem_op = -1;
    semop(rw->shared, &sbf, 1);
}

#endif

