#include <m3conf/m3conf_tokenizer.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

enum state_t {
	ST_INIT,
	ST_ID,
	ST_STRCONT,
	ST_STRESC,
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

const char* const M3CONF_TOK_NAMES[] = {
	"TOK_ID",
	"TOK_STR",
	"TOK_INT",
	"TOK_LB",
	"TOK_RB",
	"TOK_EQ",
	"TOK_SEMI",
	"TOK_EOF"
};

static size_t m3conf_clean_string(const char* s, size_t n, char** buf) {
	const char* end = s + n - 1;
	const char* r = s + 1;
	char* w;
	size_t sz = 0;

	while (r < end) {
		if (*r == '\\') {
			r++;
		}
		sz++;
		r++;
	}

	r = s + 1;
	*buf = malloc((sz + 1) * sizeof(char));
	w = *buf;
	while (r < end) {
		if (*r == '\\') {
			r++;
		}
		*w++ = *r++;
	}
	*w = '\0';
	return sz;
}

static struct m3conf_token* m3conf_create_token_impl(enum m3conf_token_t t) {
	struct m3conf_token* tok = NULL;

	tok = malloc(sizeof(struct m3conf_token));
	tok->next = NULL;
	tok->value = NULL;
	tok->length = 0;
	tok->type = t;

	return tok;
}

static struct m3conf_token* m3conf_create_string_token(const char* s, size_t n) {
	struct m3conf_token* tok = m3conf_create_token_impl(TOK_STR);
	tok->length = m3conf_clean_string(s, n, &tok->value);

	return tok;
}

static struct m3conf_token* m3conf_create_token(enum m3conf_token_t t, const char* s, size_t n) {
	struct m3conf_token* tok = m3conf_create_token_impl(t);

	tok->value = malloc((n + 1) * sizeof(char));

	memcpy(tok->value, s, n * sizeof(char));
	tok->value[n] = '\0';

	return tok;
}

size_t m3conf_tokenize(const char* s, size_t n, struct m3conf_token** ret) {
	enum state_t state = ST_INIT;
	const char* end = s + n;
	const char* start = s;
	struct m3conf_token* head = NULL;
	struct m3conf_token* tail = NULL;
	struct m3conf_token* t = NULL;

	while (s < end) {
		t = NULL;
		switch (state) {
			case ST_INIT:
				start = s;
				if (strchr(ALPHA, *s) || *s == '_') {
					state = ST_ID;
				} else if (*s == '"') {
					state = ST_STRCONT;
				} else if (*s == '0') {
					t = m3conf_create_token(TOK_INT, start, 1);
				} else if (strchr(NUM, *s)) {
					state = ST_INT;
				} else if (*s == '-') {
					state = ST_NEG;
				} else if (*s == '{') {
					t = m3conf_create_token(TOK_LB, start, 1);
				} else if (*s == '}') {
					t = m3conf_create_token(TOK_RB, start, 1);
				} else if (*s == '=') {
					t = m3conf_create_token(TOK_EQ, start, 1);
				} else if (*s == ';') {
					t = m3conf_create_token(TOK_SEMI, start, 1);
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
				if (strchr(ALPHA, *s) || strchr(NUM, *s) || *s == '_' || *s == '.') {
					s++;
				} else {
					t = m3conf_create_token(TOK_ID, start, (s - start));
					state = ST_INIT;
				}
				break;
			case ST_STRCONT:
				if (*s == '\\') {
					state = ST_STRESC;
				} else if (*s == '"') {
					state = ST_STR;
				}
				s++;
				break;
			case ST_STRESC:
				if (strchr("\\\"", *s)) {
					state = ST_STRCONT;
				} else {
					goto fail;
				}
				s++;
				break;
			case ST_STR:
				t = m3conf_create_string_token(start, (s - start));
				state = ST_INIT;
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
					t = m3conf_create_token(TOK_INT, start, (s - start));
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
			t = m3conf_create_token(TOK_ID, start, end - start);
			break;
		case ST_STR:
			t = m3conf_create_string_token(start, (end - start));
			break;
		case ST_INT:
			t = m3conf_create_token(TOK_INT, start, end - start);
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

	t = m3conf_create_token_impl(TOK_EOF);
	t->value = malloc(sizeof(char));
	t->value[0] = '\0';
	if (head == NULL) {
		head = tail = t;
	} else {
		tail->next = t;
		tail = t;
	}

	*ret = head;
	return 0;
fail:
	m3conf_free_tokens(head);
	return n - (end - s) + 1;
}

void m3conf_free_tokens(struct m3conf_token* t) {
	struct m3conf_token* next = t;
	while (next) {
		t = next;
		next = t->next;
		free(t->value);
		free(t);
	}
}
