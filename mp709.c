#include <linux/types.h>
#include <linux/input.h>
#include <linux/hidraw.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

typedef int bool;
#define true 1
#define false 0

int main(int argc, char **argv) {
	int fd, res;
	char buf[256];
	char dev_name[32];
	bool has_device = false;
	struct stat st;

	int i;
	for (i=0; i<9; ++i) {
		sprintf(dev_name, "/dev/hidraw%i", i);

		if (stat(dev_name, &st))
			break;

		fd = open(dev_name, O_RDWR|O_NONBLOCK);

		if (fd < 0) {
			perror("Unable to open device");
			continue;
		};

		res = ioctl(fd, HIDIOCGRAWNAME(256), buf);
		if (res < 0)
			perror("Cannot get device name");
		else
			printf("/dev/hidraw%i: %s\n", i, buf);

		if (strstr(buf, "MP709") != NULL) {
			has_device = true;
			break;
		} else {
			close(fd);
		};
	};

	if (!has_device) {
		printf("No device found\n");
		return 1;
	};

	buf[0] = 0xE7;

	if (argc > 1 && strcasecmp(argv[1], "on") == 0)
		buf[1] = 0x0;
	else
		buf[1] = 0x19;

	res = write(fd, buf, 2);
	if (res < 0) {
		printf("Error: %d\n", errno);
		perror("write");
	};

	close(fd);

	return 0;
}
