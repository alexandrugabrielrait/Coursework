#include "helpers.hpp"

int sockfd;

char cli_buffer[BUFLEN];

int main(int argc, char *argv[])
{
    sockfd = open_connection(HOST_IP, PORT, AF_INET, SOCK_STREAM, 0);
    while (1)
    {
        memset(cli_buffer, 0, BUFLEN);
        // citim o linie
        fgets(cli_buffer, BUFLEN - 1, stdin);
        // scoatem space si end line
        strtok(cli_buffer, " \n");
        // verificam daca comanda are mai mult de un cuvant
        if (strtok(NULL, " \n") != NULL)
        {
            cout << "Incorrect format!" << endl;
        }
        else if (strcmp(cli_buffer, "register") == 0)
        {
            string username;
            string password;
            cout << "username=";
            getline(cin, username);
            cout << "password=";
            getline(cin, password);
            register_user(username, password);
        }
        else if (strcmp(cli_buffer, "login") == 0)
        {
            string username;
            string password;
            cout << "username=";
            getline(cin, username);
            cout << "password=";
            getline(cin, password);
            login(username, password);
        }
        else if (strcmp(cli_buffer, "enter_library") == 0)
        {
            enter_library();
        }
        else if (strcmp(cli_buffer, "get_books") == 0)
        {
            get_books();
        }
        else if (strcmp(cli_buffer, "get_book") == 0)
        {
            int id;
            cout << "id=";
            cin >> id;
            cin.sync();
            // verificam daca a fost introdus un numar intreg
            if (cin.good())
                get_book(id);
            else
            {
                cin.clear();
                cout << "Incorrect format!" << endl;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else if (strcmp(cli_buffer, "add_book") == 0)
        {
            string title;
            string author;
            string genre;
            string publisher;
            int page_count;
            cout << "title=";
            getline(cin, title);
            cout << "author=";
            getline(cin, author);
            cout << "genre=";
            getline(cin, genre);
            cout << "publisher=";
            getline(cin, publisher);
            cout << "page_count=";
            cin >> page_count;
            cin.sync();
            // verificam daca a fost introdus un numar intreg
            if (cin.good())
                add_book(title, author, genre, publisher, page_count);
            else
            {
                cin.clear();
                cout << "Incorrect format!" << endl;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else if (strcmp(cli_buffer, "delete_book") == 0)
        {
            int id;
            cout << "id=";
            cin >> id;
            cin.sync();
            // verificam daca a fost introdus un numar intreg
            if (cin.good())
                delete_book(id);
            else
            {
                cin.clear();
                cout << "Incorrect format!" << endl;
            }
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else if (strcmp(cli_buffer, "logout") == 0)
        {
            /* apelam logout nesilentios pentru ca utilizatorul sa fie
            anuntat de rezultatul acestei comenzi
            */
            logout(false);
        }
        else if (strcmp(cli_buffer, "exit") == 0)
        {
            break;
        }
        else
        {
            // daca se citeste o alta comanda decat cele definite
            cout << "Unknown command: " << cli_buffer << endl;
        }
    }
    /* apelam logout silentios pentru a sterge cookieul de la server,
        fara a anunta utilizatorul
    */
    logout(true);
    close_connection(sockfd);
    return 0;
}