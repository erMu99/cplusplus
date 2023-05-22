#pragma once
#include <vector>
#include <iostream>
using std::cout;
using std::endl;

namespace nb
{
	// 非类型模板参数
	template<size_t N>
	class bitset
	{
	public:
		bitset()
		{
			//_bits.resize(N/8+1, 0);
			_bits.resize((N >> 3) + 1, 0);
		}
		void set(size_t pos)
		{
			//size_t  i = pos / 8;
			size_t i = pos >> 3;
			size_t j = pos % 8;
			_bits[i] |= (1 << j);
		}
		void reset(size_t pos)
		{
			size_t  i = pos >> 3;
			size_t j = pos % 8;

			_bits[i] &= (~(1 << j));
		}

		bool test(size_t pos)
		{
			size_t i = pos >> 3;
			size_t j = pos % 8;

			return (_bits[i] >> j) & 1;
			return _bits[i] & (1 << j);
		}

	private:
		std::vector<char>  _bits;
	};

	void test_bitset()
	{
		//bitset<100> bs1;
		bitset<-1> bs2;
		//bitset<10000> bs2;

		bs2.set(10);
		bs2.set(10000);
		bs2.set(8888);

		cout << bs2.test(10) << endl;
		cout << bs2.test(10000) << endl;
		cout << bs2.test(8888) << endl;
		cout << bs2.test(8887) << endl;
		cout << bs2.test(9999) << endl << endl;

		bs2.reset(8888);
		bs2.set(8887);

		cout << bs2.test(10) << endl;
		cout << bs2.test(10000) << endl;
		cout << bs2.test(8888) << endl;
		cout << bs2.test(8887) << endl;
		cout << bs2.test(9999) << endl;
	}

	template<size_t N>
	class twobitset
	{
	public:
		void set(size_t x)
		{
			if (!_bs1.test(x) && !_bs2.test(x)) // 00 --> 01 0次变成1次
			{
				_bs2.set(x); // 01
			}
			else if (!_bs1.test(x) && _bs2.test(x)) // 01 --> 1次变成1次以上
			{
				_bs1.set(x);
				_bs2.reset(x); // 10
			}

			// 10 1次以上不做处理
		}

		void PirntOnce()
		{
			for (size_t i = 0; i < N; ++i)
			{
				if (!_bs1.test(i) && _bs2.test(i))
				{
					cout << i << endl;
				}
			}
			cout << endl;
		}

	private:
		bitset<N> _bs1;
		bitset<N> _bs2;
	};

	void test_twobitset()
	{
		twobitset<100> tbs;
		int a[] = { 1, 2, 3, 4, 4, 5, 5, 99, 22 };
		for (auto e : a)
		{
			tbs.set(e);
		}

		tbs.PirntOnce();
	}
}
