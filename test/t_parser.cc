#include <m3conf/m3config.h>
#include <m3conf/m3conf_parser.h>
#include <m3conf/m3conf_tokenizer.h>
#include <test/XTest.h>

XTS_TEST(ParserTests, testEmptyString) {
	struct m3conf_token* t;
	struct m3config* c;
	std::string data = "";
	XTS_ASSERT_EQUALS(0, m3conf_tokenize(data.c_str(), data.size(), &t));
	c = m3conf_parse(t);

	m3conf_free(c);
	m3conf_free_tokens(t);
}

XTS_TEST(ParserTests, testBigValid) {
	struct m3conf_token* t;
	struct m3config* c;
	std::string data = "a=1;b=\"3\";z{y{x{c=\"a\";w{d=0;}e=-1;}v{}}f=\"qq\";}";
	XTS_ASSERT_EQUALS(0, m3conf_tokenize(data.c_str(), data.size(), &t));
	c = m3conf_parse(t);

	m3conf_free(c);
	m3conf_free_tokens(t);
}

XTS_TEST(ParserTests, testSimpleAssignment) {
	struct m3conf_token* t;
	struct m3config* c;
	std::string data = "a=1;";
	XTS_ASSERT_EQUALS(0, m3conf_tokenize(data.c_str(), data.size(), &t));
	c = m3conf_parse(t);

	m3conf_free(c);
	m3conf_free_tokens(t);
}

XTS_TEST(ParserTests, testNullDestroy) {
	m3conf_free(NULL);
}
