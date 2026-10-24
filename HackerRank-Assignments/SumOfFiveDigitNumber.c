#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

int main() {
	
    int n;
    scanf("%d", &n);
    int answer = 0;
    while (n>0){
      answer += n%10;
      n/=10;  
    }
    printf("%d",answer);
    return 0;
}
