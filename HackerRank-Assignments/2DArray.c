#include <stdio.h>

int main(){
    int rows, cols;
    scanf("%d %d", &rows, &cols);
    int arr[rows][cols];
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            scanf("%d", &arr[i][j]);
        }
    }
    int maximum = -63;
    for (int i = 0; i < rows - 2; i++){
        for (int j = 0; j < cols - 2; j++){
            int sum = arr[i][j] + arr[i][j + 1] + arr[i][j + 2] + arr[i + 1][j + 1] + arr[i + 2][j] + arr[i + 2][j + 1] + arr[i + 2][j + 2];
            if (sum > maximum){
                maximum = sum;
            }
        }
    }
    printf("%d\n", maximum);
    return 0;
}