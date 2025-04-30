/**
 * @file dataGet.c
 * @brief A公司提供的程序源码（数据传输框架）
 * @author 庐阳寒月 (lyhyhyl@163.com)
 *         个人主页：
 *          CSDN: https://blog.csdn.net/Mr_Hanc_Tiskor?type=blog
 *          bilibili: https://space.bilibili.com/419816855?spm_id_from=333.1007.0.0
 * @date 2025-04-20
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>

#define PACKET_SIZE     (512 * 512 * sizeof(double) + 1)    // 数据包的大小

// 检查设备上电状态，返回0表示正常，在下面实现
int device_check(void);

// 外部函数，将数据存储到_buf指向的空间中，空间动态分配，这里在下面实现
// 返回值：传输数据的字节数
int recv_data(char** _buf);

/*
功能：数据传输初始化
参数：
    dataSize_: 	[in]  共享内存大小
    shmbuf_： 	[out] 指向共享内存的映射地址
    shmid_:	  	[out] 共享内存ID
    semid_:		[out] 信号量ID
返回值：
    0：成功
    -2：失败
*/
int initTrans(int dataSize_, char** shmbuf_, int* shmid_, int* semid_) {
    *shmid_ = shmget(1, dataSize_, 0666|IPC_CREAT);
    if(*shmid_ == -1) {
        printf("initTrans: shared memory init fail\n");
        return -2;
    }
    else printf("initTrans: shared memory init success\n");

    *shmbuf_ = (char*)shmat(*shmid_, NULL, 0);
    if(*shmbuf_ == NULL) {
        printf("initTrans: shared memory map fail\n");
        return -2;
    }
    else printf("initTrans: shared memory map success\n");

    *semid_ = semget(1, 1, 0666|IPC_CREAT);
    if(*semid_ == -1) {
        printf("initTrans: semaphore init fail\n");
        return -2;
    }
    else printf("initTrans: semaphore init success\n");
    union sempun {
        int val;                // 用于SETVAL
        struct semid_ds *buf;   // 用于IPC_STAT/IPC_SET
        unsigned short *array;  // 用于GETALL/SETALL
    } arg;
    arg.val = 1;
    if(-1 == semctl(*semid_, 0, SETVAL, arg) ) {
        printf("initTrans: sem init val set fail\n");
        return -2;
    }
    printf("initTrans: sem init val is : %d\n", semctl(*semid_, 0, GETVAL));
    printf("initTrans: Trans init success!\n");
    return 0;
}

/*
功能：资源回收
参数：
    shmbuf_： 	[in]  共享内存的映射地址
    shmid_:	  	[in]  共享内存ID
    semid_:		[in]  信号量ID
返回值：
    0：成功
    -3：失败
*/
int deinitTrans(char* shmbuf_, int shmid_, int semid_) {
    if(-1 == shmdt(shmbuf_))  {
        printf("deinitTrans: share memory detach fail\n");
        return -3;
    }
    else printf("deinitTrans: share memory detach success\n");
    if(-1 == shmctl(shmid_, IPC_RMID, NULL))  {
        printf("deinitTrans: share memory delete fail\n");
        return -3;
    }
    else printf("deinitTrans: share memory delete success\n");
    if( -1 == semctl(semid_, 0, IPC_RMID)) {
        printf("deinitTrans: sem delete fail\n");
        return -3;
    }
    else printf("deinitTrans: sem delete success\n");
    return 0;
}

/*
功能：数据传输
参数：
    shmbuf_： 	[in]  共享内存的映射地址
    srcbuf_:	[in]  存放要传输数据的缓冲区
    dataSize_:	[in]  要传输的数据大小
    semid_:		[in]  信号量ID
返回值：
    0：传输成功
*/
int dataTrans(char* shmbuf_, char* srcbuf_, int dataSize_ ,int semid_) {
    printf("dataTrans: data is sending\n");
    struct sembuf stbu = {0};
    stbu.sem_op = -1;
    stbu.sem_num = 0;
    stbu.sem_flg = 0;
    semop(semid_, &stbu, 1);
    memcpy(shmbuf_, srcbuf_, dataSize_);
    stbu.sem_op = 1;
    semop(semid_, &stbu, 1);
    printf("dataTrans: data send over\n");
    return 0;
}

int getData(void) {
    int ret = 0;
    ret = device_check();
    if(ret != 0) return ret;

    /* another fun */
    int shmid = -1, semid = -1;
    char* shmbuf = NULL;
    if(initTrans(PACKET_SIZE, &shmbuf, &shmid, &semid) != 0) {
        return -2;
    }
    /* over */
    char* buf = NULL;

    while(1) {
        ret = recv_data(&buf);
        if(ret < 0) return ret; // ret = -4

        /* another process */
        dataTrans(shmbuf, buf, ret, semid);
        sleep(1);
        /* over */

        free(buf);
        buf = NULL;
    }
    return deinitTrans(shmbuf, shmid, semid);
}

int main() {
    int ret = getData();
    if(ret < 0) {
        printf("data :: error: ");
        if(ret == -1) printf("device is disconnected\n");
        else if(ret == -2) printf("Trans init error\n");
        else if(ret == -3) printf("Trans deinit error\n");
        else if(ret == -4) printf("data recv error\n");
        else printf("another error\n");
        return 0;
    }
    return 0;
}


int recv_data(char** buf) {
    // 发送数据
    *buf = (char*)malloc(PACKET_SIZE);
    static int i = 0;   
    *buf[0] = i++;  // 数据帧号

    /*
    调用底层接口
    */

    if(*buf == NULL) {
        return -4;
    }
    return PACKET_SIZE;
}

int device_check(void) {
    // 检查外设
    return 0;
}