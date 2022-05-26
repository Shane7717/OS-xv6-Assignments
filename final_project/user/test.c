#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

void forkTest() {
	int fk = fork();
	if (fk == -1) {
		printf(1, "Fork error!\n");
		return;
	} else if (fk == 0) {
		close(open("README.md", 0));
		exit();
	} else {
		wait();
	}
}

int main() {
	printf(1, "The process is being traced.\n");
	trace(1);   // turn on the strace mode; 1 means turning on
	forkTest();

	trace(0);   // turn off the strace mode; 0 means turning off
	printf(1, "The processs & forks being traced.\n");
	trace(1 | 2);   // turn on the strace mode; 1 | 2 means setting strace for current process and also its any child created using fork.
	forkTest();

	trace(0);
	printf(1, "The process not being traced.\n");
	forkTest();

	exit();
}