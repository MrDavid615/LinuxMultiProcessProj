/**
 * @file dataGet.c
 * @brief A公司提供的程序源码（数据传输框架）
 * @author 庐阳寒月 (lyhyhyl@163.com)
 *         个人主页：
 *          CSDN:       https://blog.csdn.net/Mr_Hanc_Tiskor?type=blog
 *          bilibili:   https://space.bilibili.com/419816855?spm_id_from=333.1007.0.0
 *          GitHub:     https://github.com/MrDavid615
 * @date 2025-05-01
 */

#include "dataGet.h"


int main() {
    signal(SIGINT, exceptionExit);
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


