#include "types.h"
#include "stat.h"
#include "user.h"

char buf[5120000];
char *args;
char *args2;

int uniq(int fd, char *name) {
    int final = 1, k, p, q, index = 0, m = 0, l = 0, b, n, i, count = 0, count1 = 0; 
    int temp = 0; int flag = 0;
    char pilot[10000];
    char *output[10000];
	int repeat[1000];
	
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        for(b = 0; b < n; b++) {
            if(buf[b] == '\n')
                l++;    // number of lines
        }
        for(i = 0; buf[i] != '\n'; i++) {
            count++;    // number of characters of the line
            pilot[i] = buf[i];
        }   
    	pilot[i] = '\0';
		repeat[0] = 1;
        output[0] = (char*)malloc((count + 1) * sizeof(char*));
        for(i = 0; i < count + 1; i++) {
            output[index][i] = pilot[i];
        }
		output[index][i] = '\0';
        k = i;  // k is the 'global' character position indicator

        // deal with the rest of the lines
        while(final <= l) {
        	count1 = 0;
            if (final != l) {
                for (i = k; buf[i] != '\n'; i++) {   // start with the new line
                    count1++;
                    pilot[m++] = buf[i];
        	    }
            } else {
                for (i = k; i <=  n; ++i) {
                    count1++;
                    pilot[m++] = buf[i];
                }
            }
            pilot[m] = '\0';
            k = k + count1 + 1;
            m = 0;
        	if (strcmp(output[index], pilot) != 0) {   // if the line is different from its previous line
                index = index + 1;
                output[index] = (char*)malloc((count1 + 1) * sizeof(char*));
                for(p = 0;p < count1; p++) {
                    output[index][p] = pilot[p];
                }
                output[index][p] = '\0';
                repeat[index] = 1;
        	} else {
			    repeat[index] = repeat[index] + 1;
            }
            final++;
	    }
	}
    if (((strcmp(args, "-c") == 0 || strcmp(args, "-C") == 0) && (strcmp(args2, "-i") == 0 || strcmp(args2, "-I") == 0)) || 
            ((strcmp(args2, "-c") == 0 || strcmp(args2, "-C") == 0) && (strcmp(args, "-i") == 0 || strcmp(args, "-I") == 0))) {
        char* compare = output[0];
        int idx = 0;
        char* current;
        for(q = 1; q < index + 1; q++)  {
            current = output[q];   
            for (temp = 0; compare[temp] != '\0' && current[temp] != '\0'; ++temp) {
                if (compare[temp] - current[temp] == 32 || current[temp] - compare[temp] == 32 || current[temp] == compare[temp])   
                    flag = 1;
                else    
                    flag = 0;
                if (flag == 0)
                    break;
            }
            if (flag == 1) {
                repeat[idx] = repeat[idx] + 1;
                flag = 0;
                continue;
            }
            printf(1, "%d %s \n", repeat[idx], output[idx]);
            compare = current;
            idx = q;
        }
        printf(1, "%d %s \n", repeat[idx], output[idx]);
    } else if (strcmp(args, "-c") == 0 || strcmp(args, "-C") == 0) {
		for(q = 0; q < index + 1; q++) {
            printf(1, "%d %s \n", repeat[q], output[q]);
        }
    } else if (strcmp(args, "-d") == 0 || strcmp(args,"-D") == 0) {
	    for(q = 0; q < index + 1; q++) {
		    if(repeat[q] > 1) {	
              	printf(1, "%s \n", output[q]);
		    }
	    }
    } else if (strcmp(args, "-i") == 0 || strcmp(args, "-I") == 0) {
        char* compare = output[0];
        printf(1, "%s \n", output[0]);
        char* current;
        for(q = 1; q < index + 1; q++)  {
            current = output[q];   
            for (temp = 0; compare[temp] != '\0' && current[temp] != '\0'; ++temp) {
                if (compare[temp] - current[temp] == 32 || current[temp] - compare[temp] == 32 || current[temp] == compare[temp])   
                    flag = 1;
                else    
                    flag = 0;
                if (flag == 0)
                    break;
            }
            if (flag == 1) {
                flag = 0;
                continue;
            }
            compare = current;
            printf(1, "%s \n", output[q]);
        }
    } else {
	    for(q = 0; q < index + 1; q++)  {
            printf(1, "%s \n", output[q]);
        }
	}
    for(q = 0; q < index + 1; q++)  {
        free(output[q]);
    }
	// free(output); 
    return 0;
}

int main(int argc, char *argv[]) {
    int fd;
    if (argc <= 1) {
        uniq(0, "");
        exit();
    }
    else if (argc == 2) {   // uniq FILE
        if((fd = open(argv[1], 0)) < 0) {
            printf(1, "uniq: cannot open %s\n", argv[1]);
            exit();
        }
        uniq(fd, argv[1]);
        close(fd);       
        exit();
    }
    else if (argc == 3) {  // uniq -x FILE
        if((fd = open(argv[2], 0)) < 0) {
            args = argv[1];
            args2 = argv[2];
            uniq(0, "");
            close(fd);
            exit();
        }
        args = argv[1];
        uniq(fd, argv[2]);
        close(fd);
        exit();
    }
    else if (argc == 4) {  // uniq -x -x FILE
        if((fd = open(argv[3], 0)) < 0) {
            printf(1, "uniq: cannot open %s\n", argv[3]);
            exit();
        }
        args = argv[1];
        args2 = argv[2];
        uniq(fd, argv[3]);
        close(fd);
        exit();
    }
}