#include "helpers.hpp"
#include <list>
#include <map>

//topic -> subscriber
map<string, list<subscriber>> subscriber_map;
//client_id -> client_fd
map<string, int> file_descriptors;
//client_fd -> client_id
map<int, string> ids;
//client_fd -> adresa ip
map<int, string> ip_addresses;
//client_fd -> port
map<int, uint16_t> ports;
//client_id -> lista mesaje ratate
map<string, list<server_message>> missed_messages;
//client_fd -> buffer input
map<int, input_buffer*> buffers;

void usage(char *file)
{
	fprintf(stderr, "Usage: %s server_port\n", file);
	exit(0);
}

//mapeaza un client nou
int add_client(int client_fd, string client_id)
{
	//verificam daca nu mai exista vreun client cu acelasi id
	if (file_descriptors.find(client_id) == file_descriptors.end() ||
											file_descriptors[client_id] == 0)
	{
		ids[client_fd] = client_id;
		file_descriptors[client_id] = client_fd;
		return 1;
	}
	//daca exista intoarcem 0
	return 0;
}

//scoate un client din mapari
void remove_client(int client_fd)
{
	//verificam daca exista clientul
	if (!ids[client_fd].empty() && file_descriptors.find(ids[client_fd])
													!= file_descriptors.end())
	{
		/*stergem maparile clientului
		pentru fd, setam la 0 ca sa verificam daca un client
		nu mai are fd (e offline)
		*/
		file_descriptors[ids[client_fd]] = 0;
		ids.erase(client_fd);
	}
}

//transforma topicul in char*, adaugand un 0 la final
string topic_string(char *topic)
{
	char buffer[TOPICLEN + 1];
	memcpy(buffer, topic, TOPICLEN);
	buffer[TOPICLEN] = 0;
	return buffer;
}

//aboneaza un client la un topic cu un anumit tip de sf
int subscribe(int client_fd, char *topic, int sf)
{
	string topic_s = topic_string(topic);
	//verificam daca lista topicului exista
	if (subscriber_map.find(topic_s) == subscriber_map.end())
	{
		//daca nu exista, o introducem in map
		subscriber_map.insert(pair<string, list<subscriber>>(topic_s, {}));
	}
	else
		for (auto itr = subscriber_map[topic_s].begin();
									itr != subscriber_map[topic_s].end(); itr++)
			if (!itr->id.compare(ids[client_fd]))
			{
				//daca clientul e abonat inlocuim sf
				itr->sf = sf;
				return sf;
			}
	//abonam clientul
	subscriber sb;
	sb.sf = sf;
	sb.id = ids[client_fd];
	subscriber_map[topic_s].push_back(sb);
	return 2;
}

//dezaboneaza un client de la un topic
int unsubscribe(int client_fd, char *topic)
{
	string topic_s = topic_string(topic);
	//verificam daca topicul exista
	if (subscriber_map.find(topic_s) != subscriber_map.end())
		for (auto itr = subscriber_map[topic_s].begin();
									itr != subscriber_map[topic_s].end(); itr++)
			if (!itr->id.compare(ids[client_fd]))
			{
				//dezabonam clientul
				subscriber_map[topic_s].erase(itr);
				return 1;
			}
	//daca clientul nu e abonat la acel topic
	return 0;
}

//adaugam un mesaj la mesajele ratate ale unui client
void add_missed_message(string client_id, server_message msg)
{
	//verificam daca lista exista
	if (missed_messages.find(client_id) == missed_messages.end())
	{
		//daca nu exista, o introducem in map
		missed_messages.insert(pair<string,
										list<server_message>>(client_id, {}));
	}
	missed_messages[client_id].push_back(msg);
}

