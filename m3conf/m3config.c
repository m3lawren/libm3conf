#include <m3conf/m3config.h>

struct m3config {
};

const char* m3conf_get_str(const struct m3config* c, const char* key, const char* def) {
	(void)c;
	(void)key;
	return def;
}

int m3conf_get_int(const struct m3config* c, const char* key, int def) {
	(void)c;
	(void)key;
	return def;
}
