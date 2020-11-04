#include "skel.h"

void send_ICMP_packet(packet m, uint32_t ip_addr, uint8_t *mac_addr, uint8_t icmp_type)
{
	struct ether_header *eth_hdr = (struct ether_header *)m.payload;
	struct iphdr *ip_hdr = (struct iphdr *)(m.payload + sizeof(struct ether_header));
	packet icmp_p;
	struct ether_header *eth_hdr_p = (struct ether_header *)icmp_p.payload;
	struct iphdr *ip_hdr_p = (struct iphdr *)(icmp_p.payload + sizeof(struct ether_header));
	struct icmphdr *icmp_hdr = (struct icmphdr *)(icmp_p.payload + sizeof(struct ether_header) + sizeof(struct iphdr));
	char data[] = "ICMP Message";
	icmp_p.interface = m.interface;
	memcpy(icmp_hdr + sizeof(struct icmphdr), data, sizeof(data));
	icmp_p.len = sizeof(struct ether_header) + sizeof(struct iphdr) + sizeof(struct icmphdr) + sizeof(data);
	memcpy(eth_hdr_p->ether_dhost, eth_hdr->ether_shost, 6);
	memcpy(eth_hdr_p->ether_shost, mac_addr, 6);
	eth_hdr_p->ether_type = htons(ETHERTYPE_IP);
	ip_hdr_p->version = 4;
	ip_hdr_p->ihl = sizeof(struct iphdr) / 4;
	ip_hdr_p->tos = 0;
	ip_hdr_p->tot_len = htons(sizeof(struct iphdr) + sizeof(struct icmphdr) + sizeof(data));
	ip_hdr_p->id = htons(4);
	ip_hdr_p->ttl = 64;
	ip_hdr_p->protocol = IPPROTO_ICMP;
	ip_hdr_p->daddr = ip_hdr->saddr;
	ip_hdr_p->saddr = htonl(ip_addr);
	ip_hdr_p->check = 0;
	ip_hdr_p->check = checksum(ip_hdr_p, sizeof(struct iphdr));

	icmp_hdr->type = icmp_type;
	icmp_hdr->code = 0;
	icmp_hdr->checksum = 0;
	icmp_hdr->checksum = checksum(icmp_hdr, sizeof(struct icmphdr) + sizeof(data));
	send_packet(icmp_p.interface, &icmp_p);
	return;
}

int process_IP_packet(packet m, uint32_t ip_addr, uint8_t *mac_addr, queue packet_queue, int shouldRequest)
{
	struct ether_header *eth_hdr = (struct ether_header *)m.payload;
	struct iphdr *ip_hdr = (struct iphdr *)(m.payload + sizeof(struct ether_header));
	if (ip_hdr->ttl <= 1)
	{
		send_ICMP_packet(m, ip_addr, mac_addr, ICMP_TIME_EXCEEDED);
		return -1;
	}
	if (checksum(ip_hdr, sizeof(struct iphdr)))
		return -1;
	uint16_t HC;
	struct route_table_entry *entry;
	int iAmReceiver = 0;
	if (ntohl(ip_hdr->daddr) == ip_addr)
		iAmReceiver = 1;
	else
	{
		entry = get_best_route(ip_hdr->daddr);
		if (entry == NULL)
		{
			send_ICMP_packet(m, ip_addr, mac_addr, ICMP_DEST_UNREACH);
			return -1;
		}
	}
	struct arp_entry *arpentry;
	if (!iAmReceiver)
	{
		arpentry = get_arp_entry(ntohl(entry->next_hop));

		if (arpentry == NULL)
		{
			if (!shouldRequest)
			{
				return 0;
			}
			else
			{
				packet *stored_packet = malloc(sizeof(packet));
				memcpy(stored_packet, &m, sizeof(packet));
				queue_enq(packet_queue, stored_packet);
				packet arp_request_p;
				arp_request_p.interface = entry->interface;
				arp_request_p.len = sizeof(struct ether_header) + sizeof(struct arphdr) + sizeof(struct arpaddr);
				struct ether_header *eth_hdr_req = (struct ether_header *)arp_request_p.payload;
				struct arphdr *arp_hdr = (struct arphdr *)(arp_request_p.payload + sizeof(struct ether_header));
				struct arpaddr *arp_e = (struct arpaddr *)(arp_request_p.payload + sizeof(struct ether_header) + sizeof(struct arphdr));
				memset(eth_hdr_req->ether_dhost, 0xff, 6);
				get_interface_mac(arp_request_p.interface, eth_hdr_req->ether_shost);
				eth_hdr_req->ether_type = htons(ETHERTYPE_ARP);
				arp_hdr->ar_hrd = htons(1);
				arp_hdr->ar_pro = htons(ETHERTYPE_IP);
				arp_hdr->ar_hln = 6;
				arp_hdr->ar_pln = 4;
				memset(arp_e->ar_tha, 0, 6);
				get_interface_mac(arp_request_p.interface, arp_e->ar_sha);
				struct in_addr inaddr;
				inet_aton(get_interface_ip(arp_request_p.interface), &inaddr);
				memcpy(arp_e->ar_spa, &inaddr.s_addr, 4);
				memcpy(arp_e->ar_tpa, &(entry->next_hop), 4);
				arp_hdr->ar_op = htons(ARPOP_REQUEST);
				send_packet(arp_request_p.interface, &arp_request_p);
				return 0;
			}
		}
	}
	HC = ip_hdr->check;
	check_remove_old(&HC, &ip_hdr->ttl, 0);
	ip_hdr->ttl--;
	check_add_new(&HC, &ip_hdr->ttl, 0);
	ip_hdr->check = HC;
	if (ip_hdr->protocol == IPPROTO_ICMP)
	{
		struct icmphdr *icmp_hdr = (struct icmphdr *)(m.payload + sizeof(struct ether_header) + sizeof(struct iphdr));
		//daca pachetul este un icmp echo trimis catre mine
		if (iAmReceiver && icmp_hdr->type == ICMP_ECHO)
		{
			memcpy(eth_hdr->ether_dhost, eth_hdr->ether_shost, 6);
			memcpy(eth_hdr->ether_shost, mac_addr, 6);
			check_remove_old(&HC, &ip_hdr->daddr, 0);
			check_remove_old(&HC, &ip_hdr->daddr, 1);
			ip_hdr->daddr = ip_hdr->saddr;
			check_add_new(&HC, &ip_hdr->daddr, 0);
			check_add_new(&HC, &ip_hdr->daddr, 1);
			check_remove_old(&HC, &ip_hdr->saddr, 0);
			check_remove_old(&HC, &ip_hdr->saddr, 1);
			ip_hdr->saddr = htonl(ip_addr);
			check_add_new(&HC, &ip_hdr->saddr, 0);
			check_add_new(&HC, &ip_hdr->saddr, 1);
			check_remove_old(&HC, &ip_hdr->ttl, 0);
			ip_hdr->ttl = 64;
			check_add_new(&HC, &ip_hdr->ttl, 0);
			ip_hdr->check = HC;
			HC = icmp_hdr->checksum;
			check_remove_old(&HC, &icmp_hdr->type, 0);
			icmp_hdr->type = ICMP_ECHOREPLY;
			check_add_new(&HC, &icmp_hdr->type, 0);
			icmp_hdr->checksum = HC;
			send_packet(m.interface, &m);
			return 1;
		}
	}
	if (iAmReceiver)
		return -1;
	memcpy(eth_hdr->ether_dhost, arpentry->mac, 6);
	memcpy(eth_hdr->ether_shost, mac_addr, 6);
	send_packet(entry->interface, &m);
	return 1;
}