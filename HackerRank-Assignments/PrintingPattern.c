#include <stdio.h>

int main() 
{

    int n;
    scanf("%d", &n);
  	for (int i=1; i<(n*2); i++){
        for (int j=1; j<(n*2); j++){
            int row1 = i;
            int col1 = j;
            int row2 = (2*n) - i;
            int col2 = (2*n) - j;
            int minimum = row1;
            if (col1 < minimum)
                minimum = col1;
            if (row2 < minimum)
                minimum = row2;
            if (col2 < minimum)
                minimum = col2;
            printf("%d ", n - minimum + 1);
        }
        printf("\n");
    }
    return 0;
}
