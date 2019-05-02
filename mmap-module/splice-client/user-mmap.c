#define _XOPEN_SOURCE 700
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h> /* uintmax_t */
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h> /* sysconf */

#include "../common.h" /* virt_to_phys_user */

#define TRIALS		1000000000

enum { BUFFER_SIZE = 1000 };

int main(int argc, char **argv)
{
	int fd;
	long page_size;
	char *address1, *address2;
	char buf[BUFFER_SIZE];
	uintptr_t paddr;


	if (argc < 2) {
		printf("Usage: %s <mmap_file>\n", argv[0]);
		return EXIT_FAILURE;
	}
	page_size = sysconf(_SC_PAGE_SIZE);
	printf("open pathname = %s\n", argv[1]);
	fd = open(argv[1], O_RDWR | O_SYNC);
	int data_to_write_fd = open("../../../random-files/2m-files/file.txt", O_RDONLY);
	if (fd < 0 || data_to_write_fd < 0) {
		perror("open");
		assert(0);
	}
	printf("fd = %d\n", fd);
	printf("data_to_write_fd = %d\n", data_to_write_fd);

    /* mmap the file */
	puts("mmap 1");
	address1 = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (address1 == MAP_FAILED) {
		perror("mmap");
		assert(0);
	}

	/* Check that the physical addresses are the same.
	 * They are, but TODO why virt_to_phys on kernel gives a different value? */
	assert(!virt_to_phys_user(&paddr, getpid(), (uintptr_t)address1));
	printf("paddr1 = 0x%jx\n", (uintmax_t)paddr);

	size_t cnt = 0;
	clock_t start_t, end_t, total_t;
    int pbuf[2];
    if (pipe(pbuf) < 0) 
        exit(1); 
	start_t = clock();
	printf("Starting of the program, start_t = %ld\n", start_t);
    // printf("1111111111111111\n");
    // int bt = 0;
	while(1) {
        // printf("1\n");
        size_t bt = splice(data_to_write_fd, NULL, pbuf[1], NULL, BUFFER_SIZE, SPLICE_F_MOVE);
        // printf("2\n");
        if (bt == 0) {
            break;
        }
        // printf("bt: %d\n", (int)bt);
        // printf("3\n");
		cnt += splice(pbuf[0], NULL, fd, NULL, BUFFER_SIZE, SPLICE_F_MOVE);
        // printf("cnt: %d\n", (int)cnt);
    }
	end_t = clock();
	printf("End of the big loop, end_t = %ld\n", end_t);
	
	total_t = ((double)(end_t - start_t)) / CLOCKS_PER_SEC;
	printf("Time: %f\n", (double)total_t);
    // printf("number of bytes copied: %d\n", cnt);

    /* Cleanup. */
    puts("munmap 1");
	if (munmap(address1, page_size)) {
		perror("munmap");
		assert(0);
	}
    puts("close");
	close(fd);
	return EXIT_SUCCESS;
}