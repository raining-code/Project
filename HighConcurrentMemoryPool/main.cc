#include"Test.h"
int main() {
	/*Test::TestBigObjectFree();
	Test::TestConcurrentFree();
	Test::TestBigObjectFree();*/
	//Test::TestConcurrentAlloc();
	Test::BenchMarkMalloc(10, 10, 10000);
	Test::BenchMarkConcurrentAlloc(10, 10, 10000);
	//Test::TestRadixTree();
	return 0;
}