ROOT=../
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

target:=TcpServer TcpClient



all:$(target)

TcpServer: TcpServer.o
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "迭代型服务器..."

TcpClient : TcpClient.o
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "迭代型客户端..."


%.o : %.c
	$(CC) -c $^ -o $@  $(CFLAGS) $(DEFINES)

clean :
	rm *.o
	rm $(target)
