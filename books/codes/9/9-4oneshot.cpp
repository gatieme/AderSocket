//  代码清单9-4 使用EPOLLONESHOT事件
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <pthread.h>

#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 1024

#define DEFAULT_SERVER_PORT 6666


typedef struct fds_pthread_args
{
   int epollfd;
   int sockfd;
}fds_pthread_args;

int setnonblocking( int fd )
{
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );
    return old_option;
}

/*  将fd上的EPOLLIN和EPOLLET事件注册到epollfd指示的epoll内核事件表中
 *  参数oneshot用来指定是否注册fd上的EPOLLONESHOT事件
 * */
void addfd( int epollfd, int fd, bool oneshot )
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    /*  对于注册了EPOLLONESHOT时间的文件描述符
     *  操作系统最多触发其上注册的一个可读, 可写或者异常事件
     *  且只触发一次  */
    if( oneshot )
    {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl( epollfd, EPOLL_CTL_ADD, fd, &event );
    setnonblocking( fd );
}

/*  重置fd上的事件, 这样操作后, 尽管fd上的EPOLLONESHOT事件被注册后,
 *  但是操作系统仍然会触发fd上的EPOLLIN事件
 *  且只触发一次
 * */
void reset_oneshot( int epollfd, int fd )
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    epoll_ctl( epollfd, EPOLL_CTL_MOD, fd, &event );
}


/*  工作线程  */
void* worker( void* args )
{
    fds_pthread_args *_args = (fds_pthread_args *)args;

    int sockfd = _args->sockfd;
    int epollfd = _args->epollfd;
    printf( "start new thread to receive data on fd: %d\n", sockfd );

    char buf[ BUFFER_SIZE ];
    memset( buf, '\0', BUFFER_SIZE );

    /*  循环读取sockfd上的数据, 直到遇见EAGAIN错误  */
    while( 1 )
    {
        int ret = recv( sockfd, buf, BUFFER_SIZE - 1, 0 );
        if( ret == 0 )
        {
            close( sockfd );
            printf( "foreiner closed the connection\n" );
            break;
        }
        else if( ret < 0 )
        {
            /*  首先我们看看recv的返回值：
             *  EAGAIN、EWOULDBLOCK、EINTR与非阻塞 长连接
             *  EWOULDBLOCK     用于非阻塞模式，不需要重新读或者写
             *  EINTR           指操作被中断唤醒，需要重新读/写
             *  在Linux环境下开发经常会碰到很多错误(设置errno)，
             *  其中EAGAIN是其中比较常见的一个错误(比如用在非阻塞操作中)
             *  从字面上来看, 是提示再试一次.
             *  这个错误经常出现在当应用程序进行一些非阻塞(non-blocking)操作
             *  (对文件或socket)的时候
             *  例如，以 O_NONBLOCK的标志打开文件/socket/FIFO,
             *  如果你连续做read操作而没有数据可读.
             *  此时程序不会阻塞起来等待数据准备就绪返回,
             *  read函数会返回一个错误EAGAIN，
             *  提示你的应用程序现在没有数据可读请稍后再试重新读数据,
             *  对非阻塞socket而言, EAGAIN不是一种错误。在VxWorks和Windows上,
             *  EAGAIN的名字叫做EWOULDBLOCK
             */
            if( errno == EAGAIN )
            {
                reset_oneshot( epollfd, sockfd );
                printf( "read later\n" );
                break;
            }
        }
        else
        {
            printf( "get content: %s\n", buf );
            /*  休眠5s, 模拟数据处理过程    */
            sleep( 5 );
        }
    }
    printf( "end thread receiving data on fd: %d\n", sockfd );

    return NULL;
}

int main( int argc, char* argv[] )
{
    int port = DEFAULT_SERVER_PORT;
    char *ip = NULL;

    if( argc > 3)
    {
        printf( "usage: %s port_number ip_address\n", basename( argv[0] ) );
        return 1;
    }
    else if( argc == 2 )
    {
        port = atoi(argv[1]);
    }
    else if(argc == 3)
    {
        port = atoi(argv[1]);
        ip = argv[2];
    }

    int ret = 0;
    struct sockaddr_in address;
    bzero( &address, sizeof( address ) );
    address.sin_family = AF_INET;
    if(ip != NULL)
    {
        inet_pton( AF_INET, ip, &address.sin_addr );
    }
    else
    {
        address.sin_addr.s_addr = INADDR_ANY;
    }
    address.sin_port = htons( port );

    int listenfd = socket( PF_INET, SOCK_STREAM, 0 );
    assert( listenfd >= 0 );

    ret = bind( listenfd, ( struct sockaddr* )&address, sizeof( address ) );
    assert( ret != -1 );

    ret = listen( listenfd, 5 );
    assert( ret != -1 );

    epoll_event events[ MAX_EVENT_NUMBER ];
    int epollfd = epoll_create( 5 );
    assert( epollfd != -1 );

    /*   注意, 监听套接字listen上不能注册EPOLLONESHOT事件,
     *   否则应用程序只能处理一个客户端连接
     *   因为由于EPOLLONESHOT被设置
     *   后续的客户端连接请求将不再触发listenfd的EPOLLIN事件
     */
    addfd( epollfd, listenfd, false );

    while( 1 )
    {
        int ret = epoll_wait( epollfd, events, MAX_EVENT_NUMBER, -1 );
        if ( ret < 0 )
        {
            printf( "epoll failure\n" );
            break;
        }

        for ( int i = 0; i < ret; i++ )
        {
            int sockfd = events[i].data.fd;
            if ( sockfd == listenfd )
            {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof( client_address );
                int connfd = accept( listenfd, ( struct sockaddr* )&client_address, &client_addrlength );

                /*  对每个非监听文件描述符都注册EPOLLONEHOT事件  */
                addfd( epollfd, connfd, true );
            }
            else if ( events[i].events & EPOLLIN )
            {
                pthread_t           thread;
                fds_pthread_args    fds_for_new_worker;

                fds_for_new_worker.epollfd = epollfd;
                fds_for_new_worker.sockfd = sockfd;

                /*  新启动一个工作县城为sockfd服务  */
                pthread_create( &thread, NULL, worker, ( void* )&fds_for_new_worker );
            }
            else
            {
                printf( "something else happened \n" );
            }
        }
    }

    close( listenfd );
    return 0;
}
