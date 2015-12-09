# AderSocket
High Performance TCP/UDP Socket Component

# Directory
tcp_1

# Describe
最基本的TCP项目信息
仅包括TcpClient和TcpServer两个程序

客户端向服务器依次
发送数据How are you
接收服务器的反馈数据I am fine
上传文件

同样的服务器以及
接收客户端的数据How are you
向客户端发送反馈数据I am fine
接收客户端上传文件的请求,并进行存储


以后所有的程序都是基于此版本上进行的扩充

[注意]
由于此版本未使用任何Fork多进程/分叉或者Thread多线程的函数库
因此此版本服务器同时只能处理一个客户端请求
