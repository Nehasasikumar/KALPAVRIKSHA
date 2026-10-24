#include <stdio.h>

void calculate_the_maximum(int n, int k) {
  int max_value_of_and = 0;
  int max_value_of_or = 0;
  int max_value_of_xor = 0;
  for(int i=1; i<=n; i++){
    for (int j=i+1; j<=n; j++){
        if (((i&j) > max_value_of_and) && (i&j) < k){
            max_value_of_and = i&j;
        }
        if (((i|j) > max_value_of_or) && (i|j) < k){
            max_value_of_or = i|j;
        }
        if (((i^j) > max_value_of_xor) && (i^j) < k){
            max_value_of_xor = i^j;
        }
    }
  }
  printf("%d\n", max_value_of_and);
  printf("%d\n", max_value_of_or);
  printf("%d", max_value_of_xor);
}

int main() {
    int n, k;
  
    scanf("%d %d", &n, &k);
    calculate_the_maximum(n, k);
 
    return 0;
}
