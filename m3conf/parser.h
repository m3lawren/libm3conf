#ifndef _PARSER_H_
#define _PARSER_H_

#ifdef __cplusplus
extern "C" {
#endif

struct Token;
struct m3config;

struct m3config* parse(struct Token*);

#ifdef __cplusplus
}
#endif

#endif
