#pragma once

#include "HashTable.h"

namespace nb
{
	template<class K, class Hash = HashFunc<K>>
	class unordered_set
	{
		struct SetKeyOfT
		{
			const K& operator()(const K& key)
			{
				return key;
			}
		};

	public:
		// 注意typename的使用
		typedef typename bucketHash::HashTable<K, K, Hash, SetKeyOfT>::const_iterator iterator;
		typedef typename bucketHash::HashTable<K, K, Hash, SetKeyOfT>::const_iterator const_iterator;
		iterator begin() const
		{
			return _ht.begin();
		}
		iterator end() const
		{
			return _ht.end();
		}
		// uordered_set的iterator和const_iterator是同一类型，函数仅返回值不同不能重载
		//const_iterator begin() const
		//{
		//	return _ht.begin();
		//}
		//const_iterator end() const
		//{
		//	return _ht.end();
		//}

		std::pair<iterator, bool> insert(const K& key)
		{
			return _ht.Insert(key);
		}

		iterator find(const K& key) const
		{
			return _ht.Find(key);
		}

		bool erase(const K& key)
		{
			return _ht.Erase(key);
		}

	private:
		bucketHash::HashTable <K, K, Hash, SetKeyOfT> _ht;
	};

	void test_unordered_set()
	{
		nb::unordered_set<int> us;
		us.insert(13);
		us.insert(3);
		us.insert(23);
		us.insert(5);
		us.insert(5);
		us.insert(6);
		us.insert(15);
		us.insert(1024);
		us.insert(22);

		nb::unordered_set<int>::iterator it = us.begin();
		while (it != us.end())
		{
			std::cout << *it << " ";
			++it;
		}
		std::cout << std::endl;

		for (auto e : us)
		{
			std::cout << e << " ";
		}
		std::cout << std::endl;

		it = us.find(13);
		std::cout << *it << std::endl;
		us.erase(13);
		it = us.find(13);
	}
	void func(const unordered_set<int>& us)
	{
		nb::unordered_set<int>::const_iterator it = us.begin();
		while (it != us.end())
		{
			std::cout << *it << " ";
			++it;
		}
		it = us.find(10);
		std::cout << std::endl;
	}
	void test_us_1()
	{
		nb::unordered_set<int> us;
		us.insert(10);
		func(us);
	}
	template<class K>
	struct SetKeyOfT
	{
		const K& operator()(const K& key)
		{
			return key;
		}
	};
	void test_us_2()
	{
		// 测试普通迭代器转换为const迭代器
		bucketHash::const_HTIterator<int, int, HashFunc<int>, SetKeyOfT<int>> cit(new bucketHash::HashNode<int>(10), nullptr);
		bucketHash::_HTIterator<int, int, HashFunc<int>, SetKeyOfT<int>> it(new bucketHash::HashNode<int>(20), nullptr);
		bucketHash::const_HTIterator<int, int, HashFunc<int>, SetKeyOfT<int>> cit2(it);

		std::cout << *cit << std::endl;
		std::cout << *it << std::endl;
		std::cout << *cit2 << std::endl;

		cit = it;
		std::cout << *cit << std::endl;
	}
};
