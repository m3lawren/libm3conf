#include <m3conf/m3conf_tokenizer.h>
#include <test/XTest.h>

#define CHECK_TOKEN(toktype, val) XTS_ASSERT_EQUALS(false, NULL == c); XTS_ASSERT_EQUALS(toktype, c->type); XTS_ASSERT_EQUALS(std::string(val), c->value); c = c->next;
#define CHECK_EOF() CHECK_TOKEN(TOK_EOF, ""); XTS_ASSERT_EQUALS(true, NULL == c);

XTS_TEST(TokenizerTests, testSingleChars) {
	struct Token* t;
	struct Token* c;
	std::string data = "{}=;";
	XTS_ASSERT_EQUALS(0, tokenize(data.c_str(), data.length(), &t));
	c = t;
	CHECK_TOKEN(TOK_LB, "{");
	CHECK_TOKEN(TOK_RB, "}");
	CHECK_TOKEN(TOK_EQ, "=");
	CHECK_TOKEN(TOK_SEMI, ";");
	CHECK_EOF();

	free_tokens(t);
}

XTS_TEST(TokenizerTests, testValidInt) {
	struct Token* t;
	struct Token* c;
	std::string data = "01234-44";
	XTS_ASSERT_EQUALS(0, tokenize(data.c_str(), data.length(), &t));
	c = t;
	CHECK_TOKEN(TOK_INT, "0");
	CHECK_TOKEN(TOK_INT, "1234");
	CHECK_TOKEN(TOK_INT, "-44");
	CHECK_EOF();

	free_tokens(t);
}

XTS_TEST(TokenizerTests, testValidStr) {
	struct Token* t;
	struct Token* c;
	std::string data = "\"aoeu\"\"\\\"quote\"\"#\\\\\\\"\"";
	XTS_ASSERT_EQUALS(0, tokenize(data.c_str(), data.length(), &t));
	c = t;
	CHECK_TOKEN(TOK_STR, "aoeu");
	CHECK_TOKEN(TOK_STR, "\"quote");
	CHECK_TOKEN(TOK_STR, "#\\\"");
	CHECK_EOF();

	free_tokens(t);
}

XTS_TEST(TokenizerTests, testValidId) {
	struct Token* t;
	struct Token* c;
	std::string data = "aoeu AOEU a1 section _ _._";
	XTS_ASSERT_EQUALS(0, tokenize(data.c_str(), data.length(), &t));
	c = t;
	CHECK_TOKEN(TOK_ID, "aoeu");
	CHECK_TOKEN(TOK_ID, "AOEU");
	CHECK_TOKEN(TOK_ID, "a1");
	CHECK_TOKEN(TOK_ID, "section");
	CHECK_TOKEN(TOK_ID, "_");
	CHECK_TOKEN(TOK_ID, "_._");
	CHECK_EOF();

	free_tokens(t);
}

XTS_TEST(TokenizerTests, testValidWhiteSpace) {
	struct Token* t;
	struct Token* c;
	std::string data = " \t\n\r";
	XTS_ASSERT_EQUALS(0, tokenize(data.c_str(), data.length(), &t));
	c = t;
	CHECK_EOF();

	free_tokens(t);
}

XTS_TEST(TokenizerTests, testValidComment) {
	struct Token* t;
	struct Token* c;
	std::string data = "#comment \t comment\r\n#comment";
	XTS_ASSERT_EQUALS(0, tokenize(data.c_str(), data.length(), &t));
	c = t;
	CHECK_EOF();

	free_tokens(t);
}

XTS_TEST(TokenizerTests, testChaining) {
	struct Token* t;
	struct Token* c;
	std::string data = "a1{-2a.;=0Q\"3\"3a}";
	XTS_ASSERT_EQUALS(0, tokenize(data.c_str(), data.length(), &t));
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

	free_tokens(t);
}

XTS_TEST(TokenizerTests, testCommentChaining) {
	struct Token* t;
	struct Token* c;
	std::string data = "x#a\nx.#b\n34#c\n0#d\n1#e\n";
	XTS_ASSERT_EQUALS(0, tokenize(data.c_str(), data.length(), &t));
	c = t;
	CHECK_TOKEN(TOK_ID, "x");
	CHECK_TOKEN(TOK_ID, "x.");
	CHECK_TOKEN(TOK_INT, "34");
	CHECK_TOKEN(TOK_INT, "0");
	CHECK_TOKEN(TOK_INT, "1");
	CHECK_EOF();

	free_tokens(t);
}

XTS_TEST(TokenizerTests, testInvalidInt) {
	struct Token* t;
	std::string data;
	data = "-";
	XTS_ASSERT_EQUALS(2, tokenize(data.c_str(), data.length(), &t));
	data = "-a";
	XTS_ASSERT_EQUALS(2, tokenize(data.c_str(), data.length(), &t));
	data = "-0";
	XTS_ASSERT_EQUALS(2, tokenize(data.c_str(), data.length(), &t));
}

XTS_TEST(TokenizerTests, testInvalidStr) {
	struct Token* t;
	std::string data;
	data = "\"";
	XTS_ASSERT_EQUALS(2, tokenize(data.c_str(), data.length(), &t));
	data = "\"\\"; /* TODO: should this return 2? */
	XTS_ASSERT_EQUALS(3, tokenize(data.c_str(), data.length(), &t));
	data = "\"\\a"; 
	XTS_ASSERT_EQUALS(3, tokenize(data.c_str(), data.length(), &t));
	data = "\"\\\\\\\"";
	XTS_ASSERT_EQUALS(6, tokenize(data.c_str(), data.length(), &t));
}

XTS_TEST(TokenizerTests, testInvalidId) {
	struct Token* t;
	std::string data = ".";
	XTS_ASSERT_EQUALS(1, tokenize(data.c_str(), data.length(), &t));
}

XTS_TEST(TokenizerTests, testSingleToken) {
	struct Token* t;
	struct Token* c;
	std::string data = "a";
	XTS_ASSERT_EQUALS(0, tokenize(data.c_str(), data.length(), &t));
	c = t;
	CHECK_TOKEN(TOK_ID, "a");
	CHECK_EOF();

	free_tokens(t);
}
