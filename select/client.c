//client.c
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <sys/time.h>
  #include <netinet/in.h>
  #include <errno.h>
  #include <ctype.h>
  #include <netdb.h>
  #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <sys/select.h>


 #define RET_OK   0
 #define RET_ERR -1
 #define LISTEN_QUEUE_NUM 5
 #define BUFFER_SIZE 256
 #define ECHO_PORT 2029

 int main(int argc, char **argv)
 {
     int sock, maxfd = 0;
     struct sockaddr_in servaddr;
     struct hostent *server;
     fd_set rset, set;
     int nfound, bytesread;
     char buf[BUFFER_SIZE];

     if (argc < 2)
     {
         fprintf(stderr,"usage %s hostname\n", argv[0]);
         return RET_ERR;
     }
     if((server = gethostbyname(argv[1])) == NULL)
     {
         herror("gethostbyname. ");
         return RET_ERR;
     }
     if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
     {
         perror("socket");
         return -1;
     }
     memset(&servaddr, 0, sizeof(servaddr));
     servaddr.sin_family = AF_INET;
     servaddr.sin_addr.s_addr = *(uint32_t *)server->h_addr;
     servaddr.sin_port = htons((uint16_t)ECHO_PORT);
     if (connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
     {
         perror("connect");
         return -1;
     }
     maxfd = fileno(stdin);
     FD_ZERO(&set);
     FD_SET(sock, &set);
     FD_SET(maxfd, &set);
     maxfd = (maxfd > sock ? maxfd : sock) + 1;
     while(1)
     {
         rset = set;
         if ((nfound = select(maxfd, &rset, (fd_set *)0, (fd_set *)0, NULL)) < 0)
         {
             if (errno == EINTR) {
                 fprintf(stderr, "interrupted system call\n");
                 continue;
             }
             perror("select");
             exit(1);
         }
         if (FD_ISSET(fileno(stdin), &rset)) {
             if (fgets(buf, sizeof(buf), stdin) == NULL) {
                 if (ferror(stdin)) {
                     perror("stdin");
                     return -1;
                 }
                 return 0;
             }
             if (write(sock, buf, strlen(buf)) < 0)
             {
                 perror("write");
                 return -1;
             }
         }
         if (FD_ISSET(sock,&rset)) {
             if((bytesread = read(sock, buf, sizeof buf)) < 0)
             {
                 perror("read");
                 exit(1);
             }
             else
             if(bytesread == 0)
             {
                 fprintf(stderr, "server disconnect\n");
                 exit(0);
             }
             buf[bytesread] = 0;
             printf("%s\n",buf);
        }
    }
    return 0;
}
