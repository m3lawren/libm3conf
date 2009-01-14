#ifndef _TOKENIZER_H_
#define _TOKENIZER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

enum m3conf_token_t {
	TOK_ID,
	TOK_STR,
	TOK_INT,
	TOK_LB,
	TOK_RB,
	TOK_EQ,
	TOK_SEMI,
	TOK_EOF
};

extern const char* const M3CONF_TOK_NAMES[];

struct m3conf_token {
	struct m3conf_token* next;
	char*         			value;
	size_t        			length;
	enum m3conf_token_t  type;
};

size_t m3conf_tokenize(const char*, size_t, struct m3conf_token**);

void m3conf_free_tokens(struct m3conf_token*);

#ifdef __cplusplus
}
#endif

#endif
