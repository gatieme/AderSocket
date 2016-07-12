//client.cpp
#include<sys/types.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<strings.h>
#include<string.h>
#include<arpa/inet.h>
#include<errno.h>
#include<stdio.h>
#include<pthread.h>


#define SERVER_PORT 6666

pthread_mutex_t mut=PTHREAD_MUTEX_INITIALIZER;    //互斥量
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;  //条件变量

int cond_value=1;
struct sockaddr_in servaddr;

void *handleFun(void *arg)
{
    int sockfd=*((int*)arg);

    {
        pthread_cond_wait(&cond,&mut);
        pthread_mutex_unlock(&mut);
        //信号会丢失，使得这里永远醒不了，所以需要重发信号.

        int conRes=0;
        conRes=connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

        printf("%d\n",sockfd); //如果不加connect，那么这里显示正确
        if(conRes==-1)
        {
            printf("connect error: %s\n",strerror(errno));
            return 0;
        }

    }

}

void *handleFun2(void *arg)
{
    *((int*)arg)=2;
    pthread_cond_broadcast(&cond);
}

int main(int argc, char **argv)
{
    int testCount=300;
    int sockfd[testCount];
    pthread_t ntid[testCount];

    //tcpcli <ipaddress> <data>
    if(argc!=3)
    return -1;

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(SERVER_PORT);
    inet_pton(AF_INET,argv[1],&servaddr.sin_addr);

    int testCaseIndex=0;
    for(testCaseIndex=0;testCaseIndex<testCount;testCaseIndex++)
    {
        sockfd[testCaseIndex]=socket(AF_INET,SOCK_STREAM,0);
        //为每个客户端线程创建socket
        if(sockfd[testCaseIndex]==-1)
        {
            printf("socket established error: %s\n",(char*)strerror(errno));

            return -1;
        }

        if(pthread_create(&ntid[testCaseIndex],NULL,handleFun,&sockfd[testCaseIndex])!=0)
        //客户端线程
        {
            printf("create thread error :%s\n",strerror(errno));
            return -1;
        }
    }

    printf("%d client has initiated\n",testCaseIndex);

    cond_value=2;
    while(1)
    {
        sleep(2);
        pthread_cond_broadcast(&cond);  //条件满足后发信号通知所有阻塞在条件变量上的线程！
    }
    exit(0);
}
