#include <m3conf/m3config.h>
#include <m3conf/parser.h>
#include <m3conf/tokenizer.h>
#include <string.h>

int main(void) {
	const char* str = "me {\n"
	                  "  you.z = \"foo\";\n"
						   "  you {\n"
							"    _x = 3;\n"
							"    z = \"qq\";\n"
							"  }\n"
							"  y_bleh = 4;\n"
							"}\n"
							"me.you.z = \"bleh\";\n";
	struct Token* t;
	struct Token* c;
	struct m3config* conf;
	size_t ret = tokenize(str, strlen(str), &t);
	if (ret != 0) {
		size_t i;
		ret--;
		printf("Error while tokenizing at offset %d.\n%s\n", ret, str);
		for (i = 0; i < ret; i++) {
			putchar(' ');
		}
		puts("^");
		return 1;
	} 
	c = t;
	while (c) {
		printf("%8s -> [%s]", TOK_NAMES[c->type], c->value);
		printf("\n");
		c = c->next;
	}
	conf = parse(t);
	free_tokens(t);

	printf("me.you.z = %s\n", m3conf_get_str(conf, "me.you.z", "NULL"));
	printf("me.you._x = %d\n", m3conf_get_int(conf, "me.you._x", 0));
	printf("me.y_bleh = %d\n", m3conf_get_int(conf, "me.y_bleh", 0));

	m3conf_free(conf);
	return 0;
}
