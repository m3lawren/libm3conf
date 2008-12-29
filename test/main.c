#include <string.h>
#include <tokenizer.h>

int main(void) {
	const char* str = "section foo{bar=\"bleh\";}";
	struct Token* t = tokenize(str, strlen(str));
	struct Token* c = t;
	while (c) {
		printf("%s - ", TOK_NAMES[c->type]);
		fwrite(c->value, c->length * sizeof(char), 1, stdout);
		printf("\n");
		c = c->next;
	}
	free_tokens(t);
	return 0;
}
