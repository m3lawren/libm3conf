#include <m3conf/m3conf_tokenizer.h>
#include <test/XTest.h>

#define CHECK_TOKEN(toktype, val) XTS_ASSERT_EQUALS(false, NULL == c); XTS_ASSERT_EQUALS(toktype, c->type); XTS_ASSERT_EQUALS(std::string(val), c->value); c = c->next;
#define CHECK_EOF() CHECK_TOKEN(TOK_EOF, ""); XTS_ASSERT_EQUALS(true, NULL == c);

XTS_TEST(m3conf_tokenizerTests, testSingleChars) {
	struct m3conf_token* t;
	struct m3conf_token* c;
	std::string data = "{}=;";
	XTS_ASSERT_EQUALS(0, m3conf_tokenize(data.c_str(), data.length(), &t));
	c = t;
	CHECK_TOKEN(TOK_LB, "{");
	CHECK_TOKEN(TOK_RB, "}");
	CHECK_TOKEN(TOK_EQ, "=");
	CHECK_TOKEN(TOK_SEMI, ";");
	CHECK_EOF();

	m3conf_free_tokens(t);
}

XTS_TEST(m3conf_tokenizerTests, testValidInt) {
	struct m3conf_token* t;
	struct m3conf_token* c;
	std::string data = "01234-44";
	XTS_ASSERT_EQUALS(0, m3conf_tokenize(data.c_str(), data.length(), &t));
	c = t;
	CHECK_TOKEN(TOK_INT, "0");
	CHECK_TOKEN(TOK_INT, "1234");
	CHECK_TOKEN(TOK_INT, "-44");
	CHECK_EOF();

	m3conf_free_tokens(t);
}

XTS_TEST(m3conf_tokenizerTests, testValidStr) {
	struct m3conf_token* t;
	struct m3conf_token* c;
	std::string data = "\"aoeu\"\"\\\"quote\"\"#\\\\\\\"\"";
	XTS_ASSERT_EQUALS(0, m3conf_tokenize(data.c_str(), data.length(), &t));
	c = t;
	CHECK_TOKEN(TOK_STR, "aoeu");
	CHECK_TOKEN(TOK_STR, "\"quote");
	CHECK_TOKEN(TOK_STR, "#\\\"");
	CHECK_EOF();

	m3conf_free_tokens(t);
}

XTS_TEST(m3conf_tokenizerTests, testValidId) {
	struct m3conf_token* t;
	struct m3conf_token* c;
	std::string data = "aoeu AOEU a1 section _ _._";
	XTS_ASSERT_EQUALS(0, m3conf_tokenize(data.c_str(), data.length(), &t));
	c = t;
	CHECK_TOKEN(TOK_ID, "aoeu");
	CHECK_TOKEN(TOK_ID, "AOEU");
	CHECK_TOKEN(TOK_ID, "a1");
	CHECK_TOKEN(TOK_ID, "section");
	CHECK_TOKEN(TOK_ID, "_");
	CHECK_TOKEN(TOK_ID, "_._");
	CHECK_EOF();

	m3conf_free_tokens(t);
}

XTS_TEST(m3conf_tokenizerTests, testValidWhiteSpace) {
	struct m3conf_token* t;
	struct m3conf_token* c;
	std::string data = " \t\n\r";
	XTS_ASSERT_EQUALS(0, m3conf_tokenize(data.c_str(), data.length(), &t));
	c = t;
	CHECK_EOF();

	m3conf_free_tokens(t);
}

XTS_TEST(m3conf_tokenizerTests, testValidComment) {
	struct m3conf_token* t;
	struct m3conf_token* c;
	std::string data = "#comment \t comment\r\n#comment";
	XTS_ASSERT_EQUALS(0, m3conf_tokenize(data.c_str(), data.length(), &t));
	c = t;
	CHECK_EOF();

	m3conf_free_tokens(t);
}

XTS_TEST(m3conf_tokenizerTests, testChaining) {
	struct m3conf_token* t;
	struct m3conf_token* c;
	std::string data = "a1{-2a.;=0Q\"3\"3a}";
	XTS_ASSERT_EQUALS(0, m3conf_tokenize(data.c_str(), data.length(), &t));
	c = t;
	CHECK_TOKEN(TOK_ID, "a1");
	CHECK_TOKEN(TOK_LB, "{");
	CHECK_TOKEN(TOK_INT, "-2");
	CHECK_TOKEN(TOK_ID, "a.");
	CHECK_TOKEN(TOK_SEMI, ";");
	CHECK_TOKEN(TOK_EQ, "=");
	CHECK_TOKEN(TOK_INT, "0");
	CHECK_TOKEN(TOK_ID, "Q");
	CHECK_TOKEN(TOK_STR, "3");
	CHECK_TOKEN(TOK_INT, "3");
	CHECK_TOKEN(TOK_ID, "a");
	CHECK_TOKEN(TOK_RB, "}");
	CHECK_EOF();

	m3conf_free_tokens(t);
}

XTS_TEST(m3conf_tokenizerTests, testCommentChaining) {
	struct m3conf_token* t;
	struct m3conf_token* c;
	std::string data = "x#a\nx.#b\n34#c\n0#d\n1#e\n";
	XTS_ASSERT_EQUALS(0, m3conf_tokenize(data.c_str(), data.length(), &t));
	c = t;
	CHECK_TOKEN(TOK_ID, "x");
	CHECK_TOKEN(TOK_ID, "x.");
	CHECK_TOKEN(TOK_INT, "34");
	CHECK_TOKEN(TOK_INT, "0");
	CHECK_TOKEN(TOK_INT, "1");
	CHECK_EOF();

	m3conf_free_tokens(t);
}

XTS_TEST(m3conf_tokenizerTests, testInvalidInt) {
	struct m3conf_token* t;
	std::string data;
	data = "-";
	XTS_ASSERT_EQUALS(2, m3conf_tokenize(data.c_str(), data.length(), &t));
	data = "-a";
	XTS_ASSERT_EQUALS(2, m3conf_tokenize(data.c_str(), data.length(), &t));
	data = "-0";
	XTS_ASSERT_EQUALS(2, m3conf_tokenize(data.c_str(), data.length(), &t));
}

XTS_TEST(m3conf_tokenizerTests, testInvalidStr) {
	struct m3conf_token* t;
	std::string data;
	data = "\"";
	XTS_ASSERT_EQUALS(2, m3conf_tokenize(data.c_str(), data.length(), &t));
	data = "\"\\"; /* TODO: should this return 2? */
	XTS_ASSERT_EQUALS(3, m3conf_tokenize(data.c_str(), data.length(), &t));
	data = "\"\\a"; 
	XTS_ASSERT_EQUALS(3, m3conf_tokenize(data.c_str(), data.length(), &t));
	data = "\"\\\\\\\"";
	XTS_ASSERT_EQUALS(6, m3conf_tokenize(data.c_str(), data.length(), &t));
}

XTS_TEST(m3conf_tokenizerTests, testInvalidId) {
	struct m3conf_token* t;
	std::string data = ".";
	XTS_ASSERT_EQUALS(1, m3conf_tokenize(data.c_str(), data.length(), &t));
}

XTS_TEST(m3conf_tokenizerTests, testSinglem3conf_token) {
	struct m3conf_token* t;
	struct m3conf_token* c;
	std::string data = "a";
	XTS_ASSERT_EQUALS(0, m3conf_tokenize(data.c_str(), data.length(), &t));
	c = t;
	CHECK_TOKEN(TOK_ID, "a");
	CHECK_EOF();

	m3conf_free_tokens(t);
}
