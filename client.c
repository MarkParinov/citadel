/* CITADEL CLIENT source code */
/* std */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/* string */
#include <string.h>
/* inet */
#include <sys/socket.h>
#include <arpa/inet.h>

#define _PORT 100
#define _ADDR "192.168.4.1"

#define LASER_FLAG 1 << 0;

struct Req_Packet {
	uint8_t magic;
	uint8_t flags;
	uint8_t x1, z1;
	uint8_t x2, z2;
	uint8_t x3, z3;
	uint8_t x4, z4;
} __attribute__((__packed__));

size_t send_packet(struct Req_Packet* _pckt) {
	int sock;
	struct sockaddr_in _addr;
	
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket creation failed");
		return 1;
	}

    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(_PORT);

	if (inet_pton(AF_INET, _ADDR, &_addr.sin_addr) <= 0) {
		perror("invalid/not supported address");
		close(sock);
		return 1;
	}

	if (connect(sock, (struct sockaddr *)&_addr, sizeof(_addr)) < 0) {
		perror("failed to connect");
		close(sock);
		return 1;
	}

	/* send the data */
	size_t bytes_sent = send(sock, _pckt, sizeof(*_pckt), 0);

	close(sock);

	return bytes_sent;
}

int check_cmd(char* _cmd, char* _ptrn, size_t _size) {
	for (size_t i = 0; i < _size; i++)
		if (_cmd[i] != _ptrn[i])
			return 0;

	return 1;
}
/*
void parse_command(char* _cmd, struct Req_Packet* _pckt) {
	printf("cmd = '%s'\n", _cmd);
	if (check_cmd(_cmd, "set", 3) && _cmd[5]) {
		size_t i = 5;
		size_t buffer_i = 0;
		char x_buffer[3];
		char z_buffer[3];
		
		while (_cmd[i] != ' ') {
			x_buffer[buffer_i] = _cmd[i];
			buffer_i++; i++;
		}
		
		buffer_i = 0;
		while (_cmd[i] != ' ') {
			z_buffer[buffer_i] = _cmd[i];
			buffer_i++; i++;
		}

		printf("parsed values; x angle='%s'; z angle='%s'\n", x_buffer, z_buffer);
	}
}
*/
int main(int argc, char* argv[]) {

	if (argc < 2)
		exit(1);

	struct Req_Packet pckt;
	pckt.magic = 0xcd;
	/*
	char* cmd = (char*)malloc(128);
	getline(stdin, cmd);
	
	while (strcmp(cmd, "exit")) {
		parse_command(cmd, &pckt);
		printf("scanning...\n");
		scanf("%s", cmd);
	}*/

	int code;
	
	pckt.x1 = atoi(argv[1]);
	pckt.z1 = atoi(argv[2]);

	pckt.x2 = atoi(argv[3]);
	pckt.z2 = atoi(argv[4]);

	pckt.x3 = atoi(argv[5]);
	pckt.z3 = atoi(argv[6]);

	pckt.x4 = atoi(argv[7]);
	pckt.z4 = atoi(argv[8]);

	if (atoi(argv[9]))
		pckt.flags |= LASER_FLAG;

	code = send_packet(&pckt);
	printf("sent %d bytes\n", code);

	printf("end\n");
	/*free(cmd);*/
	exit(0);
}
