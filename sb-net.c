#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define INTERFACE "eth0"

int getbytesfrom(char* ifname){
	char buf[128];
	int fd;
	int bytes;
	char path[] = "/sys/class/net/"INTERFACE"/statistics/rx_bytes"; 
	fd = open(path, O_RDONLY);
	int len = read(fd, buf, sizeof(buf)-1);
	close(fd);

	if (len < 0) {
		bytes = 0;
	}
	else {
		bytes = atoi(buf);
	}
	return bytes;
}

int checkopstate() {
	char buf[16];
	int fd;
	int len;
	char path[] = "/sys/class/net/"INTERFACE"/operstate"; 
	fd = open(path, O_RDONLY);
	len = read(fd, buf, sizeof(buf)-1);
	close(fd);

	if (len > 0) {
		return strncmp("up", buf, 2);
	}
	else {
		return 1;
	}
}

int main(int argc, char **argv)
{
	int rx_bytes[2];
	int rx_delta = 0;
	int rx_kib;

	while (checkopstate() != 0) {
		printf("%s\n", "link down");
		sleep(10);
	}

	rx_bytes[1] = getbytesfrom(INTERFACE);
	for(;;) {
		sleep(1);
		rx_bytes[0] = rx_bytes[1];
		rx_bytes[1] = getbytesfrom(INTERFACE);
		rx_delta = rx_bytes[1] - rx_bytes[0];
		rx_kib = rx_delta >> 10;
		// ignore low band
		if (rx_delta < 1000) {
			printf(" \n");
			fflush(stdout);
			continue;
		}
		if (rx_delta > 1048576) { 
			printf("%d Mbps\n", rx_kib / 1024);
		}
		else {
			printf("%d kbps\n", rx_kib);
		}
    fflush(stdout);
	}
}
