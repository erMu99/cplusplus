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

// 特化
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
// 闭散列
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
			// 大于标定负载因子，就需要扩容
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
			size_t starti = hashi;// 极端场景下都不是空，所以最多走一圈
			while (_tables[hashi]._state != EMPTY)
			{
				// 找到的条件：
				// 1. 状态是EXIST：注意 _state != EMPTY可以是DELETE
				// 2. key要相等：因为位置可能被其他冲突值占用了
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
			//if (_n / _tables.size() >= 0.7)// 这种方式是扩不了容的，整数除要么为0，要么>= 1
			if (_n * 10 / _tables.size() >= 7)
			{
				// 方式1：旧表数据，重新计算，映射到新表
				/*vector<Data> newTable;  --> vector
				newTable.resize(_tables.size() * 2);
				for (auto& e : _tables)
				{
					if (e._state == EXIST)
					{
						// 插入比方式2麻烦: 计算新的哈希地址，插入newTable
					}
				}
				// 最后交换vector
				_tables.swap(newTable);
				*/

				// 方式2：
				HashTable<K, V> newHT;  // --> HashTable
				// HashTable中的vector开好2倍空间
				newHT._tables.resize(_tables.size() * 2);
				for (auto& e : _tables)
				{
					// 旧表中存在的数据插入到新表
					if (e._state == EXIST)
					{
						// 新表调用Insert不会扩容，因为提前开好了2倍空间
						// 复用Insert
						newHT.Insert(e._kv);
					}
				}
				// 最后交换vector
				_tables.swap(newHT._tables);
			}
		}
	private:
		std::vector<Data> _tables;
		size_t _n;// 哈希表中存储的有效数据的个数
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

// 开散列
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

	// HashTable类的前置声明
	template<class K, class T, class Hash, class KeyOfT>
	class HashTable;

	// 普通迭代器类
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
			if (_node->_next) // node下一个不为空走到下一个节点
			{
				_node = _node->_next;
			}
			else // node的下一个为空找非空桶
			{
				// 计算哈希地址
				size_t hashi = Hash()(KeyOfT()(_node->_data)) % _ht->_tables.size();
				++hashi;
				while (hashi < _ht->_tables.size())
				{
					if (_ht->_tables[hashi])//找到非空桶
					{
						_node = _ht->_tables[hashi];
						break;
					}
					else
					{
						++hashi;
					}
				}
				// 所有桶走完了，走到end
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

	// const迭代器类
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

		// 普通迭代器构造const迭代器
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
			if (_node->_next) // node下一个不为空走到下一个节点
			{
				_node = _node->_next;
			}
			else // node的下一个为空找非空桶
			{
				// 计算哈希地址
				size_t hashi = Hash()(KeyOfT()(_node->_data)) % _ht->_tables.size();
				++hashi;
				while (hashi < _ht->_tables.size())
				{
					if (_ht->_tables[hashi])//找到非空桶
					{
						_node = _ht->_tables[hashi];
						break;
					}
					else
					{
						++hashi;
					}
				}
				// 所有桶走完了，走到end
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

	// 哈希表
	template<class K, class T, class Hash, class KeyOfT>
	class HashTable
	{
		// 声明 _HTIterator 为HashTable类的友元
		// 可以让 _HTIterator 结构体访问哈希表中的私有成员和保护成员
		template<class K, class T, class Hash, class KeyOfT>
		friend struct _HTIterator;
		// const迭代器
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
				// 释放桶
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

			// 负载因子控制在1，超过就扩容 --> 库中的实现 max_load_factor可查看负载因子
			if (_tables.size() == _n)
			{
				// newHT会把旧表节点释放
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
				//// 交换表中的数组
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
						// 计算新的哈希地址
						size_t hashi = Hash()(kot(data)) % newTables.size();
						// 头插到新表 -- 旧表节点没有被释放
						cur->_next = newTables[hashi];
						newTables[hashi] = cur;
						cur = next;
					}
					_tables[i] = nullptr;// 旧表位置置空，防止交换之后newTables析构出错
				}
				_tables.swap(newTables);
			}

			size_t hashi = Hash()(kot(data)) % _tables.size();
			// 头插
			Node* newnode = new Node(data);
			newnode->_next = _tables[hashi];
			_tables[hashi] = newnode;
			++_n;

			return  std::make_pair(iterator(newnode, this), true);
		}

		// 普通迭代器Find
		iterator Find(const K& key)
		{
			size_t hashi = Hash()(key) % _tables.size();
			Node* cur = _tables[hashi];
			// 单链表查找
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

		// const迭代器Find

		const_iterator Find(const K& key) const
		{
			size_t hashi = Hash()(key) % _tables.size();
			Node* cur = _tables[hashi];
			// 单链表查找
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
			// 单链表删除
			while (cur)
			{
				if (KeyOfT()(cur->_data) == key)
				{
					// 首结点删除特殊处理
					if (cur == _tables[hashi])
					{
						_tables[hashi] = cur->_next;
					}
					else
					{
						prev->_next = cur->_next;// 链接好_next指针
					}

					// 删除
					delete cur;
					--_n;// 记得--有效数据个数

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
			// 当然这里也可以用二分查找
			for (int i = 0; i < __stl_num_primes; ++i)
			{
				if (__stl_prime_list[i] > n)
				{
					return __stl_prime_list[i];
				}
			}

			return __stl_prime_list[__stl_num_primes - 1];
		}
		std::vector<Node*> _tables;// 指针数组
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
