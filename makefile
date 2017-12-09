main:
	-rm -f client server
	g++ client.cpp -std=c++11 -pthread -o client -Wall -I/usr/local/pgsql/include -L/usr/local/pgsql/lib -lpq
	g++ -std=c++11 -pthread -o server server.cpp
