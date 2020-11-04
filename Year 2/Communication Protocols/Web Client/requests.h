#ifndef _REQUESTS_
#define _REQUESTS_

// trimite o cerere de tip GET
char *compute_get_request(const char *host, const char *url,
            const char *query_params, const char **headers, int headers_count,
                                    const char **cookies, int cookies_count);

// trimite o cerere de tip DELETE
char *compute_delete_request(const char *host, const char *url,
            const char *query_params, const char **headers, int headers_count,
                                    const char **cookies, int cookies_count);

// trimite o cerere de tip POST
char *compute_post_request(const char *host, const char *url,
            const char *content_type, const char **headers, int headers_count,
                const char **cookies, int cookies_count, const char *body_data);

#endif