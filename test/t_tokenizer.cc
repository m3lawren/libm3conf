#include <m3conf/tokenizer.h>
#include <test/XTest.h>

XTS_TEST(TokenizerTests, testSingleChars) {
	struct Token* t;
	struct Token* c;
	std::string data = "{}=;";
	XTS_ASSERT_EQUALS(0, tokenize(data.c_str(), data.length(), &t));
	c = t;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_LB, c->type);
	XTS_ASSERT_EQUALS(std::string("{"), c->value);
	c = c->next;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_RB, c->type);
	XTS_ASSERT_EQUALS(std::string("}"), c->value);
	c = c->next;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_EQ, c->type);
	XTS_ASSERT_EQUALS(std::string("="), c->value);
	c = c->next;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_SEMI, c->type);
	XTS_ASSERT_EQUALS(std::string(";"), c->value);
	c = c->next;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_EOF, c->type);
	XTS_ASSERT_EQUALS(std::string(""), c->value);
	XTS_ASSERT_EQUALS(NULL, c->next);

	free_tokens(t);
}

XTS_TEST(TokenizerTests, testValidInt) {
	struct Token* t;
	struct Token* c;
	std::string data = "01234-44";
	XTS_ASSERT_EQUALS(0, tokenize(data.c_str(), data.length(), &t));
	c = t;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_INT, c->type);
	XTS_ASSERT_EQUALS(std::string("0"), c->value);
	c = c->next;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_INT, c->type);
	XTS_ASSERT_EQUALS(std::string("1234"), c->value);
	c = c->next;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_INT, c->type);
	XTS_ASSERT_EQUALS(std::string("-44"), c->value);
	c = c->next;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_EOF, c->type);
	XTS_ASSERT_EQUALS(std::string(""), c->value);
	XTS_ASSERT_EQUALS(NULL, c->next);

	free_tokens(t);
}

XTS_TEST(TokenizerTests, testValidStr) {
	struct Token* t;
	struct Token* c;
	std::string data = "\"aoeu\"\"\\\"quote\"\"\\\\\\\"\"";
	XTS_ASSERT_EQUALS(0, tokenize(data.c_str(), data.length(), &t));
	c = t;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_STR, c->type);
	XTS_ASSERT_EQUALS(std::string("aoeu"), c->value);
	c = c->next;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_STR, c->type);
	XTS_ASSERT_EQUALS(std::string("\"quote"), c->value);
	c = c->next;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_STR, c->type);
	XTS_ASSERT_EQUALS(std::string("\\\""), c->value);
	c = c->next;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_EOF, c->type);
	XTS_ASSERT_EQUALS(std::string(""), c->value);
	XTS_ASSERT_EQUALS(NULL, c->next);

	free_tokens(t);
}

XTS_TEST(TokenizerTests, testValidId) {
	struct Token* t;
	struct Token* c;
	std::string data = "aoeu AOEU a1 section _ _._";
	XTS_ASSERT_EQUALS(0, tokenize(data.c_str(), data.length(), &t));
	c = t;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_ID, c->type);
	XTS_ASSERT_EQUALS(std::string("aoeu"), c->value);
	c = c->next;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_ID, c->type);
	XTS_ASSERT_EQUALS(std::string("AOEU"), c->value);
	c = c->next;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_ID, c->type);
	XTS_ASSERT_EQUALS(std::string("a1"), c->value);
	c = c->next;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_ID, c->type);
	XTS_ASSERT_EQUALS(std::string("section"), c->value);
	c = c->next;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_ID, c->type);
	XTS_ASSERT_EQUALS(std::string("_"), c->value);
	c = c->next;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_ID, c->type);
	XTS_ASSERT_EQUALS(std::string("_._"), c->value);
	c = c->next;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_EOF, c->type);
	XTS_ASSERT_EQUALS(std::string(""), c->value);
	XTS_ASSERT_EQUALS(NULL, c->next);

	free_tokens(t);
}

XTS_TEST(TokenizerTests, testValidWhiteSpace) {
	struct Token* t;
	struct Token* c;
	std::string data = " \t\n\r";
	XTS_ASSERT_EQUALS(0, tokenize(data.c_str(), data.length(), &t));
	c = t;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_EOF, c->type);
	XTS_ASSERT_EQUALS(std::string(""), c->value);
	XTS_ASSERT_EQUALS(NULL, c->next);

	free_tokens(t);
}

XTS_TEST(TokenizerTests, testValidComment) {
	struct Token* t;
	struct Token* c;
	std::string data = "#comment \t comment\r\n#comment";
	XTS_ASSERT_EQUALS(0, tokenize(data.c_str(), data.length(), &t));
	c = t;
	XTS_ASSERT_DIFFERS(NULL, c);
	XTS_ASSERT_EQUALS(TOK_EOF, c->type);
	XTS_ASSERT_EQUALS(std::string(""), c->value);
	XTS_ASSERT_EQUALS(NULL, c->next);

	free_tokens(t);
}
