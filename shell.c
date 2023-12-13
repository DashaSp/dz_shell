#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
# define M 10

int max1 = 0;
char *get_str(FILE *f){             // функция считывает строку
    int n, k = 0;
    char *s = malloc(M);
    if (!s){
        return NULL;
    }
    while (fgets(s + k, M, f)){
        n = strlen(s);
        if ( s[n - 1] != '\n'){
            k = k + M - 1;
            s = realloc(s, k + M);
            max1++;
            if (!s){
                return NULL;
            }
        }else{
            s[n - 1] = '\0';
            return s;
        }
    }
    free(s);
    return NULL;
}

char* change(char *str){
    long long m = strlen(str) - 3;
    char *ans = malloc(sizeof(char)*(m + 1));
    for (int i = 0; i < m; i++){
        ans[i] = str[i + 3];
    }
    ans[m] = '\0';
    return ans;
}

char* creat_str(char *str, int i, int j){           // функция для создания строки
    long long m = j - i;
    char *ans = malloc(sizeof(char)*(m+1));
    for (int t = 0; t<m; t++){
        ans[t] = str[t + i];
    }
    ans[m] = '\0';
    return ans;
}

int check(char s){                    // функция для проверки
    switch(s){
        case '&':
        case '|':
        case ';':
        case '>':
        case '<':
        case '(':
        case ')':
        case '\t':
        case ' ':
        case '\0':
        return 1;
        break;
        default:
        return 0;
        break;
    }
}

int count(char *str, int j){          
    int ans = strlen(str) - 1;
    for(int i = j + 1; i<strlen(str); i++){
        if(str[i] == '"'){
            ans = i;
            return ans;
        }
    }
    return ans;
}


int check_str(char *str, int i1, int j){          
    int ans = 0;
    for(int i = i1; i<j; i++){
        if(str[i] == '"'){
            ans++;
        }
    }
    return ans;
}

char* creat_str1(char *str, int i, int j){           // функция для создания строки (с убирание ")
    long long h = check_str(str, i, j);
    long long m = j - i - h;
    int k = 0;
    char *ans = malloc(sizeof(char)*(m+1));
    for (int t = 0; t<m; t++){
        if (str[t + i + k]=='"'){
            k++;
        }
        ans[t] = str[t + i + k];
    }
    ans[m] = '\0';
    return ans;
}

void printf_str(int n, char **str){        // функция для вывода строчек из массива
    for ( int i = 0; i<n; i++){
        printf("%s\n", str[i]);
    }
}

void free_str(char **str_ans, int i1){         // функция освобождания массива строк
    for(int i = 0; i < i1; i++){
        free(str_ans[i]);
    }
    free(str_ans);
}

char** creat_string(char **str_ans, int n){
    char **ans = malloc(sizeof(char*)*(n-1));
    for (int i = 0; i<n - 1; i++){
        ans[i] = str_ans[i + 1];
    }
    return ans;
}

