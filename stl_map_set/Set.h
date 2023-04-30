#pragma once
#include "RBTree-new.h"
namespace nb
{
	template<class K>
	class set
	{
		struct SetKeyOfT
		{
			const K& operator()(const K& key)
			{
				return key;
			}
		};
	public:
	public:
		// 要使用关键字 typename 显式地指明iterator是一个类型
		// 否则编译器会误认为是一个静态成员变量而产生编译错误
		typedef typename RB::RBTree<K, K, SetKeyOfT>::const_iterator iterator;
		typedef typename RB::RBTree<K, K, SetKeyOfT>::const_iterator const_iterator;
		// 实现set的迭代器
		iterator begin() const
		{
			return _t.begin();
		}
		iterator end() const
		{
			return _t.end();
		}

		pair<iterator, bool> Insert(const K& key)
		{
			return _t.Insert(key);
			//pair<typename RB::RBTree<K, K, SetKeyOfT>::iterator, bool> ret = _t.Insert(key);
			//return pair<iterator, bool>(ret.first, ret.second);
		}

	private:
		RB::RBTree<K, K, SetKeyOfT> _t;
	};
	void TestSet1()
	{
		int a[] = { 8, 3, 1, 10, 6, 4, 7, 14, 13 };
		//int a[] = { 16, 3, 7, 11, 9, 26, 18, 14, 15 };
		//int a[] = { 4, 2, 6, 1, 3, 5, 15, 7, 16, 14 };
		//int a[] = { 13, 8, 17, 11, 14,7,6, 4,16,12,0,3,14,18,0 };

		set<int> s;
		// t.Insert(std::make_pair(1, 1));
		for (auto e : a)
		{
			s.Insert(e);
		}

		auto it = s.begin();
		while (it != s.end())
		{
			std::cout << (*it) << " ";
			//*it *= 10;
			++it;
		}
		std::cout << std::endl;
		for (auto& e : s)
		{
			std::cout << e << " ";
		}
		std::cout << std::endl;
	}
}