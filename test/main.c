#include <tokenizer.h>

int main(void) {
	struct Token* t = tokenize("!sectionfoo", 11);
	struct Token* c = t;
	while (c) {
		printf("%d - ", c->type);
		fwrite(c->value, c->length * sizeof(char), 1, stdout);
		printf("\n");
		c = c->next;
	}
	free_tokens(t);
	return 0;
}
