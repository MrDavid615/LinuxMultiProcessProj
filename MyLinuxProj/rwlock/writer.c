#include "rw.h"
                        
int main() {
    printf("Writer : Writer is begin\n");
    rwlock rw;
    if(0 > rwlock_init(&rw) ) {
        printf("Writer : rwlock init fail\n");
        return 0;
    }
    else printf("Writer : rwlock init success\n");

    char* buf = shm_init();
    if(!buf) {
        printf("Writer : shared memory init fail\n");
        return 0;
    }
    else printf("Writer : shared memory init success\n");

    while(1) {
        myWrite(&rw, buf, "hello world!\n");
        sleep(3);
    }

    return 0;
}

