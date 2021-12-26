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

int main() {
    char* fname1;
    char* fname2;
    fname1 = malloc(sizeof(char) * MAX_BUF);
    if (read(0, fname1, MAX_BUF) < 0) {
        perror("ERROR\n");
        return 1;
    }
    int l1 = strlen(fname1);
    fname1[l1 - 1] = '\0';
    fflush(stdout);
    fname2 = malloc(MAX_BUF);
    if (read(0, fname2, MAX_BUF) < 0) {
        perror("ERROR\n");
        return 1;
    }
    int l2 = strlen(fname2);
    fname2[l2 - 1] = '\0';
    fflush(stdout);
    if (strcmp(fname1, fname2) == 0) {
        perror("The files have the same names\n");
		return 1;
    }
    char tmp_name1[] = "/tmp/tmpXXXXXX";
    int tmp1 = mkstemp(tmp_name1);
    if (tmp1 < 0){
		perror("Problems with file\n");
		return 1;
	}
    char tmp_name2[] = "/tmp/tmpXXXXXX";
    int tmp2 = mkstemp(tmp_name2);
    if (tmp2 < 0){
		perror("Problems with file\n");
		return 1;
	} 
    char c = ' ';
    int cnt_str = 0;
    int ind = 0;
    while (c != EOF) {
        char* str;
        str = malloc(MAX_BUF);
        c = ' ';
        int i = 0;
        bool is_eof = false;
        while (c != '\n') {
            if (read(0, &c, sizeof(char)) < 1){
                is_eof = true;
                break;
            }
            str[i] = c;
            i++;
            ind++;
        }
        ind++;
        cnt_str++;
        int l = strlen(str); 
        str[l] = '\0';
        if (cnt_str % 2 == 1) {
            write(tmp1, str, sizeof(char) * strlen(str));
        }
        if (cnt_str % 2 == 0) {
            write(tmp2, str, sizeof(char) * strlen(str));
        }
        if (is_eof) {
            break;
        }
    }
    close(tmp1);
    close(tmp2);
    int pid1 = fork();
    if (pid1 == -1) {
        perror("Fork error\n");
        return 1;
    }
    if (pid1 != 0) {
        int pid2 = fork();
        if (pid2 == -1) {
            perror("Fork error\n");
            return 1;
        }  
        if (pid2 == 0) {
            if (execl("child", tmp_name2, fname2, NULL) == -1) {
                perror("Execl child problem");
                return 2;
            } 
        }      
    }
    else {
        if (execl("child", tmp_name1, fname1, NULL) == -1) {
            perror("Execl child problem");
            return 2;
        }
    }
    return 0;
}
