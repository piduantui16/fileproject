all: ser

ser: ser.o socket.o thread.o MYSQL.o
	g++ -o ser  ser.o socket.o thread.o MYSQL.o -lpthread -L/usr/lib/mysql -lmysqlclient

ser.o: ser.cpp
	g++ -c ser.cpp

socket.o: socket.cpp 
	g++ -c socket.cpp 

thread.o: thread.cpp
	g++ -c thread.cpp

MYSQL.o:MYSQL.cpp
	g++ -c MYSQL.cpp

clean:
	rm -rf *.o ser
