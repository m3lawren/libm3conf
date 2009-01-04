#include <test/XTest.h>

int main(void) {
	XTestSuite::instance().runAllTests();
	return 0;
}
