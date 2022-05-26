#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "kernel/syscall.h"     // For strace dump

char buf [1024];

// int readDumpFile(int num) {
//     int fd = 0;
//     int n;
//     int i;
//     fd = open("/kernel/test.txt", 0);
//     if (fd < 0) {
//         printf(1, "strace dump: cannot open the file.\n");
//         return -1;
//     }
//     while((n = read(fd, buf, sizeof(buf))) > 0 ) {
//         for (i = 0; i < n; i++) {
//             printf(1, "%c", buf[i]);
//         }
//     }
//     return 1;
// }

static char *syscall_names[] = {
    "null", "fork",  "exit", "wait",
    "pipe",  "read",  "kill",
    "exec",  "fstat", "chdir",
    "dup",   "getpid",  "sbrk",
    "sleep",  "uptime",  "open",
    "write",   "mknod",    "unlink",
    "link",    "mkdir",    "close",
    "trace",
};

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        printf(1, "Incorrect Parameters.\n");
        exit();
    }
    if (argc == 2 && strcmp(argv[1], "on") == 0) {
        trace(1);   //turn on the strace mode
    }
    if (argc == 2 && strcmp(argv[1], "off") == 0) {
        trace(0);   //turn off the strace mode
    }
    if (argc == 2 && strcmp(argv[1], "dump") == 0) {
        trace(-2);
        trace(-2);  //used for strace dump
    }
    if (argc > 2 && strcmp(argv[1], "run") == 0) {
        trace(-1);   //turn on the strace mode; -1 specifies the strace run command
        char *temp[100];
        for (int i = 2; i < argc; ++i) {
            temp[i - 2] = argv[i];
        }
        exec(temp[0], temp);
    //    trace(0);   //turn off the strace mode
    }
    if (argc > 2 && strcmp(argv[1], "-e") == 0) {
        char* command = argv[2];
        for (int i = 0; i < sizeof(syscall_names) / sizeof(syscall_names[0]); ++i) {
            if (strcmp(command, syscall_names[i]) == 0) {
                // printf(1, "%d", i);
                helper(i);
            }
        }
    }
    if (argc == 2 && strcmp(argv[1], "-s") == 0) {
        helper(-1);
    }
    if (argc == 2 && strcmp(argv[1], "-f") == 0) {
        helper(-2);
    }
    
}