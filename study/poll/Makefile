ROOT=../
PLATFORM=$(shell $(ROOT)/systype.sh)
include $(ROOT)/Make.defines.$(PLATFORM)

target:=talk_server talk_client echo_server echo_client


all:$(target)

talk_server : talk_server.o
	$(CC) $^ -o $@ $(LDFLAGS) -lpthread
	@echo $@"==poll多路复用的聊天服务器程序..."
	@echo ""

talk_client : talk_client.o
	$(CC) $^ -o $@ $(LDFLAGS) -lpthread
	@echo $@"==poll多路复用的聊天客户端程序..."
	@echo ""

echo_server : echo_server.o
	$(CC) $^ -o $@ $(LDFLAGS) -lpthread
	@echo $@"==poll多路复用的回显服务器程序..."
	@echo ""

echo_client : echo_client.o
	$(CC) $^ -o $@ $(LDFLAGS) -lpthread
	@echo $@"==poll多路复用的回显客户端程序..."
	@echo ""

%.o : %.c
	$(CC) -c $^ -o $@  $(CFLAGS) $(DEFINES)

clean :
	rm -rf *.o
	rm -rf $(target)

