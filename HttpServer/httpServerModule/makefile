a.out:main.cc ../logModule/log.cc ../socketModule/tcpServer.cc httpServer.cc endPoint.cc util.cc
	g++ $^ -o $@ -std=c++17 -lpthread
.PHONY:clean
clean:
	rm -rf a.out