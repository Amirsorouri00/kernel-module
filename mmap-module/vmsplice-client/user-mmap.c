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
	int data_to_write_fd = open("../../../random-files/2k-files/file.txt", O_RDONLY);
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
	// clock_t start_t, end_t, total_t;
    int pbuf[2];
    if (pipe(pbuf) < 0) 
        exit(1); 
	// start_t = clock();
    struct iovec local; 
    local.iov_base = address1;
    local.iov_len = page_size;
	while(1) {
        ssize_t nread = vmsplice(data_to_write_fd, &local, page_size, SPLICE_F_MORE);
        if( 0 == nread ){
            break;
        }
    }

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