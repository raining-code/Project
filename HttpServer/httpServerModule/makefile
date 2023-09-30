a.out:main.cc ../logModule/log.cc ../socketModule/tcpServer.cc httpServer.cc
	g++ $^ -o $@ -std=c++11 -lpthread
.PHONY:clean
clean:
	rm -rf a.out