int exec1(char **str, int i1)
{
    pid_t pid;
    int status;
    pid = fork();
    
    if (pid == 0) {          // сыновий процесс
        if (execvp(str[0], str) == -1) {
        perror(0);
        }
        exit(0);
    } else if (pid < 0) {         // ошибка fork
        perror(0);
    } else {                     // родительский процесс
        do {
        waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    
    return 0;
}

int exec_cd(char **str) {
	if ((str[1]== NULL) || (str[2] == NULL)) {
		if (str[1] == NULL){          //go to the home directory
			if (chdir(getenv("HOME"))) {
				perror("Error1");
				return 1;
			}
		}else{
            if (chdir(str[1])){  //go to the directory
			perror("Error2");
			return 1;
            }
        }
		printf("Current directory is %s\n", getcwd(NULL, 0));	
        return 0;
	}
	else {
		fprintf(stderr, "invalid\n");
		return 1;
	}
	return 1;
}


void conv(char **str_ans, int i1) {            // конвейер
    int k = -1, fd[2];
    char **tmp = NULL;

    for (int i = 0; i < i1; i++)
        if (!strcmp(str_ans[i], "|")){
            str_ans[i] = NULL;
        } 

    while (++k < i1){
        if (k != 0 && str_ans[k] != NULL){
            continue;
        }
        tmp = str_ans + k + 1 - (k == 0);
        pipe(fd);
        switch(fork()){
            case -1: 
                fprintf(stderr, "Error\n");
                break;
            case 0:
                if (k + 2 - (k == 0) != i1) dup2(fd[1], 1);
                close(fd[1]);
                close(fd[0]);
                execvp(tmp[0], tmp);
                fprintf(stderr, "Invalide command\n");
                exit(1);  
            default:
                dup2(fd[0], 0);
                close(fd[0]);
                close(fd[1]);
                break;
        }
    }
    while(wait(NULL) != -1);
}

void sigHndlr(char *str[]) {
    if ((str[1] != NULL) && (strcmp(str[1], "&") == 0)) {
        signal(SIGINT, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
    } else {
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
    }
}

int main(int argc, char ** argv){ 
    FILE *f1 = stdin; 
    FILE *f2 = stdout;
    int chnl0 = dup(0), chnl1 = dup(1);
    int m = 10, f = 0;
    char *str = malloc(sizeof(char)*m);
    char **str_ans = malloc(sizeof(char*)*m);

    for (int i = 0 ; i < argc; i++) {
        if (strcmp(argv[i], "-i") == 0) {
            f = 1;
            if ((f1 = fopen(argv[i + 1], "r")) == NULL) {
                perror(argv[i + 1]);
                exit(0);                
            }
        } else if (strcmp(argv[i], "-o") == 0) {
            if ((f2 = fopen(argv[i + 1], "w")) == NULL) {
                perror(argv[i + 1]);
                exit(1);                
            }            
        }
    }

    int i1; 
    while ((str = get_str(f1)) != NULL){                        // считали строк
        i1 = 0;
        char **str_ans = malloc(sizeof(char*)*m);
        if (f == 1){
            str = change(str);
            f = 0;
        }
        
        long long len = 0;
        len = strlen(str);
        int start = 0;
        int g = 0;
        for (int i = 0; i<(len + 1); i++){
            if (((str[i]=='\t')||(str[i]==' '))&&((str[i-1]=='\t')||(str[i-1]==' '))){         // не обращаем внимания на множество пробелов
                start++;
                continue;
            }
            if (str[i] == '"'){            // считываем пока не найдем " на строчке
                /*if (start == j){
                    start++;
                }else{
                    str_ans[i1] = creat_str(str[i], start, j);       // строка
                    i1++;
                    start = j;
                }*/
                int q;
                for(int h = i; h < len + 1; h++){    
                    q = count(str, h);
                    if ((str[q + 1] == ' ')||(str[q + 1] == '\t')||(str[q + 1] == '\0')){
                        str_ans[i1] = creat_str1(str, start, q + 1);
                        i1++;
                        if (i1 >= m){
                            m = m * 2;
                            str_ans = realloc(str_ans, sizeof(char*)*m);
                        }
                        start = q + 1;
                        break;
                    }else{
                        h = q + 1;
                    }
                }
                start = q + 1;
                i = q;
            }
            if (check(str[i]) == 1){
                if ((str[i] == '\0')&&(start != i)){           // конец строки
                    str_ans[i1] = creat_str(str, start, i);
                    i1++;
                    if (i1 >= m){
                        m = m * 2;
                        str_ans = realloc(str_ans, sizeof(char*)*m);
                    }
                    break;
                }
                
                if ((str[i] == '\0')&&(start == i)){      // ситуация "|\0"
                    break;
                }
                
                if((start == i)&&((str[i]=='\t')||(str[i]==' '))){
                    start++;
                    continue;
                }
                if((start == i)&&(g == 0)){                  // первое вхождение
                    if (((check(str[i+1]) == 1) &&(str[i+1] == str[i]))&& ((str[i] != '\t')||(str[i] != ' '))){     //&&, ||, <<, ...
                        str_ans[i1] = creat_str(str, i, i + 2);
                        i1++;
                        if (i1 >= m){
                            m = m * 2;
                            str_ans = realloc(str_ans, sizeof(char*)*m);
                        }
                        start = start + 2;
                        i++;
                        continue;
                    }
                    if (((str[i] != '\t')||(str[i] != ' '))){    // &, <, ; ...
                        str_ans[i1] = creat_str(str, i, i + 1);
                        i1++;
                        if (i1 >= m){
                            m = m * 2;
                            str_ans = realloc(str_ans, sizeof(char*)*m);
                        }
                        start++;
                        continue;
                    }
                }else{
                    g = 1;
                    if(start != i){
                        str_ans[i1] = creat_str(str, start, i);       // строка
                        i1++;
                        if (i1 >= m){
                                m = m * 2;
                                str_ans = realloc(str_ans, sizeof(char*)*m);
                        }
                    }
                    if ((str[i] == '\t')||(str[i] == ' ')){
                        start = i + 1;
                        continue;
                    }
                    if (((check(str[i+1]) == 1)&&(str[i+1] == str[i]))&&((str[i] != '\t')||(str[i] != ' '))){     //&&, ||, <<, ...
                        str_ans[i1] = creat_str(str, i, i + 2);
                        i1++;
                        if (i1 >= m){
                            m = m * 2;
                            str_ans = realloc(str_ans, sizeof(char*)*m);
                        }
                        start = i + 2;
                        i++;
                        continue;
                    }
                    if ((str[i] != '\t')||(str[i] != ' ')){     // &, <, ; ...
                        str_ans[i1] = creat_str(str, i, i + 1);
                        i1++;
                        if (i1 >= m){
                            m = m * 2;
                            str_ans = realloc(str_ans, sizeof(char*)*m);
                        }
                        start = i + 1;
                        continue;
                    }
                }
            }
        }
        if((i1==0)||(str_ans[0] == NULL)){
            free(str);
            continue;
        } 
        sigHndlr(str_ans);     
        int fd1 = 0, fd2 = 1;
        for(int i = 1; i<(i1-1); i++){
            if (((strcmp(str_ans[i], "<") == 0) || (strcmp(str_ans[i], ">") == 0) || (strcmp(str_ans[i], ">>") == 0))) {
                if (str_ans[i + 1]) {
                    if (strcmp(str_ans[i], "<")) {
                        if ((fd1 = open(str_ans[i + 1], O_RDONLY)) == -1) {
                            perror("Error");
                            exit(1);
                        }
                        if (dup2(fd1, 0) == (-1)) {
                            perror("Error");
                            exit(1);
                        }
                        if (close(fd1) == -1) {
                            perror("Error");
                            exit(1);
                        }
                    }
                    if (strcmp(str_ans[i], ">")) {
                        if (((fd2 = open(str_ans[i + 1], O_WRONLY|O_CREAT|O_TRUNC)) == (-1))) {
                            perror("Error");
                            exit(1);
                        }
                        if (dup2(fd2, 1) == -1) {
                            perror("Error");
                            exit(1);                            
                        }
                        if (close(fd2) == -1) {
                            perror("Error");
                            exit(1);
                        }
                    }
                    if (strcmp(str_ans[i], ">>")) {
                        if (((fd2 = open(str_ans[i + 1], O_WRONLY|O_CREAT|O_APPEND)) == (-1))) {
                            perror("Error");
                        }
                        if (dup2(fd2, 1) == (-1)) {
                            perror("Error");
                        }
                        if (close(fd2) == (-1)) {
                            perror("Error");
                        }
                    }
                }
                if (fd1 == -1){
                    close(fd1);
                }else{ 
                    dup2(fd1, 0);
                }
                if (fd2 == -1){ 
                    close (fd2);
                }else{ 
                    dup2(fd2, 1);
                }
                for (int j = i; str_ans[j] != NULL; j++) {
                    str_ans[j] = str_ans[j + 2];
                }
            }
        }
        for(int i = 1; i<(i1-1); i++){
            if((str_ans[i] != NULL) && (strcmp(str_ans[i], "&") == 0)){
                int pid;
                if ((pid = fork()) == -1) {
                    perror("Error");
                }
                else if (pid == 0){
                    switch (fork()) { 
                        case -1:
                            perror("Error");
                            exit(1);
                        case 0:
                            sigHndlr(str_ans); 
                            str_ans[i] = NULL;
                            if (execvp(str_ans[i], str_ans) == -1){
                                perror("Error");
                                exit(1);
                            }
                        default:
                            exit(0);
                    }
                }
                dup2(chnl0, 0);
                dup2(chnl1, 1);
            }
        }
        if ((str_ans[1] != NULL) && (strcmp(str_ans[1], "|") == 0)){
            int chnl0 = dup(0), chnl1 = dup(1);
            conv(str_ans, i1);
            dup2(chnl0, 0);
            dup2(chnl1, 1);
        } else {
            char *s = "cd";
            if (strcmp(str_ans[0], s) == 0){
                exec_cd(str_ans);
            }else{
                exec1(str_ans, i1);
            }
        }
        free_str(str_ans, i1);
        free(str);
        dup2(chnl0, 0);
        dup2(chnl1, 1);
        dup2(fd1, 0);
        dup2(fd2, 1);
    }
    free(str);
    free(str_ans);
    return 0;
}