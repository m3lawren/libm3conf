#include <m3conf/parser.h>
#include <m3conf/tokenizer.h>
#include <string.h>

int main(void) {
	const char* str = "section me{z=5;section you{x=3;}y=4;}";
	struct Token* t;
	struct Token* c;
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
	parse(t);
	free_tokens(t);
	return 0;
}
