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
	ST_SEC0, /* seen !        */
	ST_SEC1, /* seen !s       */
	ST_SEC2, /* seen !se      */
	ST_SEC3, /* seen !sec     */
	ST_SEC4, /* seen !sect    */
	ST_SEC5, /* seen !secti   */
	ST_SEC6, /* seen !sectio  */
	ST_WS,
	ST_COM
};

const char* ALPHA = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* NUM   = "0123456789";
const char* WS    = " \t\r\n";

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

struct Token* tokenize(const char* s, size_t n) {
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
				} else if (*s == '!') {
					state = ST_SEC0;
				} else if (strchr(WS, *s)) {
					state = ST_WS;
				} else if (*s == '#') {
					state = ST_COM;
				} else {
					/* TODO: asplode */
					assert(1 == 0);
				}
				s++;
				break;
			case ST_ID:
				if (strchr(ALPHA, *s) || strchr(NUM, *s)) {
					s++;
				} else {
					t = create_token(TOK_ID, start, (s - start));
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
					/* TODO: asplode */
					assert(1 == 0);
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
			case ST_SEC0:
				if (*s == 's') {
					state = ST_SEC1;
					s++;
				} else {
					/* TODO: asplode */
					assert(1 == 0);
				}
				break;
			case ST_SEC1:
				if (*s == 'e') {
					state = ST_SEC2;
					s++;
				} else {
					/* TODO: asplode */
					assert(1 == 0);
				}
				break;
			case ST_SEC2:
				if (*s == 'c') {
					state = ST_SEC3;
					s++;
				} else {
					/* TODO: asplode */
					assert(1 == 0);
				}
				break;
			case ST_SEC3:
				if (*s == 't') {
					state = ST_SEC4;
					s++;
				} else {
					/* TODO: asplode */
					assert(1 == 0);
				}
				break;
			case ST_SEC4:
				if (*s == 'i') {
					state = ST_SEC5;
					s++;
				} else {
					/* TODO: asplode */
					assert(1 == 0);
				}
				break;
			case ST_SEC5:
				if (*s == 'o') {
					state = ST_SEC6;
					s++;
				} else {
					/* TODO: asplode */
					assert(1 == 0);
				}
				break;
			case ST_SEC6:
				if (*s == 'n') {
					s++;
					t = create_token(TOK_SEC, start, (s - start));
					state = ST_INIT;
				} else {
					/* TODO: asplode */
					assert(1 == 0);
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
				/* TODO: asplode */
				assert(1 == 0);
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

	switch (state) {
		case ST_ID:
			t = create_token(TOK_ID, start, end - start);
			break;
		case ST_STR:
			t = create_token(TOK_STR, start, end - start);
			break;
		case ST_INT:
			t = create_token(TOK_INT, start, end - start);
			break;
		case ST_WS:
		case ST_COM:
			break;
		default:
			/* TODO: asplode */
			assert(1 == 0);
	}
	if (t != NULL) {
		if (head == NULL) {
			head = tail = t;
		} else {
			tail->next = t;
			tail = t;
		}
	}

	return head;
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
