#pragma once
#include <vector>
#include <string>
#include <utility>
#include <iostream>
template<class K>
struct HashFunc
{
	size_t operator()(const K& key)
	{
		return (size_t)key;
	}
};

// �ػ�
template<>
struct HashFunc<std::string>
{
	size_t operator()(const std::string& key)
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
// ��ɢ��
namespace closeHash
{
	enum State
	{
		EMPTY,
		EXIST,
		DELETE,
	};
	template<class K, class V>
	struct HashData
	{
		std::pair<K, V> _kv;
		State _state = EMPTY;
	};
	template<class K, class V, class Hash = HashFunc<K>>
	class HashTable
	{
		typedef HashData<K, V> Data;
	public:
		HashTable()
			:_n(0)
		{
			_tables.resize(10);
		}
		bool Insert(const std::pair<K, V>& kv)
		{
			if (Find(kv.first))
				return false;
			// ���ڱ궨�������ӣ�����Ҫ����
			CheckCapacity();
			Hash hf;
			size_t hashi = hf(kv.first) % _tables.size();
			while (_tables[hashi]._state == EXIST)
			{
				++hashi;
				hashi %= _tables.size();
			}
			_tables[hashi]._kv = kv;
			_tables[hashi]._state = EXIST;
			++_n;
			return true;
		}

		bool Erase(const K& key)
		{
			Data* ret = Find(key);
			if (ret)
			{
				ret->_state = DELETE;
				--_n;
				return true;
			}
			else
			{
				return false;
			}
		}

		Data* Find(const K& key)
		{
			//Hash hf;// Hash()(key)
			size_t hashi = Hash()(key) % _tables.size();
			//size_t hashi = key % _tables.size();
			while (_tables[hashi]._state != EMPTY)
			{
				// �ҵ���������
				// 1. ״̬��EXIST��ע�� _state != EMPTY������DELETE
				// 2. keyҪ��ȣ���Ϊλ�ÿ��ܱ�������ͻֵռ����
				if (_tables[hashi]._state == EXIST
					&& _tables[hashi]._kv.first == key)
				{
					return &_tables[hashi];
				}

				++hashi;
				hashi %= _tables.size();
			}

			return nullptr;
		}
	private:
		void CheckCapacity()
		{
			//if (_n / _tables.size() >= 0.7)// ���ַ�ʽ���������ݵģ�������ҪôΪ0��Ҫô>= 1
			if (_n * 10 / _tables.size() >= 7)
			{
				// ��ʽ1���ɱ����ݣ����¼��㣬ӳ�䵽�±�
				/*vector<Data> newTable;  --> vector
				newTable.resize(_tables.size() * 2);
				for (auto& e : _tables)
				{
					if (e._state == EXIST)
					{
						// ����ȷ�ʽ2�鷳: �����µĹ�ϣ��ַ������newTable
					}
				}
				// ��󽻻�vector
				_tables.swap(newTable);
				*/

				// ��ʽ2��
				HashTable<K, V> newHT;  // --> HashTable
				// HashTable�е�vector����2���ռ�
				newHT._tables.resize(_tables.size() * 2);
				for (auto& e : _tables)
				{
					// �ɱ��д��ڵ����ݲ��뵽�±�
					if (e._state == EXIST)
					{
						// �±����Insert�������ݣ���Ϊ��ǰ������2���ռ�
						// ����Insert
						newHT.Insert(e._kv);
					}
				}
				// ��󽻻�vector
				_tables.swap(newHT._tables);
			}
		}
	private:
		std::vector<Data> _tables;
		size_t _n;// ��ϣ���д洢����Ч���ݵĸ���
	};
	void Test1()
	{
		//HashTable<int, int> ht;
		//int a[] = { 18, 18, 8, 9, 7, 25, 15, 33 };
		//for (int e : a)
		//{
		//	ht.Insert({ e, e });
		//}
		//std::cout << ht.Find(8) << std::endl;
		//ht.Erase(8);
		//std::cout << ht.Find(8) << std::endl;

		HashTable<std::string, int> ht2;
		std::string s[] = { "apple","banana","cherry" , "cherry" };
		for (int i = 0; i < 4; ++i)
		{
			ht2.Insert({ s[i], i });
		}

		std::cout << ht2.Find("banana") << std::endl;
		ht2.Erase("banana");
		std::cout << ht2.Find("banana") << std::endl;
	}
}

// ��ɢ��
namespace bucketHash
{
	template<class K, class V>
	struct HashNode
	{
		std::pair<K, V> _kv;
		HashNode<K, V>* _next;
		HashNode(const std::pair<K, V>& kv)
			:_kv(kv)
			, _next(nullptr)
		{}
	};
	template<class K, class V, class Hash = HashFunc<K>>
	class HashTable
	{
	public:
		typedef HashNode<K, V> Node;
		HashTable()
			:_n(0)
		{
			//_tables.resize(10);
			_tables.resize(__stl_next_prime(0));
		}
		~HashTable()
		{
			for (size_t i = 0; i < _tables.size(); ++i)
			{
				// �ͷ�Ͱ
				Node* cur = _tables[i];
				while (cur)
				{
					Node* next = cur->_next;
					delete cur;
					cur = next;
				}

				_tables[i] = nullptr;
			}
		}

