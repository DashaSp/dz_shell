#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
        ans[t] = str[t+i];
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

int count(char **str, int i1, int j){          
    int ans = strlen(str[i1]) - 1;
    for(int i = j + 1; i<strlen(str[i1]); i++){
        if(str[i1][i] == '"'){
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
        if (str[t+i+k]=='"'){
            k++;
            //printf("%d", t+i+k);
        }
        ans[t] = str[t+i+k];
        //printf("%c ", ans[t]);
    }
    ans[m] = '\0';
    return ans;
}

void printf_str(int n, char **str){        // функция для вывода строчек из массива
    for ( int i =0; i<n; i++){
        printf("%s\n", str[i]);
    }
}

int main(int argc, char ** argv){ 
    
    FILE *f1 = stdin; 
    FILE *f2 = stdout;
    
    int i = 0, m = 10, f = 0;
    char **str = malloc(sizeof(char*)*m);

    for (int i = 0; i < argc; i++) {
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
    i = 0; 
    while ((str[i++] = get_str(f1)) != NULL){                        // считали строку
        if (i >= m){
            m = m * 2;
            str = realloc(str, sizeof(char*)*m);
        }
    }

    char **str_ans = malloc(sizeof(char*)*m);
    int n = i - 1;                                                   //кол-во строк

    if (f == 1){
        str[0] = change(str[0]);
    }
    
    int i1 = 0;
    for (int i = 0; i < n; i++){
        long long len = 0;
        len = strlen(str[i]);
        int start = 0;
        int g = 0;
        for (int j = 0; j<(len + 1); j++){
            if (((str[i][j]=='\t')||(str[i][j]==' '))&&((str[i][j-1]=='\t')||(str[i][j-1]==' '))){         // не обращаем внимания на множество пробелов
                start++;
                continue;
            }
            if (str[i][j] == '"'){            // считываем пока не найдем " на строчке
                /*if (start == j){
                    start++;
                }else{
                    str_ans[i1] = creat_str(str[i], start, j);       // строка
                    i1++;
                    start = j;
                }*/
                int q;
                for(int h = j; h < len + 1; h++){    
                    q = count(str, i, h);
                    if ((str[i][q + 1] == ' ')||(str[i][q + 1] == '\t')||(str[i][q + 1] == '\0')){
                        str_ans[i1] = creat_str1(str[i], start, q + 1);
                        i1++;
                        start = q + 1;
                        break;
                    }else{
                        h = q + 1;
                    }
                }
                start = q + 1;
                j = q;
            }
            //printf("TYT");
            if (check(str[i][j]) == 1){
                
                if ((str[i][j] == '\0')&&(start != j)){           // конец строки
                    str_ans[i1] = creat_str(str[i], start, j);
                    i1++;
                    break;
                }
                
                if ((str[i][j] == '\0')&&(start == j)){      // ситуация "|\0"
                    break;
                }
                
                if(start == j){
                    start++;
                    continue;
                }
                
                if((start == j)&&(g == 0)){                  // первое вхождение
                    if ((check(str[i][j+1]) == 1) &&(str[i][j+1]==str[i][j])&& ((str[i][j] != '\t')||(str[i][j] != ' '))){     //&&, ||, <<, ...
                        str_ans[i1] = creat_str(str[i], j, j + 2);
                        i1++;
                        start = start + 2;
                        j++;
                        continue;
                    }
                    if ((check(str[i][j+1]) == 0)&&((str[i][j] != '\t')||(str[i][j] != ' '))){    // &, <, ; ...
                        str_ans[i1] = creat_str(str[i], j, j + 1);
                        i1++;
                        start++;
                        continue;
                    }
                }else{
                    g = 1;
                    str_ans[i1] = creat_str(str[i], start, j);       // строка
                    i1++;
                    if ((str[i][j] == '\t')||(str[i][j] == ' ')){
                        start = j + 1;
                        continue;
                    }

                    if ((check(str[i][j+1]) == 1)&&(str[i][j+1] == str[i][j])&&((str[i][j] != '\t')||(str[i][j] != ' '))){     //&&, ||, <<, ...
                        str_ans[i1] = creat_str(str[i], j, j + 2);
                        i1++;
                        start = j + 2;
                        j++;
                        continue;
                    }
                    if ((check(str[i][j+1]) == 0)&&((str[i][j] != '\t')||(str[i][j] != ' '))){     // &, <, ; ...
                        str_ans[i1] = creat_str(str[i], j, j + 1);
                        i1++;
                        start = j + 1;
                        continue;
                    }
                }
            }else{
            }
        }
    }
    printf_str(i1, str_ans);
    return 0;
}