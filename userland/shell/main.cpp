#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	printf("Opening file...\n");
	FILE *file = fopen("/home/neko/test.txt", "r");
	printf("opened\n");

	fseek(file, 0, SEEK_END);
	size_t filesize = ftell(file);
	fseek(file, 0, SEEK_SET);
	printf("Test file has size %d.\n", filesize);

	uint8_t *data = new uint8_t[filesize + 1];
	data[filesize] = 0;

	fread(data, 1, filesize, file);

	printf("File test contents: %s\n", data);

	fclose(file);
	delete[] data;

	printf("sleeping for 5 secs...\n");
	sleep(5);
	printf("yay, we're back\n");

	return 0;
}