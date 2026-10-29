#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

int inputHandling(void){
    char input[100];
    char *end;
    long value;
    printf("Enter matrix size (2-10): ");
    if (fgets(input, sizeof(input), stdin) == NULL)
        return -1;
    errno = 0;
    value = strtol(input, &end, 10);
    if (end == input || errno == ERANGE)
        return -1;
    while (isspace((unsigned char)*end))
        end++;
    if (*end != '\0' || value < 2 || value > 10)
        return -1;
    return (int)value;
}

void generateMatrix(int **matrix, int *data, int n){
    for (int i = 0; i < n * n; i++) {
        *(data + i) = rand() % 256;
        *(matrix + i) = data + i;
    }
}

void displayMatrix(int **matrix, int n){
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%3d ", **(matrix + i * n + j));
        }
        printf("\n");
    }
}

void rotateMatrix(int **matrix, int n){
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            int *temp = *(matrix + i * n + j);
            *(matrix + i * n + j) = *(matrix + j * n + i);
            *(matrix + j * n + i) = temp;
        }
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n / 2; j++) {
            int *temp = *(matrix + i * n + j);
            *(matrix + i * n + j) = *(matrix + i * n + (n - 1 - j));
            *(matrix + i * n + (n - 1 - j)) = temp;
        }
    }
}

int smoothMatrix(int **matrix, int n){
    int *prev = malloc(n * sizeof(int));
    if (prev == NULL)
        return 0;
    int *curr = malloc(n * sizeof(int));
    if (curr == NULL) {
        free(prev);
        return 0;
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            *(curr + j) = **(matrix + i * n + j);
        }
        for (int j = 0; j < n; j++) {
            int sum = 0;
            int count = 0;
            for (int r = i - 1; r <= i + 1; r++) {
                if (r < 0 || r >= n)
                    continue;
                for (int c = j - 1; c <= j + 1; c++) {
                    if (c < 0 || c >= n)
                        continue;
                    int value;
                    if (r == i - 1) {
                        value = *(prev + c);
                    }
                    else if (r == i) {
                        value = *(curr + c);
                    }
                    else {
                        value = **(matrix + r * n + c);
                    }
                    sum += value % 256;
                    count++;
                }
            }
            int average = sum / count;
            **(matrix + i * n + j) += average * 256;
        }
        int *temp = prev;
        prev = curr;
        curr = temp;
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            **(matrix + i * n + j) /= 256;
        }
    }
    free(prev);
    free(curr);
    return 1;
}

int main(void){
    int n = inputHandling();
    if (n == -1) {
        printf("Invalid input. Enter an integer between 2 and 10.\n");
        return 1;
    }
    srand((unsigned int)time(NULL));
    int *data = malloc(n * n * sizeof(int));
    if (data == NULL) {
        printf("Memory allocation failed for data.\n");
        return 1;
    }
    int **matrix = malloc(n * n * sizeof(int *));
    if (matrix == NULL) {
        printf("Memory allocation failed for matrix.\n");
        free(data);
        return 1;
    }
    generateMatrix(matrix, data, n);
    printf("\nOriginal Randomly Generated Matrix:\n");
    displayMatrix(matrix, n);
    rotateMatrix(matrix, n);
    printf("\nMatrix after 90-degree Clockwise Rotation:\n");
    displayMatrix(matrix, n);
    if (!smoothMatrix(matrix, n)) {
        printf("Memory allocation failed for smoothing.\n");
        free(matrix);
        free(data);
        return 1;
    }
    printf("\nMatrix after Applying 3x3 Smoothing Filter:\n");
    displayMatrix(matrix, n);
    free(matrix);
    free(data);
    return 0;
}