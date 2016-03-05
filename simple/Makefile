ROOT=../
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

target:=TcpClient TcpServer



all:$(target)


TcpClient : TcpClient.o
	$(CC) $^ -o $@ $(LDFLAGS)
	echo "迭代型客户端..."
TcpServer: TcpServer.o
	$(CC) $^ -o $@ $(LDFLAGS)
	echo "迭代型服务器..."

%.o : %.c
	$(CC) -c $^ -o $@  $(CFLAGS) $(DEFINES)

clean :
	rm *.o
	rm $(target)
