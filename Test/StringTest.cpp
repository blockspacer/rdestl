#include <UnitTest++/src/UnitTest++.h>
#include "rdestl/string.h"
#include "rdestl/simple_string_storage.h"
#include <cstdio>

namespace
{
	typedef rde::basic_string<char, rde::allocator, rde::simple_string_storage<char, rde::allocator> > simple_string;

#define CONCAT_(x, y)	x ## y
#define CONCAT2_(x, y)	CONCAT_(x, y)
#define TESTC(x)		TEST(CONCAT2_(x, POSTFIX))

#define POSTFIX			COW
#define STRING_CLASS	rde::string
#include "StringTestInc.h"

#undef STRING_CLASS
#undef POSTFIX
#define POSTFIX			Simple
#define STRING_CLASS	simple_string
#include "StringTestInc.h"

#undef POSTFIX
#undef CONCAT_
#undef CONCAT2_
#undef TESTC
}
