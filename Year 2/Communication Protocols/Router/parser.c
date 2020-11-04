#include "parser.h"

void add_arp_entry(__u32 ip, uint8_t *mac)
{
	arp_table[arp_table_len].ip = ip;
	memcpy(arp_table[arp_table_len++].mac, mac, 6);
}

struct route_table_entry *get_best_route(uint32_t dest_ip)
{
	int i;
	struct route_table_entry *rte = NULL;
	uint32_t mask = 0;
	for (i = 0; i < rtable_size; ++i)
	{
		if (((dest_ip & rtable[i].mask) == rtable[i].prefix) && (rtable[i].mask > mask))
		{
			rte = (rtable + i);
			mask = rtable[i].mask;
		}
	}
	return rte;
}

struct arp_entry *get_arp_entry(uint32_t ip)
{
	int i;
	for (i = 0; i < arp_table_len; ++i)
	{
		if (ip == arp_table[i].ip)
			return (arp_table + i);
	}
	return NULL;
}

int get_rtable_size()
{
	FILE *fd = fopen("rtable.txt", "r");
	int entries = 0;
	int last_char_is_digit = 0;
	while (!feof(fd))
	{
		char c = fgetc(fd);
		if (c == '\n')
		{
			entries++;
			last_char_is_digit = 0;
		}
		else if (c >= '0' && c <= '9')
			last_char_is_digit = 1;
	}
	fclose(fd);
	if (last_char_is_digit)
		entries++;
	return entries;
}

int read_rtable(struct route_table_entry *rtable)
{
	FILE *fd = fopen("rtable.txt", "r");
	int entries = 0;
	char *line = NULL;
	size_t len = 0;
	while (getline(&line, &len, fd) > 0)
	{
		struct in_addr inaddr;
		char prefix[15], hop[15], mask[15];
		int interface;
		sscanf(line, "%s %s %s %i\n", prefix, hop, mask, &interface);
		DIE(!inet_aton(prefix, &inaddr), "inet_aton");
		rtable[entries].prefix = inaddr.s_addr;
		DIE(!inet_aton(hop, &inaddr), "inet_aton");
		rtable[entries].next_hop = inaddr.s_addr;
		DIE(!inet_aton(mask, &inaddr), "inet_aton");
		rtable[entries].mask = inaddr.s_addr;
		rtable[entries].interface = interface;
		entries++;
	}
	free(line);
	fclose(fd);
	return entries;
}