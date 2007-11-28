#include <UnitTest++/src/UnitTest++.h>
#include "rdestl/intrusive_list.h"

namespace
{
	struct MyNode : public rdestl::intrusive_list_node
	{
		explicit MyNode(int i = 0): data(i) {}
		int	data;
	};

	TEST(DefaultCtorConstructsEmptyList)
	{
		rdestl::intrusive_list<MyNode> l;
		CHECK(l.empty());
		CHECK_EQUAL(0ul, l.size());
	}
	TEST(PushBack)
	{
		rdestl::intrusive_list<MyNode> l;
		MyNode node;
		node.data = 5;
		l.push_back(&node);
		CHECK_EQUAL(1ul, l.size());
		MyNode node2;
		node2.data = 10;
		l.push_back(&node2);
		CHECK_EQUAL(2ul, l.size());
		CHECK_EQUAL(10, l.back()->data);
	}
	TEST(PushBackPtr)
	{
		rdestl::intrusive_list<MyNode> l;
		MyNode* n(new MyNode());
		l.push_back(n);
		CHECK_EQUAL(1ul, l.size());
		n = new MyNode();
	}

	TEST(PushFront)
	{
		rdestl::intrusive_list<MyNode> l;
		MyNode node;
		node.data = 5;
		l.push_front(&node);
		CHECK_EQUAL(1ul, l.size());
		MyNode node2;
		node2.data = 10;
		l.push_front(&node2);
		CHECK_EQUAL(2ul, l.size());
		CHECK_EQUAL(10, l.front()->data);
		CHECK_EQUAL(5, l.back()->data);
	}

	TEST(PopBack)
	{
		rdestl::intrusive_list<MyNode> l;
		MyNode node;
		node.data = 5;
		l.push_back(&node);
		CHECK_EQUAL(1ul, l.size());
		MyNode node2;
		node2.data = 10;
		l.push_back(&node2);
		CHECK_EQUAL(2ul, l.size());
		l.pop_back();
		CHECK_EQUAL(1ul, l.size());
		CHECK_EQUAL(5, l.front()->data);
	}

	TEST(IterEmpty)
	{
		rdestl::intrusive_list<MyNode> l;
		CHECK(l.begin() == l.end());
	}
	TEST(IterTest)
	{
		rdestl::intrusive_list<MyNode> l;
		MyNode node;
		node.data = 5;
		l.push_back(&node);
		rdestl::intrusive_list<MyNode>::iterator it(l.begin());
		CHECK(it != l.end());
		++it;
		CHECK(it == l.end());
		--it;
		CHECK(it != l.end());
		it++;
		CHECK(it == l.end());
		it--;
		CHECK(it != l.end());

		MyNode node2;
		node2.data = 10;
		l.push_back(&node2);
		it = l.begin();
		CHECK_EQUAL(5, it->data);
		CHECK_EQUAL(10, (++it)->data);

		l.insert(it, new MyNode(15));
		CHECK_EQUAL(3ul, l.size());
		it = l.begin();
		++it;
		CHECK_EQUAL(15, it->data);
		CHECK_EQUAL(10, (++it)->data);
	}
}
