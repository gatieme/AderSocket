/*************************************************************************
    > File Name: TcpClient.c
    > Author: GatieMe
    > Mail: gatieme@163.com
    > Created Time: 2015年12月09日 星期三 16时26分46秒
 ************************************************************************/

/**********************************************************
    > File Name: server.c
    > Author: GatieMe
    > Mail: gatieme@163.com
    > Created Time: 2015年04月11日 星期六 16时22分10秒
 *********************************************************/


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

#define TCP_SERVER_PORT     6666    /*  服务器的端口  */
#define BUFFER_SIZE         4096
#define IP_SIZE             20
#define MAX_FILENAME_SIZE   256
#define LISTEN_QUEUE        20


extern int errno;


/* 服务器接收从客户端传送来的文件  */
void
TcpServerPullFile(
            int         connFd,                     /*  服务器与客户端通讯的套接字文件  */
            struct      sockaddr_in clientAddr,     /*  与之通信的客户端的信息  */
            char        *fileServerRoot);           /*  上传文件的存储路径  */



/*  服务器将文件发送到客户端  */
void TcpServerPushFile(
                    int         connFd,                  /*  服务器与客户端通讯的套接字文件  */
                    struct      sockaddr_in clientAddr,  /*  与之通信的客户端的信息  */
                    char        *filePath);              /*  带发送至客户端的文件路径  */


/*  处理子进程退出的信号处理函数  */
void SignalChild(int signo);                        /* 信号的标示信息         */

/// 处理客户端的请求信息
void RaiseClientRequest(
        int connFd,     /*  客户端的连接套接字描述符, 用于发送和接收数据  */
        struct sockaddr_in  clientAddr); /*  客户端的信息, 用于显示一些客户端的信息  */





int main(int argc, char *argv[])
{
    /**********************************************************
     *
     *  创建并初始化服务器套接字
     *
     **********************************************************/
    struct sockaddr_in      serverAddr;
    int                     socketFd;

    bzero(&serverAddr, sizeof(serverAddr));     /*  全部置零  */

    /* 设置地址相关的属性 */
    serverAddr.sin_family         =   AF_INET;
    serverAddr.sin_addr.s_addr    =   htons(INADDR_ANY);
    serverAddr.sin_port           =   htons(TCP_SERVER_PORT);

    /*  创建套接字  */
    socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if(socketFd < 0)
    {
        perror("socket create error\n");
        exit(-1);
    }
    else
    {
        printf("socket create success...\n");
        printf("创建套接字成功[errno = %d]...\n", errno);
    }

    /*  绑定端口  */
    /**********************************************************
     *
     *  命名服务器的套接字, 进行BIND端口绑定
     *
     **********************************************************/
    if(bind(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) > 0)
    {
        perror("bind error\n");
        exit(-1);
    }
    else
    {
        printf("server bind port %d success...\n", TCP_SERVER_PORT);
        printf("服务器绑定端口%d成功...\n", TCP_SERVER_PORT);
    }

    /*  开始监听绑定的端口  */
    /**********************************************************
     *
     *  开始监听服务器绑定的端口
     *
     **********************************************************/
    if(listen(socketFd, LISTEN_QUEUE))
    {
        printf("Server listen error[errno = %d]...\n", errno);
        exit(-1);
    }
    else
    {
        printf("Server listen success...\n");
        printf("服务器开始监听...\n");
    }

    struct sockaddr_in  clientAddr;
    socklen_t           length = sizeof(clientAddr);
    int                 connFd;
    int                 childPid;

    /// modify by gatieme at 2015-12-10 16:12
    /// 添加了对子进程退出的异常信号处理
    //
    //  对父进程使用fork派生子进程后，如果子进程运行结束，那么该进程不会立刻被销毁，
    //  而会进入“僵尸状态”，仍然维护着自身的信息，
    //  这时候如果服务器父进程不加以处理，
    //  那么很快就会消耗完系统的内存空间，所以父进程需要监听子进程SIGCHLD信号，
    //  并做出处理以销毁残留信息，这里可以使用wait或者waitpid来实现。
    //  我们在父进程调用listen之后，
    //  注册监听信号和信号处理函数signal(SIGCHLD, SignalChild);
    //
    //  SIGCHLD 进程Terminate或Stop的时候，SIGCHLD会发送给它的父进程。
    //  缺省情况下该Signal会被忽略
    signal(SIGCHLD, SignalChild);                //子进程退出的信号处理


    while( 1 )
    {
        /* accept返回一个新的套接字与客户端进行通信  */
        /**********************************************************
         *
         *  ACCEPT返回一个新的套接字与客户端进行通信
         *
         **********************************************************/
        if((connFd = accept(socketFd, (struct sockaddr*)&clientAddr, &length)) < 0)
        {
            printf("accept error, errno = %d...\n", errno);
            continue;
        }
        else
        {
            ////////////////////////////////////////////////////////////////////////
            //
            //  这里填写服务器的处理代码
            //
            ////////////////////////////////////////////////////////////////////////
            printf("\n\naccept connect from client %s\n",inet_ntoa(clientAddr.sin_addr));
            printf("获取到从客户端%s的连接...\n\n\n", inet_ntoa(clientAddr.sin_addr));

            if((childPid = fork( )) == 0)       //  FORK在子进程中返回0, 在父进程中返回子进程的ID
            {
                //  千万不要以为fork出来一个子进程就产生了2个新的socket描述符，
                //  实际上子进程和父进程是共享socketFd和connFd的，
                //  每个文件和套接字都有一个引用计数，
                //  引用计数在文件表项中维护，
                //  它是当前打开着的引用该文件或套接字的描述符的个数。
                //  socket返回后与socketFd关联的文件表项的引用计数为1。
                //  accept返回后与connFd关联的文件表项的引用计数也为1。
                //  然后fork返回后，这两个描述符就在父进程与子进程间共享（也就是被复制），
                //  因此与这两个套接字相关联的文件表项各自的访问计数值均为2。
                //  这么一来，当父进程关闭connFd时，
                //  它只是把相应的引用计数值从2减为1。
                //  该套接字真正的清理和资源释放要等到其引用计数值到达0时才发生。
                //  这会在稍后子进程也关闭connFd时发生。
                //
                //  因此当父进程关闭connFd的时候它只是把这个connfd的访问计数值减了1而已，
                //  由于访问计数值还 > 0（因为还有客户端的connFd连着呢），
                //  所以它并没有断开和客户端的连接。
                //
                //
                //  因此在fork之后socketFd和connFd的引用计数均为2,父子进程均有一份
                //  在父进程中, 需要关闭客户端连接套接字的文件描述符号connfd -= 1
                //  在子进程中, 需要关闭服务器的文件socketFd,
                //              因为他不需要监听客户端的连接  socketFd -= 1
                //              然后处理完毕后需要关闭自己的客户端套接字connFd -= 1
                //
                //  这样在客户端处理完信息后, connFd == 0将被完全关闭
                //                            socketFd仅在父亲进程中仍被打开


                close(socketFd);

                printf("分叉了一个新的进程%d用于处理客户端的连接信息\n", getpid());
                printf("fork a new process %d to deal with the client\n", getpid());

                RaiseClientRequest(connFd, clientAddr);

                close(connFd);
                exit(0);
                // 子进程结束，会成为僵尸进程,所以注册SIGCHLD来让父亲进程处理僵尸进程的遗留数据
                //  signal(SIGCHLD, SignalChild);                //子进程退出的信号处理
            }
            else if(childPid < 0)               //  FROK出错返回-1
            {
                printf("fork error: %s\n", strerror(errno));
                close(connFd);
            }
        }
    }

    close(socketFd);
}





    /* 服务器接收从客户端传送来的文件  */
