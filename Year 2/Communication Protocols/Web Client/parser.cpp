#include "helpers.hpp"

int get_response_id(char *message)
{
    char *found = strchr(message, ' ') + 1;
    char *end = strchr(found, ' ');
    char *id_string = (char*)calloc(end - found + 1, sizeof(char));
    memcpy(id_string, found, end - found);
    int id = atoi(id_string);
    free(id_string);
    return id;
}

string get_cookie(char *message)
{
    // 12 e strlen(Set-Cookie: ")
    char *found = strstr(message, "Set-Cookie: ") + 12;
    char *end = strchr(found, ';');
    int i = end - found;
    char *cookie = (char*)calloc(i + 1, sizeof(char));
    memcpy(cookie, found, i);
    string cookie_str = cookie;
    free(cookie);
    return cookie_str;
}

json get_json_content(char *message)
{
    char *found = strstr(message, HEADER_TERMINATOR) + HEADER_TERMINATOR_SIZE;
    char *end = strchr(found, 0);
    int i = end - found;
    char *content = (char*)calloc(i + 1, sizeof(char));
    memcpy(content, found, i);
    json j = json::parse(content);
    free(content);
    return j;
}