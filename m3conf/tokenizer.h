#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

enum token_t {
	TOK_ID,
	TOK_STR,
	TOK_INT,
	TOK_LB,
	TOK_RB,
	TOK_EQ,
	TOK_SEMI,
	TOK_SEC,
	TOK_EOF
};

extern const char* const TOK_NAMES[];

struct Token {
	struct Token* next;
	char*         value;
	size_t        length;
	enum token_t  type;
};

size_t tokenize(const char*, size_t, struct Token**);

void free_tokens(struct Token*);

#ifdef __cplusplus
}
#endif

#endif
