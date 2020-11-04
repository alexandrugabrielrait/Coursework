#include "helpers.hpp"

/* trimite o cerere cu nume variabil (poate fi apelata atat pentru GET,
                                                        cat si pentru DELETE)
*/
char *compute_simple_request(const char* request_type, const char *host,
                                    const char *url, const char *query_params,
                                    const char **headers, int headers_count,
                                    const char **cookies, int cookies_count)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));

    if (query_params != NULL)
    {
        sprintf(line, "%s %s?%s HTTP/1.1", request_type, url, query_params);
    }
    else
    {
        sprintf(line, "%s %s HTTP/1.1", request_type, url);
    }

    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (headers != NULL)
    {
        int i;
        for (i = 0; i < headers_count; i++)
        {
            strcpy(line, headers[i]);
            compute_message(message, line);
        }
    }
    if (cookies != NULL)
    {
        strcpy(line, "Cookie:");
        int i;
        for (i = 0; i < cookies_count - 1; i++)
        {
            sprintf(line + strlen(line), " %s;", cookies[i]);
        }
        sprintf(line + strlen(line), " %s", cookies[i]);
        compute_message(message, line);
    }
    compute_message(message, "");

    free(line);
    return message;
}

char *compute_get_request(const char *host, const char *url,
            const char *query_params, const char **headers, int headers_count,
                                        const char **cookies, int cookies_count)
{
    return compute_simple_request("GET", host, url, query_params, headers,
                                        headers_count, cookies, cookies_count);
}

char *compute_delete_request(const char *host, const char *url,
            const char *query_params, const char **headers, int headers_count,
                                        const char **cookies, int cookies_count)
{
    return compute_simple_request("DELETE", host, url, query_params, headers,
                                        headers_count, cookies, cookies_count);
}

char *compute_post_request(const char *host, const char *url,
            const char *content_type, const char **headers, int headers_count,
                const char **cookies, int cookies_count, const char *body_data)
{
    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));
    char *body_data_buffer = (char *)calloc(LINELEN, sizeof(char));

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    int content_length = 0;
    int length = strlen(body_data);
    memcpy(body_data_buffer + content_length, body_data, length);
    content_length += length;
    body_data_buffer[content_length] = 0;
    sprintf(line, "Content-Length: %i", content_length);
    compute_message(message, line);
    if (headers != NULL)
    {
        for (int i = 0; i < headers_count; i++)
        {
            strcpy(line, headers[i]);
            compute_message(message, line);
        }
    }

    if (cookies != NULL)
    {
        strcpy(line, "Cookie:");
        int i;
        for (i = 0; i < cookies_count - 1; i++)
        {
            sprintf(line + strlen(line), " %s;", cookies[i]);
        }
        sprintf(line + strlen(line), " %s", cookies[i]);
        compute_message(message, line);
    }
    compute_message(message, "");

    memset(line, 0, LINELEN);
    compute_message(message, body_data_buffer);

    free(body_data_buffer);
    free(line);
    return message;
}