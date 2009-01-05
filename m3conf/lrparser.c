#include <m3conf/parser.h>

#include <config.h>

#include <assert.h>
#include <m3conf/m3config.h>
#include <m3conf/tokenizer.h>
#include <stdarg.h>
#include <stdlib.h>

/* 
 * Functions declared in m3config.c that are 'private', but needed to generate
 * config structure while parsing due to our lack of knowledege about struct
 * m3config.
 */
struct m3config* parse_init();
void             parse_destroy(struct m3config*);
void             parse_finalize(struct m3config*);
void             parse_enter_section(struct m3config*, const char*);
void             parse_leave_section(struct m3config*);
void             parse_add_val(struct m3config*, const char*, const char*);

const char* REDUCTIONS[] = {
	"S     -> STMTS",
	"STMTS -> STMTS id STMT",
	"STMTS ->",
	"STMT  -> eq VALUE semi",
	"STMT  -> lb STMTS rb",
	"VALUE -> int", 
	"VALUE -> str"
};

struct LRStack {
	int* d;
	size_t c;
	size_t n;
};

static struct LRStack* lrs_create() {
	struct LRStack* s = malloc(sizeof(struct LRStack));
	s->d = NULL;
	s->c = s->n = 0;

	return s;
}

static void lrs_destroy(struct LRStack* s) {
	free(s->d);
	free(s);
}

static void lrs_push(struct LRStack* s, int v) {
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

static int lrs_peek(const struct LRStack* s) {
	return s->d[s->n - 1];
}

static void lrs_pop(struct LRStack* s) {
	s->n--;
}

static int lrs_empty(const struct LRStack* s) {
	return s->n == 0;
}

static int action_reduce(int s, enum token_t t) {
	switch (s) {
		case 0:
			switch (t) {
				case TOK_EOF:
				case TOK_ID:
					return 2;
				default:
					return -1;
			}
		case 3:
			switch (t) {
				case TOK_EOF:
				case TOK_RB:
				case TOK_ID:
					return 1;
				default:
					return -1;
			}
		case 5:
			switch (t) {
				case TOK_RB:
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
				case TOK_RB:
				case TOK_ID:
					return 3;
				default:
					return -1;
			}
		case 11:
			switch (t) {
				case TOK_EOF:
				case TOK_RB:
				case TOK_ID:
					return 4;
				default:
					return -1;
			}
		default:
			return -1;
	}
}

static int action_shift(int s, enum token_t t) {
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

static int action_accept(int s, enum token_t t) {
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

static int action_shift_reduction(int s, int r) {
	switch (s) {
		case 0:
			switch (r) {
				case 1:
				case 2:
					return 1;
				default:
					return -1;
			}
		case 2:
			switch (r) {
				case 3:
				case 4:
					return 3;
				default:
					return -1;
			}
		case 4:
			switch (r) {
				case 5:
				case 6:
					return 6;
				default:
					return -1;
			}
		case 5:
			switch (r) {
				case 1:
				case 2:
					return 9;
				default:
					return -1;
			}
		default:
			return -1;
	}
}

struct m3config* parse(struct Token* t) {
	struct LRStack* s = lrs_create();
	const char* lastid = NULL;
	struct m3config* c = parse_init();
	lrs_push(s, 0);
	while (1) {
		int x;
		if ((x = action_shift(lrs_peek(s), t->type)) != -1) {
			if (t->type == TOK_STR || t->type == TOK_INT) {
				parse_add_val(c, lastid, t->value);
			} else if (t->type == TOK_ID) {
				lastid = t->value;
			} else if (t->type == TOK_LB) {
				parse_enter_section(c, lastid);
			} else if (t->type == TOK_RB) {
				parse_leave_section(c);
			} else if (t->type == TOK_EQ) {
			}
			lrs_push(s, x);
			t = t->next;
			assert(t != NULL);
		} else if ((x = action_reduce(lrs_peek(s), t->type)) != -1) {
			switch (x) {
				case 1:
				case 3:
				case 4:
					lrs_pop(s);
					assert(!lrs_empty(s));
					lrs_pop(s);
					assert(!lrs_empty(s));
				case 0:
				case 5:
				case 6:
					lrs_pop(s);
					assert(!lrs_empty(s));
				case 2:
					break;
				default:
					assert(1 == 0);
			}
			x = action_shift_reduction(lrs_peek(s), x);
			assert(x != -1);
			lrs_push(s, x);
		} else if (action_accept(lrs_peek(s), t->type)) {
			break;
		} else {
			assert(0 == 1);
		}
	}
	lrs_destroy(s);
	parse_finalize(c);
	return c;
}
