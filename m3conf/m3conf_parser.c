#include <m3conf/m3conf_parser.h>

#include <assert.h>
#include <m3conf/m3conf_tokenizer.h>
#include <stdlib.h>

enum m3conf_parse_state {
	PARSE_COMPLETE,
	PARSE_INIT,
	PARSE_POSTEQ,
	PARSE_POSTID,
	PARSE_POSTVALUE
};

struct m3config* m3conf_parse(struct m3conf_token* t) {
	enum m3conf_parse_state state = PARSE_INIT;

	struct m3conf_token* cur_id = NULL;

	while (t) {
		switch (state) {
			case PARSE_INIT:
				switch (t->type) {
					case TOK_ID:
						cur_id = t;
						state = PARSE_POSTID;
						break;
					case TOK_EOF:
						state = PARSE_COMPLETE;
						break;
					case TOK_RB:
						printf("Leaving section\n");
						/* TODO */
						state = PARSE_INIT;
						break;
					default:
						/* TODO */
						assert(1 == 0);
				}
				break;
			case PARSE_POSTID:
				switch (t->type) {
					case TOK_EQ:
						state = PARSE_POSTEQ;
						break;
					case TOK_LB:
						printf("Entering section %s\n", cur_id->value);
						/* TODO */
						state = PARSE_INIT;
						break;
					default:	
						/* TODO */
						assert(1 == 0);
				}
				break;
			case PARSE_POSTEQ:
				switch (t->type) {
					case TOK_INT:
					case TOK_STR:
						/* TODO */
						printf("%s = %s\n", cur_id->value, t->value);
						state = PARSE_POSTVALUE;
						break;
					default:
						/* TODO */
						assert(1 == 0);
				}
				break;
			case PARSE_POSTVALUE:
				switch (t->type) {
					case TOK_SEMI:
						state = PARSE_INIT;
						break;
					default:
						/* TODO */
						assert(1 == 0);
				}
				break;
			default:
				/* TODO */
				assert(1 == 0);
		}
		t = t->next;
	}

	return NULL;
}
