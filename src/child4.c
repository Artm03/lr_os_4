#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_BUF 65534

int main(int argc, char* argv[]) {
    if (argc != 2) {
        perror("Wrong number of arguments\n");
        return 1;
    }
    char* tmp_name = argv[0];
    int tmp = open(tmp_name, O_RDWR, S_IRWXU);
    if (tmp < 0){
		perror("Problems with file\n");
		return 1;
	}
    struct stat buff;
    stat(tmp_name, &buff);
    char* fname = argv[1];
    int f = open(fname, O_CREAT|O_RDWR, S_IRWXU);
    if (f < 0){
		perror("Problems with file\n");
		return 1;
	}
    if(ftruncate(f, buff.st_size) < 0){
		perror("Tmp file can not filled\n");
		return -7;
	}
    char* buf = mmap(0, buff.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, tmp, 0);
    char* chd = mmap(0, buff.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, f, 0);
    close(tmp);
    close(f);
    int i = 0;
    while (i < buff.st_size) {
        char* str = malloc(MAX_BUF);
        int ind = 0;
        while (buf[i] != '\n') {
            str[ind] = buf[i];
            ind++;
            i++;
        }
        str[ind] = '\n';
        ind++;
        i++;
        int l = strlen(str); 
        str[l] = '\0';
        for (int j = 0; j <= (l - 2) / 2; j++) {
            char k = str[l - 2 - j];
            str[l - 2 - j] = str[j];
            str[j] = k;
        }
        for (int j = 0; j < strlen(str) - 1; j++) {
            chd[i - ind + j] = str[j];
            //printf("%c ", chd[i - ind + j]);
        }
        //printf("\n");
        chd[i - 1] = '\n';
    }
    remove(tmp_name);
    return 0;
}