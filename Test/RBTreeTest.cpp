#include <UnitTest++/src/UnitTest++.h>
#include "rdestl/rb_tree.h"
#include <map>

namespace
{
	template<typename T> char HasKeyType(...);	
	template<typename T> int HasKeyType(typename T::key_type*);

	bool hasKeyType = sizeof(HasKeyType<std::map<int, int> >(0)) != sizeof(char);

	TEST(DefaultCtorEmptyTree)
	{
		rde::rb_tree<int> t;
		CHECK(t.empty());
		CHECK_EQUAL(0ul, t.size());
	}
	TEST(InsertIncreasesSize)
	{
		rde::rb_tree<int> t;
		t.insert(5);
		CHECK_EQUAL(1ul, t.size());
	}

	void PrintNode(rde::rb_tree<int>::node* n, int left, int depth)
	{
		static const char* s_left[] = { "[root]", "right", "left" };
		printf("%*s %d: Node %d, [%s, %s]\n", (depth*2), "", depth, n->key, 
			s_left[left + 1], n->color == 0 ? "red" : "black");
	}

	TEST(InsertMore)
	{
		rde::rb_tree<int> t;
		t.insert(10);
		t.insert(14);
		t.insert(2);
		t.insert(15);
		t.insert(1);
		t.insert(7);
		t.insert(5);
		t.insert(8);
		CHECK_EQUAL(8ul, t.size());
		t.traverse(PrintNode);
	}

	TEST(Erase)
	{
		rde::rb_tree<int> t;
		t.insert(10);
		t.insert(14);
		t.insert(2);
		t.insert(15);
		t.insert(1);
		t.insert(7);
		t.insert(5);
		t.insert(8);
		CHECK_EQUAL(8ul, t.size());
		t.erase(2);
		CHECK_EQUAL(7ul, t.size());
		t.traverse(PrintNode);
	}
	TEST(ClearMakesEmpty)
	{
		rde::rb_tree<int> t;
		t.insert(10);
		t.insert(14);
		t.insert(2);
		t.insert(15);
		t.insert(1);
		t.insert(7);
		t.insert(5);
		t.insert(8);
		CHECK_EQUAL(8ul, t.size());
		t.clear();
		CHECK_EQUAL(0ul, t.size());
		CHECK(t.empty());
	}
	/*TEST(IterationFromSmallest)
	{
		rde::rb_tree<int> t;
		t.insert(10); t.insert(14);	t.insert(2);
		t.insert(15); t.insert(1); t.insert(7);
		t.insert(5); t.insert(8);
		rde::rb_tree<int>::node* n(t.get_begin_node());
		CHECK_EQUAL(1, n->key);
		n = t.find_next_node(n); CHECK_EQUAL(2, n->key);
		n = t.find_next_node(n); CHECK_EQUAL(5, n->key);
		n = t.find_next_node(n); CHECK_EQUAL(7, n->key);
		n = t.find_next_node(n); CHECK_EQUAL(8, n->key);
		n = t.find_next_node(n); CHECK_EQUAL(10, n->key);
		n = t.find_next_node(n); CHECK_EQUAL(14, n->key);
		n = t.find_next_node(n); CHECK_EQUAL(15, n->key);
		n = t.find_next_node(n);
		CHECK(n == 0);
	}*/
}

