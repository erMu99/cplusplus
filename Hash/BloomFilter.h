#pragma once

#include <iostream>
#include <vector>
#include <bitset>
#include <string>
using namespace std;
namespace nb
{
	struct BKDRHash
	{
		size_t operator()(const string& key)
		{
			size_t hash = 0;
			for (auto ch : key)
			{
				hash *= 131;
				hash += ch;
			}
			return hash;
		}
	};

	struct APHash
	{
		size_t operator()(const string& key)
		{
			unsigned int hash = 0;
			int i = 0;

			for (auto ch : key)
			{
				if ((i & 1) == 0)
				{
					hash ^= ((hash << 7) ^ (ch) ^ (hash >> 3));
				}
				else
				{
					hash ^= (~((hash << 11) ^ (ch) ^ (hash >> 5)));
				}

				++i;
			}

			return hash;
		}
	};

	struct DJBHash
	{
		size_t operator()(const string& key)
		{
			unsigned int hash = 5381;

			for (auto ch : key)
			{
				hash += (hash << 5) + ch;
			}

			return hash;
		}
	};

	struct JSHash
	{
		size_t operator()(const string& s)
		{
			size_t hash = 1315423911;
			for (auto ch : s)
			{
				hash ^= ((hash << 5) + ch + (hash >> 2));
			}
			return hash;
		}
	};

	// ����N�����洢�����ݸ���
	// ƽ���洢һ��ֵ������X��λ
	template<size_t N,
		size_t X = 6,
		class K = string,
		class HashFunc1 = BKDRHash,
		class HashFunc2 = APHash,
		class HashFunc3 = DJBHash,
		class HashFunc4 = JSHash>
	class BloomFilter
	{
	public:
		void set(const K& key)
		{
			size_t hash1 = HashFunc1()(key) % (N * X);
			size_t hash2 = HashFunc2()(key) % (N * X);
			size_t hash3 = HashFunc3()(key) % (N * X);
			size_t hash4 = HashFunc4()(key) % (N * X);

			_bs.set(hash1);
			_bs.set(hash2);
			_bs.set(hash3);
			_bs.set(hash4);
		}

		bool test(const K& key)
		{
			size_t hash1 = HashFunc1()(key) % (N * X);
			size_t hash2 = HashFunc2()(key) % (N * X);
			size_t hash3 = HashFunc3()(key) % (N * X);
			size_t hash4 = HashFunc4()(key) % (N * X);
			// ��һ��Ϊ0��һ��������
			if (!_bs.test(hash1) || !_bs.test(hash2) || !_bs.test(hash3) || !_bs.test(hash4))
			{
				return false;
			}

			// ǰ���жϲ��ڶ���׼ȷ������������
			return true; // ���ܴ������У�ӳ�伸��λ�ö���ͻ���ͻ�����
		}

	private:
		std::bitset<N* X> _bs;
	};

	void test_bloomfilter1()
	{
		// 10:46����
		string str[] = { "apple", "banana", "cherry", "fruit", "peach1","1peach","p1each","p11each","1peach1" };
		BloomFilter<10> bf;
		for (auto& str : str)
		{
			bf.set(str);
		}

		for (auto& s : str)
		{
			cout << bf.test(s) << endl;
		}
		cout << endl;

		srand(time(0));
		for (const auto& s : str)
		{
			cout << bf.test(s + to_string(rand())) << endl;
		}
	}

	void test_bloomfilter2()
	{
		srand(time(0));
		const size_t N = 100000;
		BloomFilter<N> bf;

		std::vector<std::string> v1;
		std::string url = "https://www.cnblogs.com/-clq/archive/2012/05/31/2528153.html";

		for (size_t i = 0; i < N; ++i)
		{
			v1.push_back(url + std::to_string(i));
		}

		for (auto& str : v1)
		{
			bf.set(str);
		}

		// v2��v1�������ַ����������ǲ�һ��
		std::vector<std::string> v2;
		for (size_t i = 0; i < N; ++i)
		{
			std::string url = "https://www.cnblogs.com/-clq/archive/2012/05/31/2528153.html";
			url += std::to_string(999999 + i);
			v2.push_back(url);
		}

		size_t n2 = 0;
		for (auto& str : v2)
		{
			if (bf.test(str))
			{
				++n2;
			}
		}
		cout << "�����ַ���������:" << (double)n2 / (double)N << endl;

		// �������ַ�����
		std::vector<std::string> v3;
		for (size_t i = 0; i < N; ++i)
		{
			string url = "zhihu.com";
			url += std::to_string(i + rand());
			v3.push_back(url);
		}

		size_t n3 = 0;
		for (auto& str : v3)
		{
			if (bf.test(str))
			{
				++n3;
			}
		}
		cout << "�������ַ���������:" << (double)n3 / (double)N << endl;
	}
}