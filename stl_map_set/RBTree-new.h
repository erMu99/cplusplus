#pragma once
#include <assert.h>
#include <utility>
#include <iostream>
#include <queue>
#include <Windows.h>
using std::pair;
namespace RB
{
	// 红黑树的颜色
	enum Color
	{
		RED,
		BLACK
	};

	// 红黑树的节点
	// KV 改造为 T(K 或 pair<const K, V>)
	// KV由map和set层传过来，RBTree这一层看到的都是T
	//template<class K, class V>
	template<class T>
	struct RBTreeNode
	{
		RBTreeNode<T>* _left;
		RBTreeNode<T>* _right;
		RBTreeNode<T>* _parent; //指向父节点的指针
		//pair<K, V> _kv;
		T _data;
		Color _col;

		//RBTreeNode(const pair<K, V>& kv)
		RBTreeNode(const T& data)
			:_left(nullptr)
			, _right(nullptr)
			, _parent(nullptr)
			, _data(data)
			, _col(RED)
		{}
	};

	//迭代器类
	template<class T, class Ref, class Ptr>
	struct _RBTreeIterator
	{
		typedef RBTreeNode<T> Node;
		typedef _RBTreeIterator<T, Ref, Ptr> Self;
		typedef _RBTreeIterator<T, T&, T*> iterator;
		typedef _RBTreeIterator<T, const T&, const T*> const_iterator;

		Node* _node;// 与list迭代器相同，有一个节点成员

		_RBTreeIterator(Node* node)// 构造函数：用节点构造
			:_node(node)
		{}

		// 普通迭代器的时候，他是拷贝构造 // it(it)
		// const迭代器的时候，他是构造，支持用普通迭代器构造const迭代器 // cit(it)
		_RBTreeIterator(const iterator& cit)
			:_node(cit._node)
		{
			std::cout << "_RBTreeIterator(const iterator& cit)" << std::endl;
		}
		//  * 和 ->
		Ref operator* ()
		{
			return _node->_data;
		}

		Ptr operator-> ()
		{
			return &_node->_data;
		}

		bool operator!= (const Self& s) const
		{
			return _node != s._node;
		}

		bool operator== (const Self& s) const
		{
			return _node == s._node;
		}

		// 前置++ 和 --
		Self& operator++()
		{
			if (_node->_right)
			{
				Node* minRight = _node->_right;
				while (minRight->_left)
				{
					minRight = minRight->_left;
				}
				_node = minRight;// ++到minRight
			}
			else
			{
				Node* parent = _node->_parent;
				Node* cur = _node;
				// cur 不是 parent 的左就一直往上走
				// parent 为空说明走到根节点的 _parent
				while (parent && cur == parent->_right)
				{
					cur = cur->_parent;
					parent = parent->_parent;
				}
				_node = parent;
			}
			return *this;
		}
		Self& operator--()
		{
			if (_node->_left)
			{
				// 左子树的最大节点
				Node* maxLeft = _node->_left;
				while (maxLeft->_right)
				{
					maxLeft = maxLeft->_right;
				}
				_node = maxLeft;// --到maxLeft
			}
			else
			{
				Node* cur = _node;
				Node* parent = _node->_parent;
				while (parent && cur == parent->_left)
				{
					cur = cur->_parent;
					parent = parent->_parent;
				}
				_node = parent;
			}
			return *this;
		}
	};
	// map --> RBTree<K, pair<const K, V>, MapKeyOfT> _t
	// set --> RBTree<K, K, SetKeyOfT> _t
	template<class K, class T, class KeyOfT>
	class RBTree
	{
	public:
		// 节点
		typedef RBTreeNode<T> Node;
		// 迭代器
		typedef _RBTreeIterator<T, T&, T*> iterator;
		typedef _RBTreeIterator<T, const T&, const T*> const_iterator;

