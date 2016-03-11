ROOT=../
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

target:=NBAcceptServer NBAForkServer NBAThreadServer TcpClient SCClient SCMClient


all:$(target)


NBAcceptServer: NBAcceptServer.o
	$(CC) $^ -o $@ $(LDFLAGS) -lpthread
	echo "select模式下的单进程非阻塞accept服务器...\n"

NBAForkServer: NBAForkServer.o
	$(CC) $^ -o $@ $(LDFLAGS) -lpthread
	echo "select模式下的多进程非阻塞accept服务器...\n"

NBAThreadServer: NBAThreadServer.o
	$(CC) $^ -o $@ $(LDFLAGS) -lpthread
	echo "select模式下的多线程非阻塞accept服务器...\n"

TcpClient : TcpClient.o
	$(CC) $^ -o $@ $(LDFLAGS) -lpthread
	echo "简单的一次通信客户端...\n"


SCClient : SCClient.o
	$(CC) $^ -o $@ $(LDFLAGS) -lpthread
	echo "简单的模拟高并发的客户端...\n"

SCMClient : SCMClient.o
	$(CC) $^ -o $@ $(LDFLAGS) -lpthread
	echo "带线程锁的模拟高并发的客户端...\n"

%.o : %.c
	$(CC) -c $^ -o $@  $(CFLAGS) $(DEFINES)

clean :
	rm *.o
	rm $(target)

clean_test :
	rm server client
	rm server2 client2
test :
	gcc server.c -o server
	gcc -pedantic -Os client.c -o client -std=c99 -D_GNU_SOURCE

	gcc server2.c -o server2
	gcc -pedantic -Os client2.c -o client2 -std=c99 -D_GNU_SOURCE -lpthread
