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
			size_t starti = hashi;// ���˳����¶����ǿգ����������һȦ
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
				if (hashi == starti)
				{
					break;
				}
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
	template<class T>
	struct HashNode
	{
		//std::pair<K, V> _kv;
		//HashNode<K, V>* _next;
		T _data;
		HashNode<T>* _next;
		HashNode(const T& data)
			:_data(data)
			, _next(nullptr)
		{}
	};

	// HashTable���ǰ������
	template<class K, class T, class Hash, class KeyOfT>
	class HashTable;

	// ��ͨ��������
	template<class K, class T, class Hash, class KeyOfT>
	//template<class K, class T, class Ref, class Ptr, class Hash, class KeyOfT>
	struct _HTIterator
	{
		typedef HashNode<T> Node;
		typedef _HTIterator<K, T, Hash, KeyOfT> Self;
		typedef _HTIterator<K, T, Hash, KeyOfT> iterator;
		//typedef _HTIterator<K, T, Ref, Ptr, Hash, KeyOfT> Self;
		//typedef _HTIterator<K, T, T&, T*, Hash, KeyOfT> iterator;
		typedef HashTable<K, T, Hash, KeyOfT> HT;

		_HTIterator(Node* node, HT* ht)
			:_node(node)
			, _ht(ht)
		{
			//std::cout << "_HTIterator()" << std::endl;
		}

		_HTIterator(const iterator& it)
			:_node(it._node)
			, _ht(it._ht)
		{
			//std::cout << "_HTIterator(const iterator& it)" << std::endl;
		}

		T& operator*()
			//Ref operator*()
		{
			return _node->_data;
		}
		T* operator->()
			//Ptr operator->()
		{
			return &_node->_data;
		}

		Self& operator++()
		{
			if (_node->_next) // node��һ����Ϊ���ߵ���һ���ڵ�
			{
				_node = _node->_next;
			}
			else // node����һ��Ϊ���ҷǿ�Ͱ
			{
				// �����ϣ��ַ
				size_t hashi = Hash()(KeyOfT()(_node->_data)) % _ht->_tables.size();
				++hashi;
				while (hashi < _ht->_tables.size())
				{
					if (_ht->_tables[hashi])//�ҵ��ǿ�Ͱ
					{
						_node = _ht->_tables[hashi];
						break;
					}
					else
					{
						++hashi;
					}
				}
				// ����Ͱ�����ˣ��ߵ�end
				if (hashi == _ht->_tables.size())
					_node = nullptr;
			}
			return *this;
		}

		bool operator != (const Self& s) const
		{
			return _node != s._node;
		}

		bool operator == (const Self& s) const
		{
			return _node == s._node;
		}

		Node* _node;
		HT* _ht;
	};

	// const��������
	template<class K, class T, class Hash, class KeyOfT>
	//template<class K, class T, class Ref, class Ptr, class Hash, class KeyOfT>
	struct const_HTIterator
	{
		typedef HashNode<T> Node;
		typedef _HTIterator<K, T, Hash, KeyOfT> iterator;
		typedef const_HTIterator<K, T, Hash, KeyOfT> const_iterator;
		typedef const_HTIterator<K, T, Hash, KeyOfT> Self;
		//typedef _HTIterator<K, T, Ref, Ptr, Hash, KeyOfT> Self;
		//typedef _HTIterator<K, T, T&, T*, Hash, KeyOfT> iterator;
		typedef HashTable<K, T, Hash, KeyOfT> HT;

		const_HTIterator(const Node* node, const HT* ht)
			:_node(node)
			, _ht(ht)
		{
			//std::cout << "_HTIterator()" << std::endl;
		}

		// ��ͨ����������const������
		const_HTIterator(const iterator& it)
			:_node(it._node)
			, _ht(it._ht)
		{
			std::cout << "iterator --> const_iterator" << std::endl;
		}

		const T& operator*()
			//Ref operator*()
		{
			return _node->_data;
		}
		const T* operator->()
			//Ptr operator->()
		{
			return &_node->_data;
		}

		Self& operator++()
		{
			if (_node->_next) // node��һ����Ϊ���ߵ���һ���ڵ�
			{
				_node = _node->_next;
			}
			else // node����һ��Ϊ���ҷǿ�Ͱ
			{
				// �����ϣ��ַ
				size_t hashi = Hash()(KeyOfT()(_node->_data)) % _ht->_tables.size();
				++hashi;
				while (hashi < _ht->_tables.size())
				{
					if (_ht->_tables[hashi])//�ҵ��ǿ�Ͱ
					{
						_node = _ht->_tables[hashi];
						break;
					}
					else
					{
						++hashi;
					}
				}
				// ����Ͱ�����ˣ��ߵ�end
				if (hashi == _ht->_tables.size())
					_node = nullptr;
			}
			return *this;
		}

		bool operator != (const Self& s) const
		{
			return _node != s._node;
		}

		bool operator == (const Self& s) const
		{
			return _node == s._node;
		}

		const Node* _node;
		const HT* _ht;
	};

	// ��ϣ��
	template<class K, class T, class Hash, class KeyOfT>
	class HashTable
	{
		// ���� _HTIterator ΪHashTable�����Ԫ
		// ������ _HTIterator �ṹ����ʹ�ϣ���е�˽�г�Ա�ͱ�����Ա
		template<class K, class T, class Hash, class KeyOfT>
		friend struct _HTIterator;
		// const������
		template<class K, class T, class Hash, class KeyOfT>
		friend struct const_HTIterator;
		//template<class K, class T, class Ref, class Ptr, class Hash, class KeyOfT>
		//friend struct _HTIterator;
	public:
		typedef HashNode<T> Node;
		typedef _HTIterator<K, T, Hash, KeyOfT> iterator;
		typedef const_HTIterator<K, T, Hash, KeyOfT> const_iterator;
		//typedef _HTIterator<K, T, T&, T*, Hash, KeyOfT> iterator;
		//typedef _HTIterator<K, T, const T&, const T*, Hash, KeyOfT> const_iterator;

		iterator begin()
		{
			for (size_t i = 0; i < _tables.size(); ++i)
			{
				if (_tables[i])
					return iterator(_tables[i], this);
			}
			return iterator(nullptr, this);
		}

		iterator end()
		{
			return iterator(nullptr, this);
		}

		const_iterator begin() const
		{
			for (size_t i = 0; i < _tables.size(); ++i)
			{
				if (_tables[i])
					return const_iterator(_tables[i], this);
			}
			return const_iterator(nullptr, this);
		}

		const_iterator end() const
		{
			return const_iterator(nullptr, this);
		}

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
		std::pair<iterator, bool> Insert(const T& data)
		{
			KeyOfT kot;
			iterator it = Find(kot(data));
			if (it != end())
				return std::make_pair(it, false);

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
						size_t hashi = Hash()(kot(data)) % newTables.size();
						// ͷ�嵽�±� -- �ɱ�ڵ�û�б��ͷ�
						cur->_next = newTables[hashi];
						newTables[hashi] = cur;
						cur = next;
					}
					_tables[i] = nullptr;// �ɱ�λ���ÿգ���ֹ����֮��newTables��������
				}
				_tables.swap(newTables);
			}

			size_t hashi = Hash()(kot(data)) % _tables.size();
			// ͷ��
			Node* newnode = new Node(data);
			newnode->_next = _tables[hashi];
			_tables[hashi] = newnode;
			++_n;

			return  std::make_pair(iterator(newnode, this), true);
		}

		// ��ͨ������Find
		iterator Find(const K& key)
		{
			size_t hashi = Hash()(key) % _tables.size();
			Node* cur = _tables[hashi];
			// ���������
			while (cur)
			{
				if (KeyOfT()(cur->_data) == key)
				{
					return iterator(cur, this);
				}
				else
				{
					cur = cur->_next;
				}
			}
			return iterator(nullptr, this);
		}

		// const������Find

		const_iterator Find(const K& key) const
		{
			size_t hashi = Hash()(key) % _tables.size();
			Node* cur = _tables[hashi];
			// ���������
			while (cur)
			{
				if (KeyOfT()(cur->_data) == key)
				{
					return const_iterator(cur, this);
				}
				else
				{
					cur = cur->_next;
				}
			}
			return const_iterator(nullptr, this);
		}

		bool Erase(const K& key)
		{
			size_t hashi = Hash()(key) % _tables.size();
			Node* prev = nullptr;
			Node* cur = _tables[hashi];
			// ������ɾ��
			while (cur)
			{
				if (KeyOfT()(cur->_data) == key)
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
	void Test_Hash()
	{
		/*bucketHash::HashTable<int, int> ht;
		int a[] = { 18, 9, 7, 25, 15, 33 };
		for (int e : a)
		{
			ht.Insert({ e, e });
		}
		std::cout << ht.Find(18) << std::endl;
		ht.Erase(18);
		std::cout << ht.Find(18) << std::endl;

		bucketHash::HashTable<std::string, int> ht2;
		std::string s[] = { "apple","banana","cherry" , "cherry" };
		for (int i = 0; i < 4; ++i)
		{
			ht2.Insert({ s[i], i });
		}

		std::cout << ht2.Find("banana") << std::endl;
		ht2.Erase("banana");
		std::cout << ht2.Find("banana") << std::endl;*/
	}
};
