#pragma once
// 编写一个函数，让父进程调用该函数就会产生一个守护进程
/*
守护进程的特点：
1.自成一个会话
2.可以不依赖于终端
3.组长进程不能变成守护进程
4.守护进程不能向显示器打印消息，否则可能会被暂停或终止
5.调用setsid函数可以让一个进程变成守护进程
*/
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
static void MyDaemon() {
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);
    int fd = open("/dev/null", O_RDWR | 0666);
    // printf("%d\n", fd);
    fflush(stdout);
    dup2(fd, 0); // 将标准输入重定向到/dev/null
    dup2(fd, 1);
    dup2(fd, 2); // 将标准输出和标准错误重定向到日志文件
    close(fd);
    if (fork() > 0) {
        exit(0);
    }
    // 走到这里的就是子进程，此时子进程变为孤儿进程，且子进程一定不是组长进程
    setsid(); // 子进程调用setsid变成守护进程，进行服务器端的服务
}