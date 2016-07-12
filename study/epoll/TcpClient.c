#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#define TCP_SERVER_PORT     6666
#define MAX_FILENAME_SIZE   256
#define IP_SIZE             20

#define BUFFER_SIZE         4096



extern int errno;


//  客户端进行的处理逻辑
void RaiseServerResponse(int socketFd);

/* 从服务器上下载文件  */
void TcpClientPullFile(int socketFd, char *filePath);

/* 客户端将文件上传到服务器上 */
void TcpClientPushFile(int socketFd, char *filePath);



int main(int argc, char *argv[])
{
    char                serverIp[IP_SIZE];              /*  服务器的IP地址          */

    if(argc >= 2)                       /*  参数过多的时候，提示用户                */
    {
        printf("You have given to much parameters...\n");
        printf("Yous should give the IP address after %s\n without any other parametes...\n", (char *)argv[0]);
    }
    else if(argc == 1)                  /*  只有一个参数，则默认使用localhost(127.0.0.1)  */
    {
        strcpy(serverIp, "127.0.0.1");
    }
    else
    {
        strcpy(serverIp, argv[1]);
    }

    /**********************************************************
     *
     *  创建并初始化套接字
     *
     **********************************************************/
    struct sockaddr_in  serverAddr;          /*  服务器的套接字信息   */
    int                 socketFd;                      /*  客户端的套接字信息   */

    bzero(&serverAddr, sizeof(serverAddr));             /*  全部置零               */

    serverAddr.sin_family       =   AF_INET;                      /*  internet协议族         */
    serverAddr.sin_addr.s_addr  =   inet_addr(serverIp);        /*  设置所连接服务器的IP   */
    serverAddr.sin_port         =   htons(TCP_SERVER_PORT);       /*  设置连接的服务器端口   */

    /*  开始创建套接字                        */
    /*  SOCK_STREAM 面向连接的套接字，即TCP   */
    socketFd                    =   socket(AF_INET, SOCK_STREAM, 0);

    if(socketFd < 0)
    {
        printf("socket error\n");
        exit(-1);
    }

    /*  尝试连接服务器  */
    if(connect(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        printf("Can Not Connect To %s\n", serverIp);
        exit(1);
    }
    else
    {
        printf("connect to the server %s SUCCESS...\n", serverIp);
        printf("连接服务器成功...\n");
    }

    /**********************************************************
     *
     *  下面进行正常的套接字通信
     *
     **********************************************************/
    RaiseServerResponse(socketFd);

    for(;;);

    //  关闭套接字的文件描述符
    close(socketFd);


    return EXIT_SUCCESS;
}




/* 客户端将文件上传到服务器上 */
void TcpClientPushFile(int socketFd, char *filePath)
{
    FILE    *stream;
    char    buffer[BUFFER_SIZE];
    char    filename[MAX_FILENAME_SIZE];
    int     count = 0;

    bzero(buffer, BUFFER_SIZE);
    strcpy(filename, strrchr(filePath, '/') + 1);
    strncpy(buffer, filename, strlen(filename) > MAX_FILENAME_SIZE ? MAX_FILENAME_SIZE : strlen(filename));

    if((count = send(socketFd, buffer, BUFFER_SIZE, 0)) < 0)
    {
        perror("Send file information");
        exit(1);
    }
    printf("客户端待上传待文件名[%s]..\n", filename);


    /*  打开文件流  */
    if((stream = fopen(filePath, "r")) == NULL)
    {
        printf("Can't open the file [%s], errno = %d\n", filePath, errno);
        exit(-1);
    }
    else
    {
        printf("客户端打开文件成功\n");
    }

    printf("正在向服务器传上传文件...\n");
    count = 0;

    /*  清空缓冲区  */
    bzero(buffer, BUFFER_SIZE);

    /*  不断读取并发送数据  */
    while((count = fread(buffer, 1, BUFFER_SIZE, stream)) > 0)
    {
        // printf("count =%d\n", count);
        if(send(socketFd, buffer, count, 0) < 0)
        {
            printf("send file error...\n");
            break;
        }

        bzero(buffer, BUFFER_SIZE);  /*  再次将缓冲区清空  */
    }

    printf("向服务器发送文件成功...\n");

    /* 传送完毕后， 关闭文件流  */
    if(fclose(stream))
    {
        printf("file close error\n");
        exit(1);
    }
    else
    {
        printf("关闭文件流成功...\n");
    }
    ///  BUG   这里其实有问题
    ///  因为客户端将文件发送完毕后, 服务器是不知道的,
    ///  因此当客户端文件发送完毕后, 服务器会陷入一个死等的循环
    ///  这时一个问题, 但是不是我们代码的重点,
    ///  因为我们的代码, 只是用于学习套接字网络编程
    ///
    ///  这个BUG其实很好处理, 因此我们在网络传输的过程中
    ///  客户端与服务器通信的数据肯定有我们自己的格式或者规范
    ///  比如 [request/response HEAD + LENGTH + DATA]的格式
    ///  要不然连基本的UDP丢包 和 TCP粘包问题都解决不了
    ///  一般情况下, 我们与客户

    /*  关闭与服务器通讯的套接字  */
    //close(socketFd);
}




/* 从服务器上下载文件  */
void TcpClientPullFile(int socketFd, char *filePath)
{
    char    buff[BUFFER_SIZE];
    char    filename[MAX_FILENAME_SIZE];
    int     count, writeLength, dataLength;
    FILE    *stream;
    bzero(buff,BUFFER_SIZE);


    /*  首先获取服务器发送过来的文件名  */
    if((count = recv(socketFd, buff, BUFFER_SIZE, 0)) < 0)
    {
        perror("获取文件名失败...\n");
        exit(1);
    }

    strncpy(filename, buff, strlen(buff) > MAX_FILENAME_SIZE ? MAX_FILENAME_SIZE : strlen(buff));

    /*  开始接收文件  */
    printf("Preparing download file : %s", filename);

    /*  打开文件流  */
    if((stream = fopen(filename, "wb+")) == NULL)
    {
        perror("create file %s error...\n");
        perror("创建文件失败...\n");
        exit(1);
    }

    bzero(buff, BUFFER_SIZE);          /*  清空缓冲区  */
    dataLength = 0;
    while((dataLength = recv(socketFd, buff, BUFFER_SIZE, 0)) != 0)
    {
        if(dataLength < 0)  /* 如果接收文件失败  */
        {
            perror("download error...\n");
            perror("下载文件失败...\n");
            exit(1);
        }


        /*  将接收到的文件数据写入文件中  */
        writeLength = fwrite(buff, sizeof(char), dataLength, stream);
        if(writeLength < dataLength)   /*  如果写入的数据比实际接收到的数据少  */
        {
            perror("file write error...\n");
            perror("写入文件失败...\n");
            exit(1);
        }

        bzero(buff, BUFFER_SIZE);               /* 清空缓冲区  */
    }
    printf("下载来自服务器%s的文件成功\n", filename);
    printf("Receieved file:%s finished!\n", filename);

    fclose(stream);             /*  关闭文件流 */

}



//  客户端进行的处理逻辑
void RaiseServerResponse(int socketFd)
{

    char                buffer[BUFFER_SIZE];            /*  数据缓冲区              */
    int                 count;                          /*  接受或者发送的数据大小  */


    printf("\n\n\n下面将依次测试  发送数据  接收数据  上传文件  下载文件\n\n\n");


    //  发送数据流
    printf("===========send data===========\n");
    bzero(buffer, BUFFER_SIZE);
    strcpy(buffer, "How are you ?");
    if((count = send(socketFd, buffer, strlen(buffer) + 1, 0)) < 0)
    {
        printf("send data[%s] error[errno = %d]...\n", buffer, errno);
        printf("发送数据[%s] 失败[错误码 = %d]...\n", buffer, errno);
    }
    else
    {
        printf("send data[%s] success...\n", buffer);
        printf("发送数据[%s]成功...\n", buffer);
    }
    printf("===========send data===========\n\n\n");


    //  接受数据流
    printf("===========recv data===========\n");
    bzero(buffer, BUFFER_SIZE);
    if((count = recv(socketFd, buffer, BUFFER_SIZE, 0)) < 0)
    {
        printf("recv data error[errno = %d]...\n", errno);
        printf("接收数据失败[错误码 = %d]...\n", errno);
    }
    else
    {
        printf("recv %d data : %s\n", count, buffer);
        printf("接收%d个数据 : %s\n", count, buffer);
    }
    printf("===========recv data===========\n\n\n");

    // 上传文件到服务器
    printf("===========push file===========\n");
    TcpClientPushFile(socketFd, "./cdata/cpush");
    printf("===========push file===========\n\n\n");

    // 上传文件到服务器
    //printf("===========push file===========\n");
    //TcpClientPullFile(socketFd, "./cdata/");
    //printf("===========push file===========\n\n\n");



    //close(socketFd);

}





