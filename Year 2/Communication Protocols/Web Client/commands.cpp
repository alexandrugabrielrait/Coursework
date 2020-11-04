#include "helpers.hpp"

char *message;
char *response;
string cookie;
string token;
string library_prefix = "Authorization: Bearer ";

// trimite mesajul si se asigura ca se primeste raspuns de la server
void force_send()
{
    send_to_server(sockfd, message);
    response = receive_from_server(sockfd);
    while (!connected)
    {
        free(response);
        sockfd = open_connection(HOST_IP, PORT, AF_INET, SOCK_STREAM, 0);
        send_to_server(sockfd, message);
        response = receive_from_server(sockfd);
    }
}

void register_user(string username, string password)
{
    json j;
    j["username"] = username;
    j["password"] = password;
    message = compute_post_request(HOST, "/api/v1/tema/auth/register",
                        "application/json", NULL, 0, NULL, 0, j.dump().c_str());
    force_send();
    int response_id = get_response_id(response);
    if (response_id == RESP_CREATED)
    {
        cout << "Account created successfully!" << endl;
    }
    else if (response_id == RESP_TOO_MANY)
    {
        cout << "Too many requests, please try again later." << endl;
    }
    else
    {
        // contul exista deja
        j = get_json_content(response);
        cout << j["error"].get<string>() << endl;
    }
    free(message);
    free(response);
}

void login(string username, string password)
{
    json j;
    j["username"] = username;
    j["password"] = password;
    message = compute_post_request(HOST, "/api/v1/tema/auth/login",
                        "application/json", NULL, 0, NULL, 0, j.dump().c_str());
    force_send();
    int response_id = get_response_id(response);
    if (response_id == RESP_OK)
    {
        cookie = get_cookie(response);
        cout << "Logged in successfully!" << endl;
    }
    else if (response_id == RESP_TOO_MANY)
    {
        cout << "Too many requests, please try again later." << endl;
    }
    else
    {
        // nume/parola gresite
        j = get_json_content(response);
        cout << j["error"].get<string>() << endl;
    }
    free(message);
    free(response);
}

void enter_library()
{
    const char *cookies[1];
    cookies[0] = cookie.c_str();
    message = compute_get_request(HOST, "/api/v1/tema/library/access",
                                                    NULL, NULL, 0, cookies, 1);
    force_send();
    if (get_response_id(response) == RESP_OK)
    {
        cout << "Library access granted!" << endl;
    }
    else
    {
        // utilizatorul nu este logat
        json j = get_json_content(response);
        cout << j["error"].get<string>() << endl;
    }
    free(message);
    free(response);
}

void get_books()
{
    const char *headers[1];
    string header_str = library_prefix + token; 
    headers[0] = header_str.c_str();
    message = compute_get_request(HOST, "/api/v1/tema/library/books",
                                                    NULL, headers, 1, NULL, 0);
    force_send();
    if (get_response_id(response) == RESP_OK)
    {
        json j = get_json_content(response);
        if (j.size() > 0)
        {
            cout << j.size() << " books found:" << endl;
            cout << "-------------------------------------" << endl;
            cout << "Book id - Title" << endl;
            cout << "-------------------------------------" << endl;
            for (unsigned i = 0; i < j.size(); i++)
            {
                cout << j[i]["id"].get<int>() << " - " << "\""
                                << j[i]["title"].get<string>() << "\"" << endl;
            }
            cout << "-------------------------------------" << endl;
        }
        else
            cout << "No books found!" << endl;
    }
    else
    {
        cout << "You don't have access to the library yet!" << endl;
    }
    free(message);
    free(response);
}

void get_book(int id)
{
    const char *headers[1];
    string header_str = library_prefix + token; 
    headers[0] = header_str.c_str();
    char *url = (char *)calloc(32, sizeof(char));
    sprintf(url, "/api/v1/tema/library/books/%d", id);
    message = compute_get_request(HOST, url, NULL, headers, 1, NULL, 0);
    force_send();
    int response_id = get_response_id(response);
    if (response_id == RESP_OK)
    {
        /* continutul este o lista cu un singur element, 
        asa ca vom dereferenția cu [0]
        */
        json j = get_json_content(response)[0];
        cout << "\"" << j["title"].get<string>() << "\" by "
                                        << j["author"].get<string>() << endl;
        cout << "Genre: " << j["genre"].get<string>() << endl;
        cout << "Publisher: " << j["publisher"].get<string>() << endl;
        cout << "Page count: " << j["page_count"].get<int>() << endl;
    }
    else if (response_id == RESP_UNAUTHORIZED
                                            || response_id == RESP_INT_SERV_ERR)
    {
        cout << "You don't have access to the library yet!" << endl;
    }
    free(url);
    free(message);
    free(response);
}

void add_book(string title, string author, string genre,
                                            string publisher, int page_count)
{
    json j;
    j["title"] = title;
    j["author"] = author;
    j["genre"] = genre;
    j["publisher"] = publisher;
    j["page_count"] = page_count;
    const char *headers[1];
    string header_str = library_prefix + token; 
    headers[0] = header_str.c_str();
    message = compute_post_request(HOST, "/api/v1/tema/library/books",
                    "application/json", headers, 1, NULL, 0, j.dump().c_str());
    force_send();
    int response_id = get_response_id(response);
    if (response_id == RESP_OK)
    {
        cout << "Book added successfully!" << endl;
    }
    else if (response_id == RESP_UNAUTHORIZED
                                            || response_id == RESP_INT_SERV_ERR)
    {
        cout << "You don't have access to the library yet!" << endl;
    }
    else if (response_id == RESP_TOO_MANY)
    {
        cout << "Too many requests, please try again later." << endl;
    }
    free(message);
    free(response);
}

void delete_book(int id)
{
    const char *headers[1];
    string header_str = library_prefix + token; 
    headers[0] = header_str.c_str();
    char *url = (char *)calloc(32, sizeof(char));
    sprintf(url, "/api/v1/tema/library/books/%d", id);
    message = compute_delete_request(HOST, url, NULL, headers, 1, NULL, 0);
    force_send();
    int response_id = get_response_id(response);
    if (response_id == RESP_OK)
    {
        cout << "Book deleted successfully!" << endl;
    }
    else if (response_id == RESP_UNAUTHORIZED
                                            || response_id == RESP_INT_SERV_ERR)
    {
        cout << "You don't have access to the library yet!" << endl;
    }
    else
    {
        json j = get_json_content(response);
        cout << j["error"].get<string>() << endl;
    }
    free(url);
    free(message);
    free(response);
}

void logout(bool silent)
{
    const char *cookies[1];
    cookies[0] = cookie.c_str();
    message = compute_get_request(HOST, "/api/v1/tema/auth/logout",
                                                    NULL, NULL, 0, cookies, 1);
    force_send();
    if (get_response_id(response) == RESP_OK)
    {
        if (!silent)
            cout << "Logged out successfully!" << endl;
        cookie.clear();
        token.clear();
    }
    else if (!silent)
    {
        json j = get_json_content(response);
        cout << j["error"].get<string>() << endl;
    }
    free(message);
    free(response);
}