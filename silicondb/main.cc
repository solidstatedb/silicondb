#include <iostream>
#include <thread>

#include <silicondb/silicondb.h>

int main() {
	std::thread my_thread(say_hello);
	my_thread.join();

	return 0;
}
