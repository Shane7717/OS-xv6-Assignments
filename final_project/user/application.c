#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "user.h"

char buf [1024];

int main() {
    int fd = 0;
    int n;
    int i;
    fd = open("not_exist.txt", 0);
    while((n = read(fd, buf, sizeof(buf))) > 0 ) {
        for (i = 0; i < n; i++) {
            printf(1, "%c", buf[i]);
        }
    }
    return 1;
}