#ifndef _M3CONF_CONFIGFILE_H_
#define _M3CONF_CONFIGFILE_H_

#ifdef __cplusplus
extern "C" {
#endif

struct m3config;

const char* m3conf_get_str(const struct m3config*, const char* key, const char* def);
int         m3conf_get_int(const struct m3config*, const char* key, int def);

#ifdef __cplusplus
}
#endif

#endif
