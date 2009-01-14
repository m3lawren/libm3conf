#ifndef _PARSER_H_
#define _PARSER_H_

#ifdef __cplusplus
extern "C" {
#endif

struct m3conf_token;
struct m3config;

struct m3config* m3conf_parse(struct m3conf_token*);

#ifdef __cplusplus
}
#endif

#endif
