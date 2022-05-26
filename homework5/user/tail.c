#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/fcntl.h"

char buf [1024];

void tail (int fd, int num) {
    int i;
    int n;
    int line = 0;
    int npLine = 0;
    int count = 0;
    int temp = open("temp", O_CREATE | O_RDWR);
  

    while((n = read(fd, buf, sizeof(buf))) > 0 ) {
        // write to this temporary file
        write (temp, buf, n);                              

        for (i = 0; i <= n ;i++) {
            if(buf[i] != '\n') {                
                continue;
            } else {           
                // count the number of lines                              
                line++;
            }
        }
    }
    line--;

    close (temp);

    if (n < 0) {
        printf(1, "error: can't read!\n");
        exit();
    }

    temp = open ("temp", 0);

    if (line < num) npLine = 0;
    else    npLine = line - num;

    while((n = read(temp, buf, sizeof(buf))) > 0 ) {
        for (i = 0; i < n; i++) {
            if (count > npLine) {
                if (buf[i] == '\n'  && buf[i - 1] == '\n'){
                    // do nothing, skip the extra blank lines.
                } else {
                    printf(1, "%c", buf[i]); 
                }
            } 
            else if (buf[i] == '\n') {
                count++;
            }
        }
    }
    close (temp);    
    unlink("temp");                  
}


int 
main(int argc, char *argv[]) {
    int fd = 0;
    int nl = 10;
    char c;
    int i;

    // if there's no argument passed, read from standard input
    if (argc <= 1) {
        tail(fd, nl);
        exit();
    } else {
        for (i = 1; i < argc; ++i) {
            c = *argv[i];
            if (c == '-') {
                argv[i]++;
                nl = atoi(argv[i]++);
            } else {
                if ((fd = open(argv[i], 0)) < 0) {
                    printf(1, "tail: cannot open %s\n", argv[i]);
                    exit();
                }
            }
        }

        tail(fd, nl);
        close(fd);
        exit();
    }
}