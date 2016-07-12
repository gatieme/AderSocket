//  代码清单9-7  poll实现的聊天室服务器程序.

#define _GNU_SOURCE 1

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
#include <poll.h>

#define DEFAULT_SERVER_PORT     6666

#define USER_LIMIT 5            /*  最大用户数量                */
#define BUFFER_SIZE 64          /*  读缓冲区的大小              */
#define FD_LIMIT 65535          /*  文件描述符数量限制          */


/*  客户数据  */
struct client_data
{
    sockaddr_in address;        /*  客户端socket地址            */
    char* write_buf;            /*  待写到客户端的数据的位置    */
    char buf[ BUFFER_SIZE ];    /*  从客户端读入的数据          */
};

/*  设置非阻塞模式  */
int setnonblocking( int fd )
{
    int old_option = fcntl( fd, F_GETFL );
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL, new_option );

    return old_option;
}

int main( int argc, char* argv[] )
{
    const char* ip = NULL;
    int port = DEFAULT_SERVER_PORT;

    if( argc > 3 )
    {
        printf( "usage: %s port_number ip_address\n", basename( argv[0] ) );
        return 1;
    }
    else if( argc == 2 )
    {
        port = atoi(argv[1]);
    }
    else if( argc == 3 )
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

    /*  创建users数组, 分配FD_LIMIT个client_data对象.
     *  可以预期: 每个可能的socket链接都可以获得一个这样的对象
     *  并且socket的值可以直接用用下标索引socket连接对应的clent_data对象
     *  这是将socket和客户端数据关联的简单而高效的方式
     */
    client_data* users = new client_data[FD_LIMIT];

    /*  尽管我们分配了足够多的client_data对象
     *  但为了提高poll的性能, 仍然有必要限制用户的数量
     */
    pollfd fds[USER_LIMIT+1];

    int user_counter = 0;
    for( int i = 1; i <= USER_LIMIT; ++i )
    {
        fds[i].fd = -1;
        fds[i].events = 0;
    }
    fds[0].fd = listenfd;
    fds[0].events = POLLIN | POLLERR;
    fds[0].revents = 0;

    while( 1 )
    {
        ret = poll( fds, user_counter+1, -1 );
        if ( ret < 0 )
        {
            printf( "poll failure\n" );
            break;
        }

        for( int i = 0; i < user_counter+1; ++i )
        {
            /*  检测监听套机字描述符上是否有可读事件  */
            if( ( fds[i].fd == listenfd ) && ( fds[i].revents & POLLIN ) )
            {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof( client_address );
                int connfd = accept( listenfd, ( struct sockaddr* )&client_address, &client_addrlength );
                if ( connfd < 0 )
                {
                    printf( "errno is: %d\n", errno );
                    continue;
                }

                /*  如果请求的客户端太多, 则关闭新的连接  */
                if( user_counter >= USER_LIMIT )
                {
                    const char* info = "too many users\n";
                    printf( "%s", info );
                    send( connfd, info, strlen( info ), 0 );
                    close( connfd );
                    continue;
                }

                /*  人数没有达到上限, 就将连接的客户端描述符加入到users中
                 *  对于新的连接, 同时修改users和fds数据
                 */

                /*  users[connfd]对应于新连接文件描述符connfd的客户端数据  */
                user_counter++;
                users[connfd].address = client_address;
                setnonblocking( connfd );

                /*  将poll文件描述信息添加到末尾  */
                fds[user_counter].fd = connfd;
                fds[user_counter].events = POLLIN | POLLRDHUP | POLLERR;
                fds[user_counter].revents = 0;

                printf( "comes a new user, now have %d users at %d\n", user_counter, connfd);
            }
            else if( fds[i].revents & POLLERR )     /*  异常事件  */
            {
                printf( "get an error from %d\n", fds[i].fd );
                char errors[ 100 ];
                memset( errors, '\0', 100 );
                socklen_t length = sizeof( errors );
                if( getsockopt( fds[i].fd, SOL_SOCKET, SO_ERROR, &errors, &length ) < 0 )
                {
                    printf( "get socket option failed\n" );
                }
                continue;
            }
            else if( fds[i].revents & POLLRDHUP )   /*  连接断开  */
            {
                /*  下面擦除断开的客户端的users数据和fds数据
                 *  由于users[connfd]存储了描述符connfd的客户端数据
                 *  目前描述符为fds[i].fd的客户端要断开连接
                 *
                 *  清除users的工作:
                 *  直接将待擦除的users[fds[i].fd]保存尾部的信息users[fds[user_count].fd]
                 *  然后user_count--;
                 *
                 *  清除fds的工作类似:
                 *  将末尾fds[user_count]的信息保存在fds[i];
                 *  然后i--;
                 *
                 *  而关闭描述符:
                 *  close( fds[i].fd );
                 *
                 *  */
                users[fds[i].fd] = users[fds[user_counter].fd];
                close( fds[i].fd );
                fds[i] = fds[user_counter];

                i--;
                user_counter--;
                printf( "a client left\n" );
            }
            else if( fds[i].revents & POLLIN )
            {
                int connfd = fds[i].fd;
                memset( users[connfd].buf, '\0', BUFFER_SIZE );
                ret = recv( connfd, users[connfd].buf, BUFFER_SIZE-1, 0 );
                printf( "get %d bytes of client data %s from %d\n", ret, users[connfd].buf, connfd );
                if( ret < 0 )
                {
                    /*  如果读取操作出错, 则关闭连接  */
                    if( errno != EAGAIN )
                    {
                        close( connfd );
                        users[fds[i].fd] = users[fds[user_counter].fd];
                        fds[i] = fds[user_counter];
                        i--;
                        user_counter--;
                    }
                }
                else if( ret == 0 )
                {
                    printf( "code should not come to here\n" );
                }
                else
                {
                    /*  如果接收到客户端的数据,
                     *  则通知其他socket连接准备写数据  */
                    for( int j = 1; j <= user_counter; ++j )
                    {
                        if( fds[j].fd == connfd )
                        {
                            continue;
                        }

                        fds[j].events |= ~POLLIN;
                        fds[j].events |= POLLOUT;
                        users[fds[j].fd].write_buf = users[connfd].buf;
                    }
                }
            }
            else if( fds[i].revents & POLLOUT )     /*  写事件  */
            {
                int connfd = fds[i].fd;
                if( ! users[connfd].write_buf )
                {
                    continue;
                }
                ret = send( connfd, users[connfd].write_buf, strlen( users[connfd].write_buf ), 0 );
                users[connfd].write_buf = NULL;

                /*  写完数据后需要重新注册fds[i]上的可读事件  */
                fds[i].events |= ~POLLOUT;
                fds[i].events |= POLLIN;
            }
        }
    }

    delete [] users;
    close( listenfd );
    return 0;
}
