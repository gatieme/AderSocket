/*************************************************************************
    > File Name: 1.c
    > Author: gatieme
    > Created Time: Sun 10 Jul 2016 04:45:17 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main (void)
{
    char IPdotdec[81]; //存放点分十进制IP地址
    struct in_addr s; // IPv4地址结构体

    //输入IP地址
    printf("Please input IP address: ");
    scanf("%s", IPdotdec);

    // 转换
    inet_pton(AF_INET, IPdotdec, (void *)&s);

    printf("inet_pton: 0x%x\n", s.s_addr); // 注意得到的字节序

    // 反转换
    inet_ntop(AF_INET, (void *)&s, IPdotdec, 16);
    printf("inet_ntop: %s\n", IPdotdec);
}