		// 普通迭代器
		iterator begin()
		{
			Node* Left = _root;
			// 找最左节点
			while (Left && Left->_left)
			{
				Left = Left->_left;
			}
			return iterator(Left);
		}
		iterator end()
		{
			return iterator(nullptr);
		}
		// const迭代器
		const_iterator begin() const
		{
			Node* Left = _root;
			// 找最左节点
			while (Left && Left->_left)
			{
				Left = Left->_left;
			}
			return const_iterator(Left);
		}
		const_iterator end() const
		{
			return const_iterator(nullptr);
		}
		pair<iterator, bool> Insert(const T& data)
		{
			// 是空树直接将新增节点作为根
			if (_root == nullptr)
			{
				_root = new Node(data);
				_root->_col = BLACK;
				return std::make_pair(iterator(_root), true);
			}

			KeyOfT kot;// kot的作用：获取Key用于比较大小
			Node* parent = nullptr;
			Node* cur = _root;
			while (cur)
			{
				// pair的 > 不能满足要求
				// 获取插入data的key和cur->_data的key，比较大小
				if (kot(data) > kot(cur->_data))
				{
					parent = cur;
					cur = cur->_right;
				}
				else if (kot(data) < kot(cur->_data))
				{
					parent = cur;
					cur = cur->_left;
				}
				else
				{
					// 返回已存在值的迭代器，充当查找
					return std::make_pair(iterator(cur), false);;
				}
			}

			cur = new Node(data);
			// cur可能会向上调整发生变化
			// 需要一个newNode用于返回新增节点的迭代器
			Node* newNode = cur;
			cur->_col = RED;

			if (kot(data) > kot(parent->_data))
			{
				parent->_right = cur;
				cur->_parent = parent;
			}
			else
			{
				parent->_left = cur;
				cur->_parent = parent;
			}

			// parent为空，则cur是根节点不用继续更新
			// parent节点颜色为红，需要继续相信向上调整
			while (parent && parent->_col == RED)
			{
				Node* grandfather = parent->_parent;
				// p是g的左孩子
				if (parent == grandfather->_left)
				{
					Node* uncle = grandfather->_right;
					// 所有情况 p、cur都是红，g都是黑，
					// 情况一：u存在且为红
					if (uncle && uncle->_col == RED)
					{
						parent->_col = uncle->_col = BLACK;
						grandfather->_col = RED;
						cur = grandfather;// cur变g继续更新
						parent = cur->_parent;//更新p
					}
					else// u不存在 或 u存在且为黑
					{
						// 情况二：cur是p的左，p是g的左
						if (cur == parent->_left)
						{
							//      g
							//    p
							// cur
							// 对g右单旋
							RotateR(grandfather);
							// p变黑，g变红
							parent->_col = BLACK;
							grandfather->_col = RED;
						}
						else// 情况三：cur是p的右，p是g的左
						{
							//     g
							//   p
							//     cur
							RotateL(parent);
							//      g
							//   cur
							// p
							RotateR(grandfather);
							cur->_col = BLACK;
							grandfather->_col = RED;
						}
						break;// 旋转完成后即可退出
					}
				}
				else// p是g的右孩子
				{
					Node* uncle = grandfather->_left;
					if (uncle && uncle->_col == RED)
					{
						parent->_col = uncle->_col = BLACK;
						grandfather->_col = RED;
						cur = grandfather;
						parent = cur->_parent;
					}
					else
					{
						// 情况二：
						if (cur == parent->_right)
						{
							// g
							//   p
							//    cur
							RotateL(grandfather);
							parent->_col = BLACK;
							grandfather->_col = RED;
						}
						else
						{
							// 情况三：
							//   g
							//     p
							// cur
							RotateR(parent);
							//  g
							//    cur
							//       p
							RotateL(grandfather);
							cur->_col = BLACK;
							grandfather->_col = RED;
						}
						break;// 旋转完成后即可退出
					}
				}
			}
			// 避免更新出错，把根节点颜色直接给黑色
			_root->_col = BLACK;
			return std::make_pair(iterator(newNode), true);
		}
		void RotateR(Node* parent)
		{
			Node* subL = parent->_left;
			Node* subLR = subL->_right;

			// 30的右变成60的左
			parent->_left = subLR;

			if (subLR != nullptr) // 30的右不为空，更新_parent指针
			{
				subLR->_parent = parent;
			}

			Node* ppNode = parent->_parent;
			// 60变成30的右
			subL->_right = parent;
			parent->_parent = subL;//不要忘记更新parent的父指针

			if (_root == parent) // parent就是根
				//if (ppNode == nullptr) //也可以使用这个判断条件
			{
				_root = subL;
				_root->_parent = nullptr;
			}
			else // parent是左或右子树
			{
				// parent是左就把subL链接到左，是右就链接到右
				if (parent == ppNode->_left)
				{
					ppNode->_left = subL;
				}
				else
				{
					ppNode->_right = subL;
				}
				subL->_parent = ppNode;// 同样不要忘记更新subL的父指针
			}
		}

