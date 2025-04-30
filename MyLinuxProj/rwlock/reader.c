#include "rw.h"

int main() {
    printf("Reader : Writer is begin\n");
    rwlock rw;
    if( 0 > rwlock_init(&rw) ) {
        printf("Reader : rwlock init fail\n");
        return 0;
    }
    else printf("Reader : rwlock init success\n");

    char* shmbuf = shm_init();
    if(!shmbuf) {
        printf("Reader : shared memory init fail\n");
        return 0;
    }
    else printf("Reader : shared memory init success\n");

    char data[100];

    while(1) {
        myRead(&rw, shmbuf, data);
        printf("%s", data);
        usleep(200*1000);
    }

    return 0;
}

