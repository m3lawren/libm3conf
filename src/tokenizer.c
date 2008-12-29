#include <tokenizer.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

enum state_t {
	ST_INIT,
	ST_ID,
	ST_STRCONT,
	ST_STR,
	ST_NEG,
	ST_INT,
	ST_WS,
	ST_COM
};

const char* const ALPHA = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* const NUM   = "0123456789";
const char* const WS    = " \t\r\n";

const char* const SECTION = "section";

const char* const TOK_NAMES[] = {
	"TOK_ID",
	"TOK_STR",
	"TOK_INT",
	"TOK_LB",
	"TOK_RB",
	"TOK_EQ",
	"TOK_SEMI",
	"TOK_SEC"
};

struct Token* create_token(enum token_t t, const char* s, size_t n) {
	struct Token* tok = NULL;
	
	tok = malloc(sizeof(struct Token));
	tok->next = NULL;
	tok->value = malloc((n + 1) * sizeof(char));
	tok->length = n;
	tok->type = t;
	
	memcpy(tok->value, s, n * sizeof(char));
	tok->value[n] = '\0';

	return tok;
}

size_t tokenize(const char* s, size_t n, struct Token** ret) {
	enum state_t state = ST_INIT;
	const char* end = s + n;
	const char* start = s;
	struct Token* head = NULL;
	struct Token* tail = NULL;
	struct Token* t = NULL;

	while (s < end) {
		t = NULL;
		switch (state) {
			case ST_INIT:
				start = s;
				if (strchr(ALPHA, *s)) {
					state = ST_ID;
				} else if (*s == '"') {
					state = ST_STRCONT;
				} else if (*s == '0') {
					t = create_token(TOK_INT, start, 1);
				} else if (strchr(NUM, *s)) {
					state = ST_INT;
				} else if (*s == '-') {
					state = ST_NEG;
				} else if (*s == '{') {
					t = create_token(TOK_LB, start, 1);
				} else if (*s == '}') {
					t = create_token(TOK_RB, start, 1);
				} else if (*s == '=') {
					t = create_token(TOK_EQ, start, 1);
				} else if (*s == ';') {
					t = create_token(TOK_SEMI, start, 1);
				} else if (strchr(WS, *s)) {
					state = ST_WS;
				} else if (*s == '#') {
					state = ST_COM;
				} else {
					goto fail;
				}
				s++;
				break;
			case ST_ID:
				if (strchr(ALPHA, *s) || strchr(NUM, *s)) {
					s++;
				} else {
					t = create_token(TOK_ID, start, (s - start));
					if (strcmp(t->value, SECTION) == 0) {
						t->type = TOK_SEC;
					}
					state = ST_INIT;
				}
				break;
			case ST_STRCONT:
				if (*s == '"') {
					state = ST_STR;
				} 
				s++;
				break;
			case ST_STR:
				if (*s == '"') {
					state = ST_STRCONT;
					s++;
				} else {
					t = create_token(TOK_STR, start, (s - start));
					state = ST_INIT;
				}
				break;
			case ST_NEG:
				if (strchr(NUM, *s) && *s != '0') {
					state = ST_INT;
					s++;
				} else {
					goto fail;
				}
				break;
			case ST_INT:
				if (strchr(NUM, *s)) {
					s++;
				} else {
					t = create_token(TOK_INT, start, (s - start));
					state = ST_INIT;
				}
				break;
			case ST_WS:
				if (strchr(WS, *s)) {
					s++;
				} else {
					state = ST_INIT;
				}
				break;
			case ST_COM:
				if (strchr("\r\n", *s)) {
					state = ST_INIT;
				}
				s++;
				break;
			default:
				goto fail;
		}
		if (t != NULL) {
			if (head == NULL) {
				head = tail = t;
			} else {
				tail->next = t;
				tail = t;
			}
		}
	}

	t = NULL;
	switch (state) {
		case ST_ID:
			t = create_token(TOK_ID, start, end - start);
			if (strcmp(t->value, SECTION) == 0) {
				t->type = TOK_SEC;
			}
			break;
		case ST_STR:
			t = create_token(TOK_STR, start, end - start);
			break;
		case ST_INT:
			t = create_token(TOK_INT, start, end - start);
			break;
		case ST_WS:
		case ST_COM:
		case ST_INIT:
			break;
		default:
			goto fail;
	}
	if (t != NULL) {
		if (head == NULL) {
			head = tail = t;
		} else {
			tail->next = t;
			tail = t;
		}
	}

	*ret = head;
	return 0;
fail:
	free_tokens(head);
	return n - (end - start) + 1;
}

void free_tokens(struct Token* t) {
	struct Token* next = t;
	while (next) {
		t = next;
		next = t->next;
		free(t->value);
		free(t);
	}
}
