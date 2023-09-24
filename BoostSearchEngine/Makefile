ALL:parser server
parser:parser.cc
	g++ $^ -o $@ -std=c++17 -lboost_system -lboost_filesystem -static
server:server.cc index.cc
	g++ $^ -o $@ -std=c++17 -ljsoncpp 
.PHONY:clean
clean:
	rm -rf parser server