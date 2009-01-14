#include <m3conf/m3config.h>

#include <stdlib.h>
#include <string.h>

struct m3conf_cnode {
	struct m3conf_cnode* next;

	enum {
		CN_SECTION,
		CN_VALUE
	}             type;

	/* used for both CN_VALUE and CN_SECTION */
	char*         key;
	size_t        ksz;
	struct m3conf_cnode* parent;

	/* only used for CN_VALUE */
	char*         val;

	/* only used for CN_SECTION */
	struct m3conf_cnode* child;
};

struct m3config {
	struct m3conf_cnode* data;

	/* used for the parsing functions */
	struct m3conf_cnode* parsecur;
};
	
static void m3conf_cnode_destroy(struct m3conf_cnode* n) {
	while (n) {
		struct m3conf_cnode* x = n;
		if (x->type == CN_SECTION && x->child) {
			m3conf_cnode_destroy(x->child);
		} else if (x->type == CN_VALUE && x->val) {
			free(x->val);
		}
		free(x->key);
		n = x->next;
		x->next = x->child = x->parent = NULL;
		x->key = x->val = NULL;
		x->ksz = 0;
		free(x);
	}
}

void m3conf_free(struct m3config* c) {
	if (!c) {
		return;
	}

	if (c->data) {
		m3conf_cnode_destroy(c->data);
	}

	/* c->parsecur should be pointing to part of c->data, so it's good */
	c->data = c->parsecur = NULL;

	free(c);
}

static const struct m3conf_cnode* m3conf_find_key(const struct m3conf_cnode* start, const char* key) {
	size_t keyoff = 0;
	while (start) {
		if (start->type == CN_SECTION && key[keyoff + start->ksz] == '.' && strncmp(start->key, key + keyoff, start->ksz) == 0) {
			if (start->child) {
				keyoff += start->ksz + 1;
				start = start->child;
			}
		} else if (start->type == CN_VALUE && strcmp(start->key, key + keyoff) == 0) {
			return start;
		} else {
			if (start->next) {
				start = start->next;
			} else {
				start = start->parent;
				if (start) {
					keyoff -= start->ksz + 1;
					start = start->next;
				}
			}
		}
	}
	return NULL;
}

const char* m3conf_get_str(const struct m3config* c, const char* key, const char* def) {
	const struct m3conf_cnode* n = m3conf_find_key(c->data, key);
	if (!n) {
		return def;
	}
	return n->val;
}

int m3conf_get_int(const struct m3config* c, const char* key, int def) {
	const struct m3conf_cnode* n = m3conf_find_key(c->data, key);
	if (!n) {
		return def;
	}
	return atoi(n->val);
}

/* 
 * Private functions used during parsing.
 */
struct m3config* m3conf_parse_init() {
	struct m3config* c = malloc(sizeof(struct m3config));
	c->data = c->parsecur = NULL;
	return c;
}

void m3conf_parse_destroy(struct m3config* c) {
	m3conf_free(c);
}

void m3conf_parse_finalize(struct m3config* c) {
	c->parsecur = NULL;
}

void m3conf_parse_enter_section(struct m3config* c, const char* name) {
	struct m3conf_cnode* n = malloc(sizeof(struct m3conf_cnode));
	n->type = CN_SECTION;
	n->ksz = strlen(name);
	n->key = malloc((n->ksz + 1) * sizeof(char));
	memcpy(n->key, name, n->ksz + 1);
	n->parent = c->parsecur;
	n->child = NULL;

	if (c->parsecur) {
		n->next = c->parsecur->child;
		c->parsecur->child = n;
	} else {
		/* we're at the top level */
		n->next = c->data;
		c->data = n;
	}
	c->parsecur = n;
}

void m3conf_parse_leave_section(struct m3config* c) {
	c->parsecur = c->parsecur->parent;
}

void m3conf_parse_add_val(struct m3config* c, const char* key, const char* val) {
	struct m3conf_cnode* n = malloc(sizeof(struct m3conf_cnode));
	n->type = CN_VALUE;
	n->ksz = strlen(key);
	n->key = malloc((n->ksz + 1) * sizeof(char));
	memcpy(n->key, key, n->ksz + 1);
	n->parent = c->parsecur;
	n->val = malloc((strlen(val) + 1) * sizeof(char));
	memcpy(n->val, val, strlen(val) + 1);

	if (c->parsecur) {
		n->next = c->parsecur->child;
		c->parsecur->child = n;
	} else {
		n->next = c->data;
		c->data = n;
	}
}
