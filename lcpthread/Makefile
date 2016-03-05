ROOT=../
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

target:=TcpClient TcpServer


all:$(target)


TcpClient : TcpClient.o
	$(CC) $^ -o $@ $(LDFLAGS) -lpthread
	echo "简单的一次通信客户端..."
TcpServer: TcpServer.o
	$(CC) $^ -o $@ $(LDFLAGS) -lpthread
	echo "简单的一次通信服务器..."

%.o : %.c
	$(CC) -c $^ -o $@  $(CFLAGS) $(DEFINES)

clean :
	rm *.o
	rm $(target)
