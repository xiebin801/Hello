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
bool addItem(vector<A1>& vec, int id, string str)
{
	A1 a(id);
	A1& theA = a;
	//shared_ptr<A1> pA = nullptr;
	A1* pA = nullptr;
	bool hasFound = false;

	auto itr = find(vec.begin(), vec.end(), A1(id));
	if (itr != vec.end())
	{
		pA = &(*itr);
		//pA = make_shared<A1>(*itr);	// Does NOT work, point to another instance.
		//pA = shared_ptr<A1>(&(*itr), [](A1 const*) {}); // works!
		hasFound = true;
		//theA = *itr; // Does NOT work, still reference to a
	}
	if (!hasFound)
	{
		pA = new A1(id);
		//pA = make_shared<A1>(A1(id));
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
int testItrPtr()
{
	A1 a(1), b(2);
	A1& r = a;
	r.id = 11;
	r = b;	// still point to a, but not b
	r.id = 21;

	vector<A1> list;
	addItem(list, 1, "CHN");
	addItem(list, 1, "CHT");
	addItem(list, 1, "ENG");
	addItem(list, 2, "DUT");
	addItem(list, 1, "CHS");

	return 0;
}

#include <atomic>
#include <thread>
std::atomic_int x, y;
int r1, r2;
void writeX() {
	x.store(1);
	r1 = y.load();
}
void writeY() {
	y.store(1);
	r2 = x.load();
}
int testAtomic()
{
	x = 0;
	y = 0;
	std::thread a(writeX);
	std::thread b(writeY);
	a.join();
	b.join();
	std::cout << r1 << r2 << std::endl;

	return 0;
}

int main()
{
	testItrPtr();
	testAtomic();
}
