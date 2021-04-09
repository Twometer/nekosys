#include <stdio.h>
#include <nekosys.h>
#include <errno.h>

int main(int argc, char **argv)
{
	if (argc == 0) {
		printf("Running as host\n");
		int shbuf = shbuf_create(128);
		printf("shbuf created: %d\n", shbuf);

		uint8_t *buf;
		shbuf_map(shbuf, (void**) &buf);

		buf[0] = 'H';
		buf[1] = 'E';
		buf[2] = 'Y';
		buf[3] = '\0';

		for (;;) ;
	} else {
		int i = argv[0][0] - '0'; // The world's most ugly int parser
		printf("Running as client with %d\n", i);

		uint8_t *buf;
		int res = shbuf_map(i, (void**) &buf);
		if (res != 0) {
			printf("error mapping shbuf %d\n", i);
			return 0;
		}

		printf("mapped to %x\n", buf);

		printf("contents: %s\n", buf);
	}

	return 0;
}
