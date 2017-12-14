#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fring-user.h>
#include <iostream>

using namespace std;
class test_fr {

public:
    int open_close();
    int alloc_free(const char *name, int count, int size);
};

int test_fr::open_close() {

    int fd = open("/dev/fring", O_RDWR);

    if(fd < 0) {
        cout << "Failed to open /dev/fring" << endl;
        return -1;
    }
    close(fd);
    return 0;
}

int test_fr::alloc_free(const char *name, int count, int size) {
    int fd = open("/dev/fring", O_RDWR);
    struct fr_info fr_info;
    int ret;

    if(fd < 0) {
        cout << "Failed to open /dev/fring" << endl;
        return -1;
    }

    fr_info.count = count;
    fr_info.size = size;
    strncpy(fr_info.name, name, FR_SZ_NAME);
    cout << "FR handle init: " << fr_info.fr << endl;
    ret = ioctl(fd, FRING_ALLOC, &fr_info);
    if(ret < 0) {
        cout << "Failed to alloc buffer" << endl;
        goto end;
    }
    cout << "FR handle get : " << fr_info.fr << endl;
    ioctl(fd, FRING_FREE, fr_info.fr);

end:
    close(fd);
    return ret;
}

int main()
{
    test_fr tfr;

    tfr.open_close();
    tfr.alloc_free("testalloc", 4, 4 * 1024 * 1024);
    return 0;
}
