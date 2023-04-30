#pragma once
#include <assert.h>
#include <utility>
#include <iostream>
#include <queue>
#include <Windows.h>
using std::pair;
namespace RB
{
	// ���������ɫ
	enum Color
	{
		RED,
		BLACK
	};

	// ������Ľڵ�
	// KV ����Ϊ T(K �� pair<const K, V>)
	// KV��map��set�㴫������RBTree��һ�㿴���Ķ���T
	//template<class K, class V>
	template<class T>
	struct RBTreeNode
	{
		RBTreeNode<T>* _left;
		RBTreeNode<T>* _right;
		RBTreeNode<T>* _parent; //ָ�򸸽ڵ��ָ��
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

	//��������
	template<class T, class Ref, class Ptr>
	struct _RBTreeIterator
	{
		typedef RBTreeNode<T> Node;
		typedef _RBTreeIterator<T, Ref, Ptr> Self;
		typedef _RBTreeIterator<T, T&, T*> iterator;
		typedef _RBTreeIterator<T, const T&, const T*> const_iterator;

		Node* _node;// ��list��������ͬ����һ���ڵ��Ա

		_RBTreeIterator(Node* node)// ���캯�����ýڵ㹹��
			:_node(node)
		{}

		// ��ͨ��������ʱ�����ǿ������� // it(it)
		// const��������ʱ�����ǹ��죬֧������ͨ����������const������ // cit(it)
		_RBTreeIterator(const iterator& cit)
			:_node(cit._node)
		{
			std::cout << "_RBTreeIterator(const iterator& cit)" << std::endl;
		}
		//  * �� ->
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

		// ǰ��++ �� --
		Self& operator++()
		{
			if (_node->_right)
			{
				Node* minRight = _node->_right;
				while (minRight->_left)
				{
					minRight = minRight->_left;
				}
				_node = minRight;// ++��minRight
			}
			else
			{
				Node* parent = _node->_parent;
				Node* cur = _node;
				// cur ���� parent �����һֱ������
				// parent Ϊ��˵���ߵ����ڵ�� _parent
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
				// �����������ڵ�
				Node* maxLeft = _node->_left;
				while (maxLeft->_right)
				{
					maxLeft = maxLeft->_right;
				}
				_node = maxLeft;// --��maxLeft
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
		// �ڵ�
		typedef RBTreeNode<T> Node;
		// ������
		typedef _RBTreeIterator<T, T&, T*> iterator;
		typedef _RBTreeIterator<T, const T&, const T*> const_iterator;

		// ��ͨ������
		iterator begin()
		{
			Node* Left = _root;
			// ������ڵ�
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
		// const������
		const_iterator begin() const
		{
			Node* Left = _root;
			// ������ڵ�
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
			// �ǿ���ֱ�ӽ������ڵ���Ϊ��
			if (_root == nullptr)
			{
				_root = new Node(data);
				_root->_col = BLACK;
				return std::make_pair(iterator(_root), true);
			}

			KeyOfT kot;// kot�����ã���ȡKey���ڱȽϴ�С
			Node* parent = nullptr;
			Node* cur = _root;
			while (cur)
			{
				// pair�� > ��������Ҫ��
				// ��ȡ����data��key��cur->_data��key���Ƚϴ�С
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
					// �����Ѵ���ֵ�ĵ��������䵱����
					return std::make_pair(iterator(cur), false);;
				}
			}

			cur = new Node(data);
			// cur���ܻ����ϵ��������仯
			// ��Ҫһ��newNode���ڷ��������ڵ�ĵ�����
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

			// parentΪ�գ���cur�Ǹ��ڵ㲻�ü�������
			// parent�ڵ���ɫΪ�죬��Ҫ�����������ϵ���
			while (parent && parent->_col == RED)
			{
				Node* grandfather = parent->_parent;
				// p��g������
				if (parent == grandfather->_left)
				{
					Node* uncle = grandfather->_right;
					// ������� p��cur���Ǻ죬g���Ǻڣ�
					// ���һ��u������Ϊ��
					if (uncle && uncle->_col == RED)
					{
						parent->_col = uncle->_col = BLACK;
						grandfather->_col = RED;
						cur = grandfather;// cur��g��������
						parent = cur->_parent;//����p
					}
					else// u������ �� u������Ϊ��
					{
						// �������cur��p����p��g����
						if (cur == parent->_left)
						{
							//      g
							//    p
							// cur
							// ��g�ҵ���
							RotateR(grandfather);
							// p��ڣ�g���
							parent->_col = BLACK;
							grandfather->_col = RED;
						}
						else// �������cur��p���ң�p��g����
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
						break;// ��ת��ɺ󼴿��˳�
					}
				}
				else// p��g���Һ���
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
						// �������
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
							// �������
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
						break;// ��ת��ɺ󼴿��˳�
					}
				}
			}
			// ������³����Ѹ��ڵ���ɫֱ�Ӹ���ɫ
			_root->_col = BLACK;
			return std::make_pair(iterator(newNode), true);
		}
		void RotateR(Node* parent)
		{
			Node* subL = parent->_left;
			Node* subLR = subL->_right;

			// 30���ұ��60����
			parent->_left = subLR;

			if (subLR != nullptr) // 30���Ҳ�Ϊ�գ�����_parentָ��
			{
				subLR->_parent = parent;
			}

			Node* ppNode = parent->_parent;
			// 60���30����
			subL->_right = parent;
			parent->_parent = subL;//��Ҫ���Ǹ���parent�ĸ�ָ��

			if (_root == parent) // parent���Ǹ�
				//if (ppNode == nullptr) //Ҳ����ʹ������ж�����
			{
				_root = subL;
				_root->_parent = nullptr;
			}
			else // parent�����������
			{
				// parent����Ͱ�subL���ӵ������Ҿ����ӵ���
				if (parent == ppNode->_left)
				{
					ppNode->_left = subL;
				}
				else
				{
					ppNode->_right = subL;
				}
				subL->_parent = ppNode;// ͬ����Ҫ���Ǹ���subL�ĸ�ָ��
			}
		}

		void RotateL(Node* parent)
		{
			Node* subR = parent->_right;
			Node* subRL = subR->_left;

			// 60������30����
			parent->_right = subRL;
			// ����subRL�ĸ�ָ��
			if (subRL)
			{
				subRL->_parent = parent;
			}
			Node* ppNode = parent->_parent;
			// 30���60����
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
			// ����������ڴ洢ÿһ��Ľڵ�
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
					std::cout << "Υ�����򣺱���·���ĺ�ɫ�ڵ������������·�������" << std::endl;
					return false;
				}

				return true;
			}

			if (root->_col == RED && root->_parent->_col == RED)
			{
				std::cout << "Υ�����򣺳���������ɫ�ڵ�" << std::endl;
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
