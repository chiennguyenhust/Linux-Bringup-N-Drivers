#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_FILE "/dev/led_module_test"

int main(int argc, char* argv[])
{
    int fd = 0;
    ssize_t size = 0;

    fd = open(DEVICE_FILE, O_RDWR);
    if(fd < 0)
    {
        printf("Open file is failed\n");
        return -1;
    }

    /* Turn led */
    size = write(fd, argv[1], 1);
    if(size < 0)
    {
        printf("Write file failed\n");
        return -1;
    }

    close(fd);

    return 0;
}










