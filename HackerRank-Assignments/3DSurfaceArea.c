#include <stdio.h>

int surfaceArea(int H, int W, int **A) {
    int area = 0;
    for (int i = 0; i < H; i++) {
        for (int j = 0; j < W; j++) {
            area += 2;
            if (i == 0)
                area += A[i][j];
            else if (A[i][j] > A[i-1][j])
                area += A[i][j] - A[i-1][j];
            if (i == H-1)
                area += A[i][j];
            else if (A[i][j] > A[i+1][j])
                area += A[i][j] - A[i+1][j];
            if (j == 0)
                area += A[i][j];
            else if (A[i][j] > A[i][j-1])
                area += A[i][j] - A[i][j-1];
            if (j == W-1)
                area += A[i][j];
            else if (A[i][j] > A[i][j+1])
                area += A[i][j] - A[i][j+1];
        }
    }
    return area;
}

int main() {
    int H, W;
    scanf("%d %d", &H, &W);
    int **A = malloc(H * sizeof(int*));
    for (int i = 0; i < H; i++) {
        A[i] = malloc(W * sizeof(int));
        for (int j = 0; j < W; j++) {
            scanf("%d", &A[i][j]);
        }
    }
    printf("%d\n", surfaceArea(H, W, A));
    return 0;
}