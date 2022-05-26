#include "kernel/types.h" 
#include "kernel/stat.h" 
#include "user.h"


int main(int argc, char **argv) { 
    int niceValue = nice(7);
    printf(1, "After setting the nice value, it gets: %d\n", niceValue);
    exit();
}