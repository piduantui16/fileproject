项目组成：客户端，服务器
实现功能：用户登录，注册，上传下载文件，校验，秒传，断点续传。
进程间采用自定义协议通信
采用MYSQL记录用户信息。
mysql.cpp主要完成数据库层的登录，注册，更新信息，校验是否存在文件
client.cpp主要完成客户端、服务器之间的链接。实现了收发文件的功能。以及对一些操作的屏蔽。如：ps使其不可查看系统中的进程。
thread.cpp主要完成文件收发。
