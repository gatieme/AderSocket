ROOT=../
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

target:=TcpClient TcpServer SCClient SCMClient


all:$(target)

TcpServer: TcpServer.o
	$(CC) $^ -o $@ $(LDFLAGS) -lpthread
	echo "处理高并发的通信服务器(开辟一个线程池来处理accept)...\n"

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
