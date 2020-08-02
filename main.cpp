#include "cthread_pool.h"
#include <chrono>
#include <thread>
#include <iostream>
#include <string>
#include <stdlib.h>


void hello() {
	std::this_thread::sleep_for(std::chrono::seconds(5));
	std::cout << std::this_thread::get_id() << " Hello task." << std::endl;
}

int power(int x, int y) {
	return x * y;
}

void prints(std::string str, std::string begin, int cnt) {
	std::cout << begin << std::endl;
	for (int i = 0; i < cnt; i++) {
		std::cout << str << ' ';
	}
	std::cout << std::endl;
}


int main() {
	std::string cc("oubu");
	std::string s("str print func:");
	WplThreadingPool task_pool(4);
	auto t1 = task_pool.add_task(hello);
	auto t2 = task_pool.add_task(power, 3, 5);
	auto t3 = task_pool.add_task(prints, cc, s, 3);
	std::this_thread::sleep_for(std::chrono::microseconds(800));
	std::cout << "Main thread: " << std::this_thread::get_id() << std::endl;
	std::cout << task_pool.freeNums() << std::endl;
	std::cout << t2.get() << std::endl;
	std::cout << "hello func will be completed in 5s" << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(5));
	std::cout << "Completed" << std::endl;
//	system("pause");
}