void
TcpServerPullFile(
            int         connFd,                     /*  服务器与客户端通讯的套接字文件  */
            struct      sockaddr_in clientAddr,     /*  与之通信的客户端的信息  */
            char        *fileServerRoot)            /*  上传文件的存储路径  */
{
    char    buffer[BUFFER_SIZE];
    char    filename[MAX_FILENAME_SIZE];
    char    fileServerPath[MAX_FILENAME_SIZE]/* = fileServerRoot*/;
    // 定义文件流
    FILE    *stream;

    int     count;              /*  发送文件名的字节数目  */
    int     dataLength;         /*  接收到的数据大小  */
    int     writeLength;        /* 实际写入的数据大小  */
    int     flag = 0;

    bzero(buffer, BUFFER_SIZE);
    /*
     *  向客户端提示输入文件路径提示...
     *
     * strcpy(buffer, "请输入要传输的文件的完整路径：");
    strcat(buffer, "\n");

    send(new_server_socket, buffer, BUFFER_SIZE, 0);
    bzero(buffer, BUFFER_SIZE);
    */

    /*  首先获取客户端发送过来的文件名  */
    count = recv(connFd, buffer, BUFFER_SIZE, 0);

    if(count < 0)
    {
        perror("获取文件名失败...\n");
        exit(1);
    }
    else
    {

        strncpy(filename, buffer, strlen(buffer) > MAX_FILENAME_SIZE ? MAX_FILENAME_SIZE : strlen(buffer));
        strcpy(fileServerPath, fileServerRoot);
        strcat(fileServerPath, filename);
        printf("\n获取客户端发送过来的文件名成功...\n");
        printf("文件名[%s]\n", filename);
        printf("文件存储路径[%s]\n\n", fileServerPath);
    }

    //  服务器接受数据, 首先打开一个文件流
    if((stream = fopen(fileServerPath, "w")) == NULL)
    {
        perror("file open error...\n");
        exit(1);
    }
    else
    {
        bzero(buffer,BUFFER_SIZE);
    }

    printf("正在接收来自%s的文件....\n",inet_ntoa(clientAddr.sin_addr));

    dataLength = 0;


    /*  先将数据接受到缓冲区buffer中，再写入到新建的文件中  */
    while((dataLength = recv(connFd, buffer, BUFFER_SIZE, 0)) > 0)
    {

        flag++;

        if(flag == 1)
        {
            printf("正在接收来自%s的文件....\n", inet_ntoa(clientAddr.sin_addr));
        }

        if(dataLength < 0)
        {
            printf("接收错误i\n");
            exit(1);
        }

        /*  向文件中写入数据  */
        writeLength = fwrite(buffer, sizeof(char), dataLength, stream);

        if(writeLength != dataLength)
        {
             printf("file write failed\n");
             exit(1);
        }
        bzero(buffer,BUFFER_SIZE);
    }

    if(flag > 0)
    {
        printf("%s的文件传送完毕\n", inet_ntoa(clientAddr.sin_addr));
    }
    if(flag==0)
    {
        printf("%s的文件传输失败\n", inet_ntoa(clientAddr.sin_addr));
    }

    fclose(stream);
    //rename("data",inet_ntoa(clientAddr.sin_addr));
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

}



