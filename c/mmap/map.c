#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>

int main(int argc, char const* argv[])
{
    int fd;
    int *value;

    fd = open("./mapping", O_RDWR);
    value = (int*) mmap(NULL, sizeof(fd), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (value < 0) {
        perror("mmap failed");
    } else {
        printf("address %p\n", value);
        printf("value   %d\n", *value);
    }
    (*value)++;
    printf("value   %d\n", *value);
    return 0;
}
