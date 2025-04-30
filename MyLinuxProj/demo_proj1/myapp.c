/**
 * @file myapp.c
 * @brief 小明开发的程序处理软件（数据传输框架）
 * @author 庐阳寒月 (lyhyhyl@163.com)
 *         个人主页：
 *          CSDN: https://blog.csdn.net/Mr_Hanc_Tiskor?type=blog
 *          bilibili: https://space.bilibili.com/419816855?spm_id_from=333.1007.0.0
 * @date 2025-04-20
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>

#define PACKET_SIZE     (512 * 512 * sizeof(double) + 1)

void dataProcess(char* _iniData, char* proData);
void imageDisplay(char* _iamgeData);

int initTransRcv(int dataSize_, char** shmbuf_, int* shmid_, int* semid_) {
	printf("initTransRcv: wait send port init\n");	// 等待发送端初始化
	sleep(3);

    *shmid_ = shmget(1, dataSize_, 0666|IPC_CREAT);
    if(*shmid_ == -1) {
        printf("initTransRcv: shared memory init fail\n");
        return -2;
    }
    else printf("initTransRcv: shared memory init success\n");

    *shmbuf_ = (char*)shmat(*shmid_, NULL, 0);
    if(*shmbuf_ == NULL) {
        printf("initTransRcv: shared memory map fail\n");
        return -2;
    }
    else printf("initTransRcv: shared memory map success\n");

    *semid_ = semget(1, 1, 0666|IPC_CREAT); 
    if(*semid_ == -1) {
        printf("initTransRcv: semaphore init fail\n");
        return -2;
    }
    else printf("initTransRcv: semaphore init success\n");
    
    printf("initTransRcv: Trans init success!\n");
    return 0;
}

int deinitTransRcv(char* shmbuf_) {
    if(-1 == shmdt(shmbuf_)) {
        printf("deinitTransRcv: share memory detach fail\n");
        return -3;
    }
    else printf("deinitTransRcv: share memory detach success\n");
    return 0;
}

int dataTransRcv(char* shmbuf_, char* srcbuf_, int dataSize_ ,int semid_) {
    struct sembuf stbu = {0};
    stbu.sem_op = -1;
    stbu.sem_num = 0;
    stbu.sem_flg = 0;
    semop(semid_, &stbu, 1);
    static int lastID = 0;
	if(shmbuf_[0] > lastID) {	// 判断是否接收新到数据，防止反复刷新内存
        printf("dataTransRcv: data is receiving\n");
		memcpy(srcbuf_, shmbuf_, dataSize_);
		lastID = shmbuf_[0];
		printf("dataTransRcv: receive new data(indx = %d)\n", lastID);
	}
    else printf("buf is not flush\n");
    stbu.sem_op = 1;
    semop(semid_, &stbu, 1);
    
    return 0;
}

int app() {
    char* databuf = malloc(PACKET_SIZE);
    char* imagebuf = malloc(PACKET_SIZE);
    if(databuf == NULL || imagebuf == NULL) return -1;

    /* interface */
    int shmid = -1, semid = -1;
    char* shmbuf = NULL;
    initTransRcv(PACKET_SIZE, &shmbuf, &shmid, &semid);
    /* over */

    while(1) {
        dataTransRcv(shmbuf, databuf, PACKET_SIZE, semid);
        dataProcess(databuf, imagebuf);
        imageDisplay(imagebuf);
        usleep(200*1000);
    }
    return deinitTransRcv(shmbuf);
}

int main() {
    int ret = app();
    if(ret != 0) {
        printf("myapp :: error: ");
        if(ret == -1) { printf("malloc error in databuf or imagebuf\n"); }
        else if(ret == -2) { printf("initTransRecv error\n"); }
        else if(ret == -3) { printf("deinitTransRecv error\n"); }
        else if(ret == -4) {}
    }
    return 0;
}

void dataProcess(char* _iniData, char* proData) {
    // 实现你的代码
    return;
}

void imageDisplay(char* _iamgeData) {
    // 绘制你的UI
    return;
}