int main(int argc, char *argv[])
{
	int newsockfd, portno;
	char buffer[BUFLEN];
	server_message smsg;
	struct sockaddr_in serv_addr, cli_addr;
	int n, i, ret;
	socklen_t cli_len;

	fd_set read_fds;
	fd_set tmp_fds;
	int fdmax;

	if (argc != 2)
	{
		usage(argv[0]);
	}

	FD_ZERO(&read_fds);
	FD_ZERO(&tmp_fds);

	int tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	DIE(tcp_sockfd < 0, "tcp_sockfd");
	int udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	DIE(udp_sockfd < 0, "tcp_sockfd");

	portno = atoi(argv[1]);
	DIE(portno == 0, "atoi");

	memset((char *)&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	int enable = 1;
	if (setsockopt(tcp_sockfd, SOL_SOCKET, SO_REUSEADDR,
													&enable, sizeof(int)) == -1)
	{
		perror("setsocketopt tcp");
		exit(1);
	}
	if (setsockopt(udp_sockfd, SOL_SOCKET, SO_REUSEADDR,
													&enable, sizeof(int)) == -1)
	{
		perror("setsocketopt udp");
		exit(1);
	}

	ret = bind(tcp_sockfd, (struct sockaddr *)&serv_addr,
													sizeof(struct sockaddr));
	DIE(ret < 0, "bind tcp");
	ret = bind(udp_sockfd, (struct sockaddr *)&serv_addr,
													sizeof(struct sockaddr));
	DIE(ret < 0, "bind udp");

	ret = listen(tcp_sockfd, MAX_CLIENTS);
	DIE(ret < 0, "listen");

	FD_SET(0, &read_fds);

	//adaugam fd tcp si udp
	FD_SET(tcp_sockfd, &read_fds);
	FD_SET(udp_sockfd, &read_fds);

	//setam fdmax la maximul dintre cele doua fd
	fdmax = tcp_sockfd > udp_sockfd ? tcp_sockfd : udp_sockfd;

	while (1)
	{
		tmp_fds = read_fds;

		ret = select(fdmax + 1, &tmp_fds, NULL, NULL, NULL);
		DIE(ret < 0, "select");

		for (i = 0; i <= fdmax; i++)
		{
			if (FD_ISSET(i, &tmp_fds))
			{
				if (i == 0)
				{
					//citire de la stdin
					memset(buffer, 0, BUFLEN);
					fgets(buffer, BUFLEN - 1, stdin);
					strtok(buffer, " \n");
					if (strcmp(buffer, "exit") == 0)
					{
						exit(0);
					}
					else
					{
						printf("Invalid command: %s\n", buffer);
					}
				}
				else if (i == tcp_sockfd)
				{
					//citire de pe tcp
					cli_len = sizeof(cli_addr);
					newsockfd = accept(tcp_sockfd, (struct sockaddr *)&cli_addr,
																	&cli_len);
					DIE(newsockfd < 0, "accept");

					FD_SET(newsockfd, &read_fds);
					if (newsockfd > fdmax)
					{
						fdmax = newsockfd;
					}

					/*setam id-ul la id necunoscut
					incepem sirul cu \0 ca sa fim siguri ca nu
					este un nume care poate fi dat de client
					*/
					ids[newsockfd] = "\0";
					//trimitem mesaj de interogare id
					memset(&smsg, 0, sizeof(server_message));
					smsg.message_type = -1;
					ip_addresses[newsockfd] = inet_ntoa(cli_addr.sin_addr);
					ports[newsockfd] = ntohs(cli_addr.sin_port);
					buffers[newsockfd] =
								(input_buffer *)calloc(1, sizeof(input_buffer));
					n = send(newsockfd, &smsg, smsg_size(smsg.message_type, 0),
																			 0);
					DIE(n < 0, "send");
				}
				else if (i == udp_sockfd)
				{
					//citire mesaj udp
					int bytes_received;
					memset(&smsg, 0, sizeof(server_message));
					bytes_received = recvfrom(udp_sockfd, &(smsg.notif),
											sizeof(notification), 0,
											(struct sockaddr *)&(smsg.source),
											&cli_len);
					if (bytes_received < 0)
						continue;
					/*trimitem mesajul la subscriberii care sunt abonati la
					topicul lui
					*/
					smsg.message_type = 0;
					for (auto itr = subscriber_map[smsg.notif.topic].begin();
						itr != subscriber_map[smsg.notif.topic].end(); itr++)
					{
						if (file_descriptors[itr->id])
						{
							n = send(file_descriptors[itr->id],
											&smsg, smsg_size(smsg.message_type,
											smsg.notif.data_type), 0);
							DIE(n < 0, "send");
						}
						else if (itr->sf)
						/*daca clientul e offline si are sf = 1
						punem mesajul in lista de mesaje ratate
						*/
							add_missed_message(itr->id, smsg);
					}
				}
				else
				{
					//citim mesaj de la un subscriber
					memset(buffers[i]->new_bytes_buffer, 0,
														sizeof(client_message));
					n = recv(i, buffers[i]->new_bytes_buffer,
													 sizeof(client_message), 0);
					DIE(n < 0, "recv");

					if (n == 0)
					{
						//conexiunea s-a inchis
						if (ids[i][0] != 0)
							printf("Client %s disconnected.\n", ids[i].c_str());
						/*scoatem clientul, eliberam bufferul
						si inchidem socketul
						*/
						remove_client(i);
						close(i);
						free(buffers[i]);
						FD_CLR(i, &read_fds);
					}
					else
					{
						//procesam bufferul
						int size;
						//calculam dimensiunea
						if (buffers[i]->current_bytes == 0)
							size = cmsg_size(buffers[i]->new_bytes_buffer[0]);
						else
							size = cmsg_size(buffers[i]->msg.message_type);
						if (buffers[i]->current_bytes + n >= size)
						{
							/*scriem in mesaj daca avem cel
							putin un mesaj complet
							*/
							int bytes_received = n;
							n = n - size + buffers[i]->current_bytes;
							memcpy((char *)(&buffers[i]->msg)
												+ buffers[i]->current_bytes,
												buffers[i]->new_bytes_buffer,
											size - buffers[i]->current_bytes);
							memmove(buffers[i]->new_bytes_buffer,
											buffers[i]->new_bytes_buffer + n,
											bytes_received - n);
							buffers[i]->current_bytes = 0;
							client_message cmsg = buffers[i]->msg;
							//procesam mesajul
							switch (cmsg.message_type)
							{
							case -1:
							{
								//raspuns interogare
								string client_id = cmsg.content;
								int added = add_client(i, client_id);
								if (added)
									printf(
									"New client %s connected from %s:%hu.\n",
									client_id.c_str(), ip_addresses[i].c_str(),
									ports[i]);
								else
								{
									//numele e deja folosit
									server_message name_taken_msg;
									name_taken_msg.message_type = -2;
									int n = send(i, &name_taken_msg,
											smsg_size(smsg.message_type, 3), 0);
									DIE(n < 0, "send");
								}
								ip_addresses.erase(i);
								ports.erase(i);
								if (added && missed_messages.find(client_id)
													!= missed_messages.end())
									while (!missed_messages[client_id].empty())
									{
										server_message msg =
											missed_messages[client_id].front();
										missed_messages[client_id].pop_front();
										n = send(i, &msg,
													smsg_size(msg.message_type,
													msg.notif.data_type), 0);
										DIE(n < 0, "send");
									}
								break;
							}
							case 0: //subscribe cu sf = 0
							case 1: //subscribe cu sf = 1
								memset(&smsg, 0, sizeof(server_message));
								//folosim data_type ca rezultat pentru subscribe
								smsg.notif.data_type = subscribe(i,
											cmsg.content, cmsg.message_type);
								smsg.message_type = 1;
								memcpy(smsg.notif.topic, cmsg.content, 50);
								n = send(i, &smsg,
											smsg_size(smsg.message_type, 0), 0);
								DIE(n < 0, "send");
								break;
							case 2: //unsubscribe
								memset(&smsg, 0, sizeof(server_message));
								/*folosim data_type ca rezultat
								pentru unsubscribe
								*/
								smsg.notif.data_type =
												unsubscribe(i, cmsg.content);
								smsg.message_type = 2;
								memcpy(smsg.notif.topic, cmsg.content, 50);
								n = send(i, &smsg,
											smsg_size(smsg.message_type, 0), 0);
								DIE(n < 0, "send");
								break;
							}
						}
						//daca mai sunt byti in buffer, ii concatenam la mesaj
						if (n != 0 && buffers[i]->current_bytes + n < size)
						{
							memcpy((char *)(&buffers[i]->msg) +
											buffers[i]->current_bytes,
											buffers[i]->new_bytes_buffer, n);
							buffers[i]->current_bytes += n;
							continue;
						}
					}
				}
			}
		}
	}

	close(tcp_sockfd);
	close(udp_sockfd);

	return 0;
}