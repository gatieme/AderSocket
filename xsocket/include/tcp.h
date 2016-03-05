#ifndef  TCP_H_INCLUDE
#define  TCP_H_INCLUDE


#include "config.h"






/***********
 *  客户端信息
 *  SCloudTcpClient
 *
 **********/
#define MAX_FILENAME_SIZE   256
#define TCP_SERVER_PORT     6666    /*  服务器端口信息*/
#define BUFFER_SIZE         4096
#define LISTEN_QUEUE        20

#define IP_SIZE             20

typedef struct TcpClient
{
    /*  套接字信息
     *  这些属性是一个客户端所必须存储的信息
     *  */
    struct sockaddr_in  m_serverAddr;           /*  服务器套接字地址    */
    struct sockaddr_in  m_clientAddr;           /*  客户端套接字地址    */
    int                 m_socketFd;             /*  套接字信息          */

    /*  扩展信息
     *  这些属性是我们为了方便使用所扩充的数据成员
     *  */
    char                m_serverIp[IP_SIZE];    /*  服务器的IP地址
    此数据成员其实可以使用inet_ntoa(m_serverAddr.sin_addr.s_addr)所替代
    但是为了方便使用我们还是使用此字段来标识服务器IP地址                */
}TcpClient;

/*  按照user的信息初始化客户端  */
TcpClient* CreateTcpClient(char *tcpServerIp);

/*  销毁客户端的信息  */
void DestroyTcpClient(TcpClient *tcpClient);

/*  客户端将文件上传到服务器上 */
void TcpClientPushFile(TcpClient *client, char *filePath);


/***********
 *  服务器信息
 *  SCloudTcpServer
 *
 **********/
typedef struct TcpServer
{
    /*  套接字信息  */
    struct sockaddr_in  m_serverAddr;       	/*  服务器套接字地址  */
    struct sockaddr_in  m_clientAddr;       	/*  客户端套接字地址  */
    int                 m_socketFd; 		/*  套接字信息  */

}TcpServer;

/*  创建一个服务器  */
TcpServer* CreateTcpServer( );

/*  销毁服务器  */
void DestroyTcpServer(TcpServer *tcpServer);

/*  服务器运行函数  */
void TcpServerRun(TcpServer *server);







#endif      // #define __SCLOUD_H__
