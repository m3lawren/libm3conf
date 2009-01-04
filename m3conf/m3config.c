#include <m3conf/m3config.h>

#include <stdlib.h>
#include <string.h>

struct cnode {
	struct cnode* next;

	enum {
		CN_SECTION,
		CN_VALUE
	}             type;

	/* used for both CN_VALUE and CN_SECTION */
	char*         key;
	size_t        ksz;
	struct cnode* parent;

	/* only used for CN_VALUE */
	char*         val;

	/* only used for CN_SECTION */
	struct cnode* child;
};

struct m3config {
	struct cnode* data;
};

static const struct cnode* find_key(const struct cnode* start, const char* key) {
	size_t keyoff = 0;
	while (start) {
		if (start->type == CN_SECTION && strncmp(start->key, key + keyoff, start->ksz)) {
			if (start->child) {
				start = start->child;
				keyoff += start->ksz + 1;
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
				}
			}
		}
	}
	return NULL;
}

const char* m3conf_get_str(const struct m3config* c, const char* key, const char* def) {
	const struct cnode* n = find_key(c->data, key);
	if (!n) {
		return def;
	}
	return n->val;
}

int m3conf_get_int(const struct m3config* c, const char* key, int def) {
	const struct cnode* n = find_key(c->data, key);
	if (!n) {
		return def;
	}
	return atoi(n->val);
}
