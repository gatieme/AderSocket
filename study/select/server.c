//server.c
  #include <sys/types.h>
  #include <ctype.h>
  #include <strings.h>
  #include <unistd.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <netdb.h>
  #include <arpa/inet.h>
 #include <ctype.h>
 #include <errno.h>
 #include <sys/time.h>
 #include <stdio.h>
 #include <string.h>
 #include <sys/select.h>
 #include <stdlib.h>

 #define LISTEN_QUEUE_NUM 5
 #define BUFFER_SIZE 256
 #define ECHO_PORT 2029

 int main(int argc, char **argv)
 {
     struct sockaddr_in servaddr, remote;
     int request_sock, new_sock;
     int nfound, fd, maxfd, bytesread;
     uint32_t  addrlen;
     fd_set rset, set;
     struct timeval timeout;
     char buf[BUFFER_SIZE];

     if ((request_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
         perror("socket");
         return -1;
     }
     memset(&servaddr, 0, sizeof(servaddr));
     servaddr.sin_family = AF_INET;
     servaddr.sin_addr.s_addr = INADDR_ANY;
     servaddr.sin_port = htons((uint16_t)ECHO_PORT);

     if (bind(request_sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
     {
         perror("bind");
         return -1;
     }
     if (listen(request_sock, LISTEN_QUEUE_NUM) < 0)
     {
         perror("listen");
         return -1;
     }

     FD_ZERO(&set);
     FD_SET(request_sock, &set);
     maxfd = request_sock;
     while(1) {
         rset = set;
         timeout.tv_sec = 0;
         timeout.tv_usec = 500000;
         if((nfound = select(maxfd + 1, &rset, (fd_set *)0, (fd_set *)0, &timeout)) < 0)
         {
             perror("select");
         return -1;
         }
        else if (nfound == 0)
        {
            printf("."); fflush(stdout);
            continue;
        }
        printf("%d, nfound = %d", __LINE__, nfound);
        if (FD_ISSET(request_sock, &rset))
        {
            addrlen = sizeof(remote);
            if ((new_sock = accept(request_sock, (struct sockaddr *)&remote, &addrlen)) < 0)
            {
                perror("accept");
                return -1;
            }
            printf("connection from host %s, port %d, socket %d\r\n",
                    inet_ntoa(remote.sin_addr), ntohs(remote.sin_port),
                    new_sock);
            FD_SET(new_sock, &set);
            if (new_sock > maxfd)
                maxfd = new_sock;
            FD_CLR(request_sock, &rset);
            nfound --;
        }

        for (fd=0; fd <= maxfd && nfound > 0; fd++)
        {
            if (FD_ISSET(fd, &rset))
            {
                nfound --;
                if ((bytesread = read(fd, buf, sizeof buf - 1))<0)
                {
                    perror("read");
                }
                if (bytesread == 0)
                {
                    fprintf(stderr, "server: end of file on %d\r\n",fd);
                    FD_CLR(fd, &set);
                    close(fd);
                    continue;
                }
                buf[bytesread] = 0;
                printf("%s: %d bytes from %d: %s\n", argv[0], bytesread, fd, buf);
                if (write(fd, buf, bytesread) < 0)
                {
                    perror("echo");
                    FD_CLR(fd, &set);
                    close(fd);
                }
            }
        }
    }
    return 0;
}
