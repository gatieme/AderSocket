ROOT=../
CC:=gcc
CXX:=g++

target:=9-1use_select 9-3mtlt 9-4oneshot		\
		9-5unblockconnect 9-6mytalk_client		\
		9-7mytalk_server  9-8multi_port


all:$(target)

9-1use_select:9-1use_select.o
	$(CXX) $^ -o $@ $(LDFLAGS) -lpthread
	@echo "代码清单9-1 使用select同时接收普通数据和带外数据..."
	@echo ""


9-3mtlt : 9-3mtlt.o
	$(CXX) $^ -o $@ $(LDFLAGS) -lpthread
	@echo "代码清单9-3 epoll-LT电平触发和ET边沿触发模式..."
	@echo ""


9-4oneshot : 9-4oneshot.o
	$(CXX) $^ -o $@ $(LDFLAGS) -lpthread
	@echo "代码清单9-4 epoll使用EPOLLONESHOT事件..."
	@echo ""

9-5unblockconnect : 9-5unblockconnect.o
	$(CXX) $^ -o $@ $(LDFLAGS) -lpthread
	@echo "代码清单9-5 select非阻塞connect..."
	@echo ""


9-6mytalk_client : 9-6mytalk_client.o
	$(CXX) $^ -o $@ $(LDFLAGS) -lpthread
	@echo "代码清单9-6 poll实现的聊天室客户端程序..."
	@echo ""


9-7mytalk_server  : 9-7mytalk_server.o
	$(CXX) $^ -o $@ $(LDFLAGS) -lpthread
	@echo "代码清单9-7 poll实现的聊天室服务器程序..."
	@echo ""


9-8multi_port : 9-8multi_port.o
	$(CXX) $^ -o $@ $(LDFLAGS) -lpthread
	@echo "代码清单9-8 epoll同时处理TCP请求和UDP请求的回射服务器..."
	@echo ""


%.o : %.c
	$(CC) -c $^ -o $@  $(CFLAGS) $(DEFINES)

%.o : %.cpp
	$(CXX) -c $^ -o $@  $(CFLAGS) $(DEFINES)

clean :
	rm -rf *.o
	rm -rf $(target)

