#include <string.h>
#include <tokenizer.h>

int main(void) {
	const char* str = "section foo{bar=\"bleh\";}";
	struct Token* t;
	struct Token* c;
	size_t ret = tokenize(str, strlen(str), &t);
	if (ret != 0) {
		size_t i;
		ret--;
		printf("Error while tokenizing at offset%d.\n%s\n", ret, str);
		for (i = 0; i < ret; i++) {
			putchar(' ');
		}
		puts("^");
		return 1;
	} 
	c = t;
	while (c) {
		printf("%s - ", TOK_NAMES[c->type]);
		fwrite(c->value, c->length * sizeof(char), 1, stdout);
		printf("\n");
		c = c->next;
	}
	free_tokens(t);
	return 0;
}
