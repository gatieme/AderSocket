ROOT=../
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

target:=TcpServer TcpClient



all:$(target)

TcpServer: TcpServer.o
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "使用fork的b通信服务器..."

TcpClient : TcpClient.o
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "简单通信客户端..."

%.o : %.c
	$(CC) -c $^ -o $@  $(CFLAGS) $(DEFINES)

clean :
	rm -rf *.o
	rm -rf $(target)
