/*************************************************************************
    > File Name: TcpServer.c
    > Author: GatieMe
    > Mail: gatieme@163.com
    > Created Time: 2016年01月31日 星期日 15时02分20秒
 ************************************************************************/

#include "tcp.h"



TcpServer* CreateTcpServer(char *ip, int port)
{
    /**********************************************************
     *
     *  创建并初始化套接字
     *
     **********************************************************/
	TcpServer *server = (TcpServer *)malloc(sizeof(TcpServer));

    /*
     *  初始化一下套接字信息
     *
     *  struct sockaddr_in  m_serverAddr;   服务器套接字地址
     *  struct sockaddr_in  m_clientAddr;   客户端套接字地址
     *  int                 m_socketFd; 	套接字信息
     *
     */

	InitTcpServer(server, ip, port);

    return server;
}


void InitTcpServer(TcpServer *server, int port)
{
    /**********************************************************
     *
     *  初始化套接字
     *
     **********************************************************/
	struct sockaddr_in *serverAddr = &(server->serverAddr);
    bzero(serverAddr, sizeof(struct sockaddr_in)); /*  全部置零  */

    serverAddr->sin_family       =   AF_INET;   		/*  internet协议族         */
    serverAddr->sin_addr.s_addr  =   inet_addr(INADDR_ANY);		/*  设置所连接服务器的IP   */
    serverAddr->sin_port         =   htons(port);		/*  设置连接的服务器端口   */

    /*  开始创建套接字                        */
    /*  SOCK_STREAM 面向连接的套接字，即TCP   */
    server->socketFd       		 =   socket(AF_INET, SOCK_STREAM, 0);
}



int main(int argc, char *argv[])
{
    int serverPort = -1;
    if(argc > 1)                       /*  参数过多的时候，提示用户                */
    {
        printf("You have given to much parameters...\n");
        printf("Yous should give the PORT  after %s\n without any other parametes...\n", (char *)argv[0]);
    }
    else if(argc == 1)                  /*  只有一个参数，则默认使用localhost(127.0.0.1)  */
    {
        strcpy(serverPort, 6666);
    }
    else
    {
        strcpy(serverPort, atoi(argv[1]));
    }

    TcpServer *server = CreateTcpServer(serverPort);
}
