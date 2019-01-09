// ModernCPP.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <algorithm>
#include <memory>
#include <vector>

using namespace std;

struct A1
{
	int id;
	vector<string> strings;
	
	A1(int idValue) : id(idValue) {}
	A1(int idValue, string str) : id(idValue), strings{ str } {}

	bool operator == (const A1& other)
	{
		return id == other.id;
	}
};
using A1UniquePtr = unique_ptr<A1, void(*)(A1 const*)>;
void A1DeleterNothing(A1 const* ins) { }
void A1DeleterDefault(A1 const* ins) 
{ 
	delete ins;
	//ins = nullptr;
}
bool addItem(vector<A1>& vec, int id, string str)
{
	A1 a(id);
	A1& theA = a;
	A1UniquePtr pA(nullptr, nullptr);
	//shared_ptr<A1> pA = nullptr;
	//A1* pA = nullptr;
	bool hasFound = false;

	auto itr = find(vec.begin(), vec.end(), A1(id));
	if (itr != vec.end())
	{
		//pA = A1UniquePtr(&(*itr), [](A1 const*) {});
		pA = A1UniquePtr(&(*itr), A1DeleterNothing);
		//pA = make_shared<A1>(*itr);	// Does NOT work, point to another instance.
		//pA = shared_ptr<A1>(&(*itr), [](A1 const*) {}); // works!
		//pA = &(*itr);
		hasFound = true;
		//theA = *itr; // Does NOT work, still reference to a
	}
	if (!hasFound)
	{
		//pA = A1UniquePtr(new A1(id), [](A1 const* ptr) {delete ptr; });
		pA = A1UniquePtr(new A1(id), A1DeleterDefault);	//default_delete<A1>()
		//pA = shared_ptr<A1>(new A1(id));
		//pA = new A1(id);
	}
	
	pA->strings.push_back(str);
	//theA.strings.push_back(str);
	if (!hasFound)
	{
		vec.push_back(*pA);
		//delete pA;
		//vec.push_back(theA);
	}

	return true;
}
int testRefrence()
{
	cout << "Starts to test Refrences" << endl;
	A1 a(1), b(2);
	A1& r = a;
	r.id = 11;
	r = b;	// still point to a, but not b
	r.id = 21;
	cout << "b.id = " << b.id << endl;	// 2

	A1& r2 = (b.id == 2) ? b : a;
	r2.id = 3;
	cout << "a.id = " << a.id << endl;
	cout << "b.id = " << b.id << endl;

	return 0;
}
int testItrPtr()
{
	cout << endl << "Starts to test Pointers" << endl;

	vector<A1> list;
	addItem(list, 1, "CHN");
	addItem(list, 1, "CHT");
	addItem(list, 1, "ENG");
	addItem(list, 2, "DUT");
	addItem(list, 1, "CHS");

	for_each (list.cbegin(), list.cend(), [&](A1 v)
	{
		cout << "v.id = " << v.id << ", v.strings = {" ;
		for (auto& s : v.strings) 
		{ 
			cout << s.c_str() << ", ";
		}
		cout << "}" << endl;
	});

	return 0;
}

#include <atomic>
#include <thread>
std::atomic_int x, y;
int r1, r2;
void writeX() 
{
	x.store(1);
	r1 = y.load();
}
void writeY() 
{
	y.store(1);
	r2 = x.load();
}
int testAtomic()
{
	cout << endl << "Starts to test Atomic" << endl;
	x = 0;
	y = 0;
	std::thread a(writeX);
	std::thread b(writeY);
	a.join();
	b.join();
	std::cout << r1 << r2 << std::endl;

	return 0;
}

#include <mutex>              // std::mutex, std::unique_lock
#include <condition_variable> // std::condition_variable
std::mutex mtx;
std::condition_variable cv;
bool ready = false;
void print_id(int id) 
{
	std::unique_lock<std::mutex> lck(mtx);
	while (!ready) cv.wait(lck);
	std::cout << "thread " << id << '\n';
}
void go() 
{
	std::unique_lock<std::mutex> lck(mtx);
	ready = true;
	cv.notify_all();
}
int testConditionVariable()
{
	cout << endl << "Starts to test ConditionVariable" << endl;
	std::thread threads[10];
	// spawn 10 threads:
	for (int i = 0; i < 10; ++i)
		threads[i] = std::thread(print_id, i);

	cout << "10 threads ready to race...\n";
	go();                       // go!

	for (auto& th : threads) th.join();

	return 0;
}

#include <future>         // std::async, std::future
#include <chrono>         // std::chrono::milliseconds
// a non-optimized way of checking for prime numbers:
bool is_prime(int x) 
{
	for (int i = 2; i <= x/2; ++i) if (x%i == 0) return false;
	return true;
}
int testFuture()
{
	cout << endl << "Starts to test Future" << endl;
	// call function asynchronously:
	std::future<bool> fut = std::async(is_prime, 444444443);

	// do something while waiting for function to set future:
	std::cout << "checking, please wait";
	std::chrono::milliseconds span(100);
	while (fut.wait_for(span) == std::future_status::timeout)
		std::cout << '.' << std::flush;

	bool x = fut.get();     // retrieve return value

	std::cout << "\n444444443 " << (x ? "is" : "is not") << " prime.\n";

	return 0;
}

void helloFunction() 
{
	cout << "function" << endl;
}
class HelloFunctionObject 
{
public:
	void operator()() const 
	{
		cout << "function object" << endl;
	}
};
int testThread()
{
	cout << endl << "Starts to test Thread" << endl;
	thread t1(helloFunction); // function
	HelloFunctionObject helloFunctionObject;
	thread t2(helloFunctionObject); // function object
	thread t3([] { cout << "lambda function"; }); // lambda function

	t1.join();
	t2.join();
	t3.join();
	
	return 0;
}




int main()
{
	testRefrence();
	testItrPtr();
	testAtomic();
	testConditionVariable();
	testFuture();
	testThread();

	return 0;
}
