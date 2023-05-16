#pragma once

#include "HashTable.h"

namespace nb
{
	template<class K, class V, class Hash = HashFunc<K>>
	class unordered_map
	{
		struct MapKeyOfT
		{
			const K& operator()(const std::pair<const K, V>& kv)
			{
				return kv.first;
			}
		};

	public:
		typedef typename bucketHash::HashTable<K, std::pair<const K, V>, Hash, MapKeyOfT>::iterator iterator;
		typedef typename bucketHash::HashTable<K, std::pair<const K, V>, Hash, MapKeyOfT>::const_iterator const_iterator;

		iterator begin()
		{
			return _ht.begin();
		}

		iterator end()
		{
			return _ht.end();
		}

		// constµü´úÆ÷
		const_iterator begin() const
		{
			return _ht.begin();
		}
		const_iterator end() const
		{
			return _ht.end();
		}

		std::pair<iterator, bool> insert(const std::pair<K, V>& data)
		{
			return _ht.Insert(data);
		}

		V& operator[](const K& key)
		{
			std::pair<iterator, bool> ret = _ht.Insert(std::make_pair(key, V()));
			return ret.first->second;
		}

	private:
		bucketHash::HashTable < K, std::pair<const K, V>, Hash, MapKeyOfT> _ht;
	};

	void test_unordered_map()
	{
		std::string arr[] = { "apple", "banana", "cherry", "cherry" };

		unordered_map<std::string, int> countMap;
		for (auto& e : arr)
		{
			countMap[e]++;
		}

		for (const auto& kv : countMap)
		{
			std::cout << kv.first << ":" << kv.second << std::endl;
		}
	}

	void Func(const unordered_map<std::string, int>& countMap)
	{
		nb::unordered_map<std::string, int>::const_iterator it = countMap.begin();

		while (it != countMap.end())
		{
			std::cout << it->first << " " << it->second << std::endl;
			++it;
		}
	}
	void test_um()
	{
		//std::string arr[] = { "apple", "banana", "cherry", "cherry" };
		std::string arr[] = { "cherry" };

		unordered_map<std::string, int> countMap;
		for (auto& e : arr)
		{
			countMap[e]++;
		}

		nb::unordered_map<std::string, int>::const_iterator it = countMap.begin();

		while (it != countMap.end())
		{
			std::cout << it->first << " " << it->second << std::endl;
			++it;
		}
	}
};