		void RotateL(Node* parent)
		{
			Node* subR = parent->_right;
			Node* subRL = subR->_left;

			// 60的左变成30的右
			parent->_right = subRL;
			// 更新subRL的父指针
			if (subRL)
			{
				subRL->_parent = parent;
			}
			Node* ppNode = parent->_parent;
			// 30变成60的左
			subR->_left = parent;
			parent->_parent = subR;

			//if (_root == parent)
			if (ppNode == nullptr)
			{
				_root = subR;
				_root->_parent = nullptr;
			}
			else
			{
				if (parent == ppNode->_left)
				{
					ppNode->_left = subR;
				}
				else
				{
					ppNode->_right = subR;
				}
				subR->_parent = ppNode;
			}
		}
		void Inorder()
		{
			_Inorder(_root);
		}

		void _Inorder(Node* root)
		{
			if (root == nullptr)
				return;

			_Inorder(root->_left);
			std::cout << root->_kv.first << ":" << root->_kv.second << (root->_col ? " BLACK" : " RED") << std::endl;
			_Inorder(root->_right);
		}

		void levelOrder()
		{
			_levelOrder(_root);
		}

		void _levelOrder(Node* root)
		{
			if (root == nullptr)
				return;
			// 定义队列用于存储每一层的节点
			std::queue<Node*> q;
			q.push(root);
			while (!q.empty())
			{
				auto node = q.front();
				q.pop();

				std::cout << node->_kv.second << "(" << (node->_col ? " BLACK" : " RED") << ")\n";
				if (node->_left != nullptr) {
					q.push(node->_left);
				}
				if (node->_right != nullptr) {
					q.push(node->_right);
				}
			}
		}

		bool Check(Node* root, int blackNum, const int ref)
		{
			if (root == nullptr)
			{
				//cout << blackNum << endl;
				if (blackNum != ref)
				{
					std::cout << "违反规则：本条路径的黑色节点的数量跟最左路径不相等" << std::endl;
					return false;
				}

				return true;
			}

			if (root->_col == RED && root->_parent->_col == RED)
			{
				std::cout << "违反规则：出现连续红色节点" << std::endl;
				return false;
			}

			if (root->_col == BLACK)
			{
				++blackNum;
			}

			return Check(root->_left, blackNum, ref)
				&& Check(root->_right, blackNum, ref);
		}

		bool IsBalance()
		{
			if (_root == nullptr)
			{
				return true;
			}

			if (_root->_col != BLACK)
			{
				return false;
			}

			int ref = 0;
			Node* left = _root;
			while (left)
			{
				if (left->_col == BLACK)
				{
					++ref;
				}

				left = left->_left;
			}

			return Check(_root, 0, ref);
		}
	private:
		Node* _root = nullptr;
	};
}
