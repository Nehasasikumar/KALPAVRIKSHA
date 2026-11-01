#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* readline();
char* isValid(char* s) {
    static char yes[] = "YES";
    static char no[] = "NO";
    int frequency[26] = {0};
    for(int i = 0; s[i] != '\0'; i++){
        frequency[s[i]-'a']++;
    }
    int first = 0;
    int second = 0;
    for(int i = 0;i < 26; i++){
        if(frequency[i] == 0)
            continue;
        if(first == 0)
            first = frequency[i];
        else if(frequency[i] == first)
            continue;
        else if(second == 0)
            second = frequency[i];
        else if(frequency[i] == second)
            continue;
        else 
            return no;
    }
    if(second == 0)
        return yes;
    int countFirst = 0;
    int countSecond = 0;
    for(int i = 0;i <26; i++){
        if(frequency[i] == first)
            countFirst++;
        if(frequency[i] == second)
            countSecond++;
    }
    if(first > second){
        int temp = first;
        first = second;
        second = temp;
        temp = countFirst;
        countFirst = countSecond;
        countSecond = temp;
    }
    if(first == 1 && countFirst == 1)
        return yes;
    if(second == first+1 && countSecond ==1)
        return yes;
    return no;
}

int main()
{
    FILE* fptr = fopen(getenv("OUTPUT_PATH"), "w");

    char* s = readline();

    char* result = isValid(s);

    fprintf(fptr, "%s\n", result);

    fclose(fptr);

    return 0;
}

char* readline() {
    size_t alloc_length = 1024;
    size_t data_length = 0;

    char* data = malloc(alloc_length);

    while (true) {
        char* cursor = data + data_length;
        char* line = fgets(cursor, alloc_length - data_length, stdin);

        if (!line) {
            break;
        }

        data_length += strlen(cursor);

        if (data_length < alloc_length - 1 || data[data_length - 1] == '\n') {
            break;
        }

        alloc_length <<= 1;

        data = realloc(data, alloc_length);

        if (!data) {
            data = '\0';

            break;
        }
    }

    if (data[data_length - 1] == '\n') {
        data[data_length - 1] = '\0';

        data = realloc(data, data_length);

        if (!data) {
            data = '\0';
        }
    } else {
        data = realloc(data, data_length + 1);

        if (!data) {
            data = '\0';
        } else {
            data[data_length] = '\0';
        }
    }

    return data;
}
