#include <m3conf/m3conf_parser.h>

#include <config.h>

#include <assert.h>
#include <m3conf/m3config.h>
#include <m3conf/m3conf_tokenizer.h>
#include <stdarg.h>
#include <stdlib.h>

/* 
 * Functions declared in m3config.c that are 'private', but needed to generate
 * config structure while parsing due to our lack of knowledege about struct
 * m3config.
 */
struct m3config* m3conf_parse_init();
void             m3conf_parse_destroy(struct m3config*);
void             m3conf_parse_finalize(struct m3config*);
void             m3conf_parse_enter_section(struct m3config*, const char*);
void             m3conf_parse_leave_section(struct m3config*);
void             m3conf_parse_add_val(struct m3config*, const char*, const char*);

const char* REDUCTIONS[] = {
	"S     -> STMTS",
	"STMTS -> STMTS id STMT",
	"STMTS ->",
	"STMT  -> eq VALUE semi",
	"STMT  -> lb STMTS rb",
	"VALUE -> int", 
	"VALUE -> str"
};

struct m3conf_lrstack {
	int* d;
	size_t c;
	size_t n;
};

static struct m3conf_lrstack* m3conf_lrs_create() {
	struct m3conf_lrstack* s = malloc(sizeof(struct m3conf_lrstack));
	s->d = NULL;
	s->c = s->n = 0;

	return s;
}

static void m3conf_lrs_destroy(struct m3conf_lrstack* s) {
	free(s->d);
	free(s);
}

static void m3conf_lrs_push(struct m3conf_lrstack* s, int v) {
	if (s->c == s->n) {
		if (s->c) {
			s->c *= 2;
		} else {
			s->c = 1;
		}

		s->d = realloc(s->d, s->c * sizeof(int));
	}
	s->d[s->n++] = v;
}

static int m3conf_lrs_peek(const struct m3conf_lrstack* s) {
	return s->d[s->n - 1];
}

static void m3conf_lrs_pop(struct m3conf_lrstack* s) {
	s->n--;
}

static int m3conf_lrs_empty(const struct m3conf_lrstack* s) {
	return s->n == 0;
}

static int m3conf_action_reduce(int s, enum m3conf_token_t t) {
	switch (s) {
		case 0:
			switch (t) {
				case TOK_EOF:
					return 2;
				case TOK_ID:
					return 2;
				default:
					return -1;
			}
		case 3:
			switch (t) {
				case TOK_EOF:
					return 1;
				case TOK_RB:
					return 1;
				case TOK_ID:
					return 1;
				default:
					return -1;
			}
		case 5:
			switch (t) {
				case TOK_RB:
					return 2;
				case TOK_ID:
					return 2;
				default:
					return -1;
			}
		case 7:
			switch (t) {
				case TOK_SEMI:
					return 5;
				default:
					return -1;
			}
		case 8:
			switch (t) {
				case TOK_SEMI:
					return 6;
				default:
					return -1;
			}
		case 10:
			switch (t) {
				case TOK_EOF:
					return 3;
				case TOK_RB:
					return 3;
				case TOK_ID:
					return 3;
				default:
					return -1;
			}
		case 11:
			switch (t) {
				case TOK_EOF:
					return 4;
				case TOK_RB:
					return 4;
				case TOK_ID:
					return 4;
				default:
					return -1;
			}
		default:
			return -1;
	}
}

static int m3conf_action_shift(int s, enum m3conf_token_t t) {
	switch (s) {
		case 1:
			switch (t) {
				case TOK_ID:
					return 2;
				default:
					return -1;
			}
		case 2:
			switch (t) {
				case TOK_EQ:
					return 4;
				case TOK_LB:
					return 5;
				default:
					return -1;
			}
		case 4:
			switch (t) {
				case TOK_INT:
					return 7;
				case TOK_STR:
					return 8;
				default:
					return -1;
			}
		case 6:
			switch (t) {
				case TOK_SEMI:
					return 10;
				default:
					return -1;
			}
		case 9:
			switch (t) {
				case TOK_RB:
					return 11;
				case TOK_ID:
					return 2;
				default:
					return -1;
			}
		default:
			return -1;
	}
}

static int m3conf_action_accept(int s, enum m3conf_token_t t) {
	switch (s) {
		case 1:
			switch (t) {
				case TOK_EOF:
					return 1;
				default:
					return 0;
			}
		default:
			return 0;
	}
}

static int m3conf_action_shift_reduction(int s, int r) {
	switch (s) {
		case 0:
			switch (r) {
				case 1:
					return 1;
				case 2:
					return 1;
				default:
					return -1;
			}
		case 2:
			switch (r) {
				case 3:
					return 3;
				case 4:
					return 3;
				default:
					return -1;
			}
		case 4:
			switch (r) {
				case 5:
					return 6;
				case 6:
					return 6;
				default:
					return -1;
			}
		case 5:
			switch (r) {
				case 1:
					return 9;
				case 2:
					return 9;
				default:
					return -1;
			}
		default:
			return -1;
	}
}

struct m3config* m3conf_parse(struct m3conf_token* t) {
	struct m3conf_lrstack* s = m3conf_lrs_create();
	const char* lastid = NULL;
	struct m3config* c = m3conf_parse_init();
	m3conf_lrs_push(s, 0);
	while (1) {
		int x;
		if ((x = m3conf_action_shift(m3conf_lrs_peek(s), t->type)) != -1) {
			if (t->type == TOK_STR || t->type == TOK_INT) {
				m3conf_parse_add_val(c, lastid, t->value);
			} else if (t->type == TOK_ID) {
				lastid = t->value;
			} else if (t->type == TOK_LB) {
				m3conf_parse_enter_section(c, lastid);
			} else if (t->type == TOK_RB) {
				m3conf_parse_leave_section(c);
			} else if (t->type == TOK_EQ) {
			}
			m3conf_lrs_push(s, x);
			t = t->next;
			assert(t != NULL);
		} else if ((x = m3conf_action_reduce(m3conf_lrs_peek(s), t->type)) != -1) {
			switch (x) {
				case 1:
				case 3:
				case 4:
					m3conf_lrs_pop(s);
					assert(!m3conf_lrs_empty(s));
					m3conf_lrs_pop(s);
					assert(!m3conf_lrs_empty(s));
				case 0:
				case 5:
				case 6:
					m3conf_lrs_pop(s);
					assert(!m3conf_lrs_empty(s));
				case 2:
					break;
				default:
					assert(1 == 0);
			}
			x = m3conf_action_shift_reduction(m3conf_lrs_peek(s), x);
			assert(x != -1);
			m3conf_lrs_push(s, x);
		} else if (m3conf_action_accept(m3conf_lrs_peek(s), t->type)) {
			break;
		} else {
			assert(0 == 1);
		}
	}
	m3conf_lrs_destroy(s);
	m3conf_parse_finalize(c);
	return c;
}
