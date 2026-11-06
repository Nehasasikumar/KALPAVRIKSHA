#include <stdio.h>
#include <stdlib.h>

int main(){
    int n, m;
    scanf("%d %d", &n, &m);
    long long *arr = calloc(n + 2, sizeof(long long));
    for (int i = 0; i < m; i++){
        int a, b, k;
        scanf("%d %d %d", &a, &b, &k);
        arr[a] += k;
        arr[b + 1] -= k;
    }
    long long sum = 0;
    long long maximum = 0;
    for (int i = 1; i <= n; i++){
        sum += arr[i];
        if (sum > maximum){
            maximum = sum;
        }
    }
    printf("%lld\n", maximum);
    free(arr);
    return 0;
}