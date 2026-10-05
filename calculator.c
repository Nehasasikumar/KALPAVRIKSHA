#include <stdio.h>
#include <ctype.h>

int invalidexpr(char expr[]){
    for(int i=0; expr[i]!='\0'; i++){
        if (isdigit(expr[i])){
            continue;
        }
        if (expr[i] == '+' || 
           expr[i] == '-' ||
           expr[i] == '*' ||
           expr[i] == '/'){
               continue;
           }
        return 1;
    }
    return 0;
}

int invalidstructure(char expr[]){
    int expectedNum=1;
    int i = 0;
    while(expr[i] != '\0'){
        if(expectedNum){
            if(!isdigit(expr[i])){
                return 1;
            }
            i++;
            while (isdigit(expr[i])){
                i++;
                
            }
            expectedNum = 0;
        }
        else{
            if(expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == '/'){
                i++;
                expectedNum = 1;
            }
            else{
                return 1;
            }
            
        }
    }
    if(expectedNum){
        return 1;
    }     
    return 0;
}

int precedence(char op){
    if (op == '+' || op == '-'){
        return 1;
    }
    if (op == '*' || op == '/'){
        return 2;
    }
    return 0;
}

int calculate(int a, int b, char op){
    if (op == '+'){
        return a+b;
    }
    if (op == '-'){
        return a-b;
    }
    if (op == '*'){
        return a*b;
    }
    if (op == '/'){
        if (b == 0){
            printf("Error: Division by zero");
            return -99999;
        }   
        return a/b;
    }
    return -99999;
}

int evaluate(char expr[]){
    int values[100];
    char ops[100];
    int valTop = -1;
    int opTop = -1;
    
    for(int i=0; expr[i]!='\0'; i++){
        if (isdigit(expr[i])){
            int val = 0;
            while (isdigit(expr[i])){
                val = (val*10) + (expr[i]-'0');
                i++;
            }
            values[++valTop] = val;
            i--;
        }
        else{
            while(opTop != -1 && precedence(ops[opTop]) >= precedence(expr[i])){
                int b = values[valTop--];
                int a = values[valTop--];
                char op = ops[opTop--];
                values[++valTop] = calculate(a, b, op);
            }
            ops[++opTop] = expr[i];
        }
    }
    
    while(opTop != -1){
        int b = values[valTop--];
        int a = values[valTop--];
        char op = ops[opTop--];
        values[++valTop] = calculate(a, b, op);
    }
    
    return values[valTop];
}

int main(){
    char expr[100];
    printf("Enter the expression: ");
    scanf("%99[^\n]",expr);
    
    int j=0;
    
    for(int i=0; expr[i]!='\0'; i++){
        if (expr[i] != ' '){
            expr[j] = expr[i];
            j++;
        }
        
    }
    expr[j] = '\0';
    if (invalidexpr(expr) || invalidstructure(expr)){
        printf("Error: Invalid expression");
        return 1;
    }
    if (evaluate(expr) == -99999){
        return 1;
    }
    else{
        printf("Result: %d", evaluate(expr));
    }
}