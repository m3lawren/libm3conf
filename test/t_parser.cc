#include <m3conf/m3config.h>
#include <m3conf/parser.h>
#include <m3conf/tokenizer.h>
#include <test/XTest.h>

XTS_TEST(ParserTests, testEmptyString) {
	struct Token* t;
	struct m3config* c;
	std::string data = "";
	XTS_ASSERT_EQUALS(0, tokenize(data.c_str(), data.size(), &t));
	c = parse(t);

	m3conf_free(c);
	free_tokens(t);
}
