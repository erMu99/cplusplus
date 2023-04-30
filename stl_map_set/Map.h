#pragma once

#include "RBTree-new.h"

namespace nb
{
	template<class K, class V>
	class map
	{
		struct MapKeyOfT
		{
			const K& operator()(const pair<K, V>& kv)
			{
				return kv.first;
			}
		};
	public:
		// 要使用关键字 typename 显式地指明iterator是一个类型
		// 否则编译器会误认为是一个静态成员变量而产生编译错误
		typedef typename RB::RBTree<K, pair<const K, V>, MapKeyOfT>::iterator iterator;
		typedef typename RB::RBTree<K, pair<const K, V>, MapKeyOfT>::const_iterator const_iterator;
		// 实现map的迭代器
		iterator begin()
		{
			return _t.begin();
		}
		iterator end()
		{
			return _t.end();
		}
		const_iterator begin() const
		{
			return _t.begin();
		}
		const_iterator end() const
		{
			return _t.end();
		}
		//void Insert(const pair<K, V>& kv)
		//{
		//	_t.Insert(kv);
		//}

		pair<iterator, bool> Insert(const pair<K, V>& kv)
		{
			return _t.Insert(kv);
		}

		V& operator[](const K& key)
		{
			pair<iterator, bool> ret = Insert(make_pair(key, V()));
			return ret.first->second;
		}

	private:
		RB::RBTree<K, pair<const K, V>, MapKeyOfT> _t;
	};
	void TestMap1()
	{
		//int a[] = { 8, 3, 1, 10, 6, 4, 7, 14, 13 };
		////int a[] = { 16, 3, 7, 11, 9, 26, 18, 14, 15 };
		////int a[] = { 4, 2, 6, 1, 3, 5, 15, 7, 16, 14 };
		////int a[] = { 13, 8, 17, 11, 14,7,6, 4,16,12,0,3,14,18,0 };

		//map<int, int> mp;
		//// t.Insert(std::make_pair(1, 1));
		//for (auto e : a)
		//{
		//	mp.Insert(std::make_pair(e, e));
		//}

		//auto it = mp.begin();
		//while (it != mp.end())
		//{
		//	std::cout << it->first << " " << it->second << " ";
		//	++it;
		//	//it++;
		//}
		////auto it = mp.end();
		////while (it != mp.begin())
		////{
		////	std::cout << it->first << " " << it->second << " ";
		////	--it;
		////}

		//std::cout << std::endl;
		//for (auto& e : mp)
		//{
		//	std::cout << e.first << " " << e.second << " ";
		//}
		//std::cout << std::endl;

		nb::map<std::string, int> mp;
		std::string s[] = { "apple","banana","cherry" , "cherry" };

		for (auto& e : s)
		{
			++mp[e];
		}
		for (auto& e : mp)
		{
			std::cout << e.first << " " << e.second << std::endl;
		}
		std::cout << std::endl;
	}

	void TestMap2()
	{
		srand(time(0));
		const size_t N = 100000;
		// RBTree<int, int> t;
		map<int, int> t;
		for (size_t i = 0; i < N; ++i)
		{
			size_t x = rand();
			t.Insert({ x, x });
			//std::cout << "insert " << x << " : " << std::boolalpha << t.IsBalance() << std::endl;
			//Sleep(1000);
			//std::cout << std::boolalpha << t.IsBalance() << std::endl;
		}

		//t.Inorder();
		// std::cout << std::boolalpha << t.IsBalance() << std::endl;
	}
}