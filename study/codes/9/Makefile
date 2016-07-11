ROOT=../
CC:=gcc
CXX:=g++

target:=9-1use_select 9-3mtlt 9-4oneshot		\
		9-5unblockconnect 9-6mytalk_client		\
		9-7mytalk_server  9-8multi_port    	


all:$(target)

9-1use_select:9-1use_select.o
	$(CXX) $^ -o $@ $(LDFLAGS) -lpthread
	@echo "处理高并发的通信服务器(开辟一个线程池来处理accept)..."

9-3mtlt : 9-3mtlt.o
	$(CXX) $^ -o $@ $(LDFLAGS) -lpthread
	@echo "LT电平触发和ET边沿触发模式..."

9-4oneshot : 9-4oneshot.o
	$(CXX) $^ -o $@ $(LDFLAGS) -lpthread
	@echo "使用EPOLLONESHOT事件..."

9-5unblockconnect : 9-5unblockconnect.o
	$(CXX) $^ -o $@ $(LDFLAGS) -lpthread
	@echo "简单的一次通信客户端..."
	
9-6mytalk_client : 9-6mytalk_client.o 
	$(CXX) $^ -o $@ $(LDFLAGS) -lpthread
	@echo "简单的一次通信客户端..."

9-7mytalk_server  : 9-7mytalk_server.o  
	$(CXX) $^ -o $@ $(LDFLAGS) -lpthread
	@echo "带线程锁的模拟高并发的客户端..."

9-8multi_port : 9-8multi_port.o
	$(CXX) $^ -o $@ $(LDFLAGS) -lpthread
	@echo "带线程锁的模拟高并发的客户端..."

%.o : %.c
	$(CC) -c $^ -o $@  $(CFLAGS) $(DEFINES)

%.o : %.cpp
	$(CXX) -c $^ -o $@  $(CFLAGS) $(DEFINES)

clean :
	rm -rf *.o
	rm -rf $(target)

