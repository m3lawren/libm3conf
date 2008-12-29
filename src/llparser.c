#include <parser.h>

#include <assert.h>
#include <stdlib.h>
#include <tokenizer.h>

enum state_t {
	ST_S,
	ST_STMTS,
	ST_VALUE
};

struct StateStack { 
	enum state_t* s;
	size_t        n;
	size_t        c;
};

static struct StateStack* sstack_create() {
	struct StateStack* ss = malloc(sizeof(struct StateStack));
	ss->s = NULL;
	ss->n = ss->c = 0;

	return ss;
}

static void sstack_destroy(struct StateStack* ss) {
	free(ss->s);
	ss->s = NULL;
	ss->n = ss->c = 0;
	free(ss);
}

static void sstack_push(struct StateStack* ss, enum state_t s) {
	if (ss->n == ss->c) {
		if (ss->c == 0) {
			ss->c = 1;
		} else {
			ss->c *= 2;
		}
		ss->s = realloc(ss->s, ss->c * sizeof(enum state_t));
		assert(ss->s);
	}
	ss->s[ss->n++] = s;
}

static enum state_t sstack_pop(struct StateStack* ss) {
	assert(ss->n);
	return ss->s[--ss->n];
}

void parse(struct Token* t) {
	struct StateStack* ss = sstack_create();
	sstack_push(ss, ST_S);
	sstack_pop(ss);
	sstack_destroy(ss);
	(void)t;
}
