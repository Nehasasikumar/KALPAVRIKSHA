#include <stdio.h>
#include <string.h>

int* matchingStrings(int stringList_count, char** stringList, int queries_count, char** queries, int* result_count) {
    *result_count = queries_count;
    int *result = malloc(queries_count * sizeof(int));
    for (int i = 0; i < queries_count; i++) {
        result[i] = 0;
        for (int j = 0; j < stringList_count; j++) {
            if (strcmp(queries[i], stringList[j]) == 0) {
                result[i]++;
            }
        }
    }
    return result;
}

int main() {
    int n, q;
    scanf("%d", &n);
    char **stringList = malloc(n * sizeof(char*));
    for (int i = 0; i < n; i++) {
        stringList[i] = malloc(21 * sizeof(char));
        scanf("%20s", stringList[i]);
    }
    scanf("%d", &q);
    char **queries = malloc(q * sizeof(char*));
    for (int i = 0; i < q; i++) {
        queries[i] = malloc(21 * sizeof(char));
        scanf("%20s", queries[i]);
    }
    int result_count;
    int *result = matchingStrings(n, stringList, q, queries, &result_count);
    for (int i = 0; i < result_count; i++) {
        printf("%d\n", result[i]);
    }
    return 0;
}