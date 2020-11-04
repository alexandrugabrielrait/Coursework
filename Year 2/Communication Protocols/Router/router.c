#include "skel.h"
#include <netinet/ip.h> //TODO DELETE

struct route_table_entry *rtable;
int rtable_size;

struct arp_entry *arp_table;
int arp_table_len = 0;

int main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);
	packet m;
	queue packet_queue = queue_create();
	int rc;

	init();
	rtable = malloc(sizeof(struct route_table_entry) * get_rtable_size());
	arp_table = malloc(sizeof(struct arp_entry) * 100);
	DIE(rtable == NULL, "memory");
	rtable_size = read_rtable(rtable);

	while (1)
	{
		rc = get_packet(&m);
		DIE(rc < 0, "get_message");
		/* Students will write code here */
		uint32_t ip_addr;
		struct in_addr inaddr;
		inet_aton(get_interface_ip(m.interface), &inaddr);
		ip_addr = ntohl(inaddr.s_addr);
		uint8_t mac_addr[6] = {0, 0, 0, 0, 0, 0};
		DIE(get_interface_mac(m.interface, mac_addr) < 0, "Interface MAC address not found");
		struct ether_header *eth_hdr = (struct ether_header *)m.payload;
		//verificam tipul pachetului
		switch (ntohs(eth_hdr->ether_type))
		{
		case ETHERTYPE_ARP:;
			struct arphdr *arp_hdr = (struct arphdr *)(m.payload + sizeof(struct ether_header));
			struct arpaddr *arp_e = (struct arpaddr *)(m.payload + sizeof(struct ether_header) + sizeof(struct arphdr));
			short op = ntohs(arp_hdr->ar_op);
			if (op == ARPOP_REQUEST)
			{
				uint32_t source_addr = ntohl(*(uint32_t *)arp_e->ar_spa);
				uint32_t target_addr = ntohl(*(uint32_t *)arp_e->ar_tpa);
				//daca requestul este trimis routerului,  ii raspundem
				if (ip_addr == target_addr)
				{
					arp_hdr->ar_op = htons(ARPOP_REPLY);
					if (get_arp_entry(source_addr) == NULL)
						add_arp_entry(source_addr, eth_hdr->ether_shost);
					memcpy(arp_e->ar_tpa, arp_e->ar_spa, 4);
					ip_addr = htonl(ip_addr);
					memcpy(arp_e->ar_spa, &ip_addr, 4);
					ip_addr = ntohl(ip_addr);
					memcpy(eth_hdr->ether_dhost, eth_hdr->ether_shost, 6);
					memcpy(arp_e->ar_tha, eth_hdr->ether_shost, 6);
					memcpy(eth_hdr->ether_shost, mac_addr, 6);
					memcpy(arp_e->ar_sha, mac_addr, 6);
					send_packet(m.interface, &m);
				}
			}
			else if (op == ARPOP_REPLY)
			{
				int source_addr = ntohl(*(int *)arp_e->ar_spa);
				if (get_arp_entry(source_addr) == NULL)
					add_arp_entry(source_addr, eth_hdr->ether_shost);
				//daca primim un reply, cream o coada noua
				queue temp_queue = packet_queue;
				packet_queue = queue_create();
				//incercam sa trimitem pachete spre noua adresa gasit
				packet *packet_ex;
				while (!queue_empty(temp_queue))
				{
					packet_ex = queue_deq(temp_queue);
					//daca pachetul nu este trimis si nici aruncat, el reintra in coada
					if (process_IP_packet(*packet_ex, ip_addr, mac_addr, packet_queue, 0) == 0)
						queue_enq(packet_queue, packet_ex);
					else
						free(packet_ex);
				}
				free(temp_queue);
			}
			break;
		case ETHERTYPE_IP:
			//aici pachetul va fi nevoit sa ceara un request deci arp_request = 1
			process_IP_packet(m, ip_addr, mac_addr, packet_queue, 1);
			break;
		}
	}

	free(rtable);
	return 0;
}