		bool Insert(const std::pair<K, V>& kv)
		{
			if (Find(kv.first))
				return false;

			// �������ӿ�����1������������ --> ���е�ʵ�� max_load_factor�ɲ鿴��������
			if (_tables.size() == _n)
			{
				// newHT��Ѿɱ�ڵ��ͷ�
				//HashTable<K, V, Hash> newHT;
				//newHT._tables.resize(_tables.size() * 2);
				//for (auto cur : _tables)
				//{
				//	while (cur)
				//	{
				//		newHT.Insert(cur->_kv);
				//		cur = cur->_next;
				//	}
				//}
				//// �������е�����
				//_tables.swap(newHT._tables);

				std::vector<Node*> newTables;
				//newTables.resize(2 * _tables.size(), nullptr);
				newTables.resize(__stl_next_prime(_tables.size()), nullptr);
				for (size_t i = 0; i < _tables.size(); ++i)
				{
					Node* cur = _tables[i];
					while (cur)
					{
						Node* next = cur->_next;
						// �����µĹ�ϣ��ַ
						size_t hashi = Hash()(cur->_kv.first) % newTables.size();
						// ͷ�嵽�±� -- �ɱ�ڵ�û�б��ͷ�
						cur->_next = newTables[hashi];
						newTables[hashi] = cur;
						cur = next;
					}
					_tables[i] = nullptr;// �ɱ�λ���ÿգ���ֹ����֮��newTables��������
				}
				_tables.swap(newTables);
			}

			size_t hashi = Hash()(kv.first) % _tables.size();
			// ͷ��
			Node* newnode = new Node(kv);
			newnode->_next = _tables[hashi];
			_tables[hashi] = newnode;
			++_n;

			return true;
		}

		Node* Find(const K& key)
		{
			size_t hashi = Hash()(key) % _tables.size();
			Node* cur = _tables[hashi];
			// ���������
			while (cur)
			{
				if (cur->_kv.first == key)
				{
					return cur;
				}
				else
				{
					cur = cur->_next;
				}
			}
			return nullptr;
		}

		bool Erase(const K& key)
		{
			size_t hashi = Hash()(key) % _tables.size();
			Node* prev = nullptr;
			Node* cur = _tables[hashi];
			// ������ɾ��
			while (cur)
			{
				if (cur->_kv.first == key)
				{
					// �׽��ɾ�����⴦��
					if (cur == _tables[hashi])
					{
						_tables[hashi] = cur->_next;
					}
					else
					{
						prev->_next = cur->_next;// ���Ӻ�_nextָ��
					}

					// ɾ��
					delete cur;
					--_n;// �ǵ�--��Ч���ݸ���

					return true;
				}
				else
				{
					prev = cur;
					cur = cur->_next;
				}
			}
			return false;
		}

	private:
		inline unsigned long __stl_next_prime(unsigned long n)
		{
			static const int __stl_num_primes = 28;
			static const unsigned long __stl_prime_list[__stl_num_primes] =
			{
				53, 97, 193, 389, 769,
				1543, 3079, 6151, 12289, 24593,
				49157, 98317, 196613, 393241, 786433,
				1572869, 3145739, 6291469, 12582917, 25165843,
				50331653, 100663319, 201326611, 402653189, 805306457,
				1610612741, 3221225473, 4294967291
			};
			// ��Ȼ����Ҳ�����ö��ֲ���
			for (int i = 0; i < __stl_num_primes; ++i)
			{
				if (__stl_prime_list[i] > n)
				{
					return __stl_prime_list[i];
				}
			}

			return __stl_prime_list[__stl_num_primes - 1];
		}
		std::vector<Node*> _tables;// ָ������
		size_t _n = 0;
	};
	void Test1()
	{
		bucketHash::HashTable<int, int> ht;
		int a[] = { 18, 9, 7, 25, 15, 33 };
		for (int e : a)
		{
			ht.Insert({ e, e });
		}
		//std::cout << ht.Find(8) << std::endl;
		//ht.Erase(8);
		//std::cout << ht.Find(8) << std::endl;

		//bucketHash::HashTable<std::string, int> ht2;
		//std::string s[] = { "apple","banana","cherry" , "cherry" };
		//for (int i = 0; i < 4; ++i)
		//{
		//	ht2.Insert({ s[i], i });
		//}

		//std::cout << ht2.Find("banana") << std::endl;
		//ht2.Erase("banana");
		//std::cout << ht2.Find("banana") << std::endl;
	}
};
