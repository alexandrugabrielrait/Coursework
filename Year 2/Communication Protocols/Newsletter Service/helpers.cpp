#include "helpers.hpp"

const char *identifier_name[] = {"INT", "SHORT_REAL", "FLOAT", "STRING"};
const int data_size[] = {5, 2, 6, CONTENTLEN};

int smsg_size(int8_t message_type, int8_t data_type)
{
	if (message_type == -1 || message_type == -2)
		return 1;
	//pentru anuntare succes folosim topic si type
	if (message_type == 1 || message_type == 2)
		return offset_after_type;
	return offset_after_type + data_size[data_type];
}

int cmsg_size(int8_t message_type)
{
	if (message_type == -1)
		return 12;
	return sizeof(client_message);
}

char *parse_notification_value(notification notif)
{
	char *parsed_string = (char *)malloc((CONTENTLEN + 1) * sizeof(char));
	switch (notif.data_type)
	{
	case 0:
	{
		uint8_t sign = *(notif.content);
		if (sign == 0)
			sprintf(parsed_string, "%u",
									ntohl(*(u_int32_t *)(notif.content + 1)));
		else
			sprintf(parsed_string, "-%u",
									ntohl(*(u_int32_t *)(notif.content + 1)));
		break;
	}
	case 1:
		sprintf(parsed_string, "%.2f",
								ntohs(*(uint16_t *)notif.content) / (float)100);
		break;
	case 2:
	{
		uint8_t sign = *(notif.content);
		int8_t power = (*(int8_t *)(notif.content + 5));
		float result = ntohl(*(uint32_t *)(notif.content + 1));
		int i;
		for (i = 0; i < power; i++)
			result /= 10;
		for (i = 0; i > power; i--)
			result *= 10;
		if (sign == 0)
			sprintf(parsed_string, "%.4f", result);
		else
			sprintf(parsed_string, "-%.4f", result);
		/*scoatem zerourile de la final, si punctul
		in cazul in care nu avem cifre nenule dupa el
		*/
		int index = strlen(parsed_string) - 1;
		while (parsed_string[index] == '0' || parsed_string[index] == '.')
		{
			parsed_string[index] = 0;
			index--;
		}
		break;
	}
	default:
		memcpy(parsed_string, notif.content, CONTENTLEN);
		//adaugam caracterul nul pentru ca el poate lipsi
		parsed_string[CONTENTLEN] = 0;
		break;
	}
	return parsed_string;
}