/*  服务器将文件发送到客户端
 *
 *  当用户选择了下载文件后，服务器将执行此操作
 *
 *  */
void TcpServerPushFile(
                    int         connFd,                  /*  服务器与客户端通讯的套接字文件  */
                    struct      sockaddr_in clientAddr,  /*  与之通信的客户端的信息  */
                    char        *filePath)              /*  带发送至客户端的文件路径  */
{
    //send file imformation

    char    buff[BUFFER_SIZE];
    char    filename[MAX_FILENAME_SIZE];
    int     count;
    FILE    *stream;


    /* 先将文件名发送给客户端
     * 2015-4-13 21:38 Modify
     * 发送文件名时只需要发送filePath最后的文件名filename就可以了
     * */
    bzero(buff, BUFFER_SIZE);
    strcpy(filename, strrchr(filePath, '/') + 1);
    strncpy(buff, filename, strlen(filename) > MAX_FILENAME_SIZE ? MAX_FILENAME_SIZE : strlen(filename));
    count = send(connFd, buff, BUFFER_SIZE, 0);
    printf("服务器待发送的文件名[%s]..\n", filename);

    if(count < 0)
    {
        perror("Send file information");
        exit(1);
    }

    /*  服务器开始读取并且发送文件 ： */
    if((stream = fopen(filePath, "rb")) == NULL)
    {
        printf("%s not found!\n", filePath);
    }
    printf("服务器打开文件成功...\n");
    printf("正在向客户端发送文件...\n");
    bzero(buff, BUFFER_SIZE);

    int fileBlockLength = 0;
    while((fileBlockLength = fread(buff, sizeof(char), BUFFER_SIZE, stream)) > 0)
    {
        printf("读取了:%d个数据...\n",fileBlockLength);
        if((count =send(connFd, buff, fileBlockLength, 0)) < 0)
        {
            perror("Send file error...\n");
            perror("向客户端发送文件失败...\n");
            exit(1);
        }

        bzero(buff,BUFFER_SIZE);
    }

    fclose(stream);
    printf("服务器发送文件成功\n");

}

/// 处理客户端的请求信息
void RaiseClientRequest(
        int connFd,     /*  客户端的连接套接字描述符, 用于发送和接收数据  */
        struct sockaddr_in  clientAddr) /*  客户端的信息, 用于显示一些客户端的信息  */
{

    printf("\n\n\n下面将依次测试  接收数据  发送数据  存储文件  推送文件\n\n\n");

    int count;
    char buffer[BUFFER_SIZE];

    //  首先测试接收客户端发送来的数据
    printf("===========recv data===========\n");
    bzero(buffer, BUFFER_SIZE);
    if((count = recv(connFd, buffer, BUFFER_SIZE, 0)) < 0)
    {
        printf("recv data error from %s error, errno = %d...\n", inet_ntoa(clientAddr.sin_addr), errno);
        printf("接收来自 %s 的数据错误, 错误码errno = %d....\n", inet_ntoa(clientAddr.sin_addr), errno);
    }
    else
    {
        printf("recv %d data : %s\n", count, buffer);
        printf("接收%d个数据 : %s\n", count, buffer);
    }
    printf("===========recv data===========\n\n\n");


    //  接着测试向客户端发送反馈数据
    printf("===========send data===========\n");
    bzero(buffer, BUFFER_SIZE);
    strcpy(buffer, "I am fine !");
    if((count = send(connFd, buffer, strlen(buffer) + 1, 0)) < 0)
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


    //  首先测试接收客户端发送来的数据
    printf("===========pull file============\n");
    TcpServerPullFile(connFd, clientAddr, "./sdata/"); /*  将客户端发送来的文件存储在./sdata目录下　　*/
    printf("===========pull file============\n");

    //  首先测试接收客户端发送来的数据
    //printf("===========pull file============\n");
    //TcpServerPushFile(connFd, clientAddr, "./sdata/spush"); /*  将客户端发送来的文件存储在./sdata目录下　　*/
    //printf("===========pull file============\n");


}

//  处理子进程退出的信号处理函数
void SignalChild(int signo)         //父进程对子进程结束的信号处理
{
    pid_t       pid;
    int         stat;
    //  SIGCHLD 20,17,18 B 子进程结束信号
    printf("get a signal %d\n", signo);

    while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
    {
        printf("child %d terminated\n", pid);
        printf("子进程%d终止\n", pid);
    }

    return ;
}



