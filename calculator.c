#include <stdio.h>
#include <ctype.h>
#define MAX_SIZE 100

int is_invalid_structure(char expression[]){
    int expect_number = 1;
    int i = 0;
    while(expression[i] != '\0'){
        if(expect_number){
            if(!isdigit(expression[i])){
                return 1;
            }
            i++;
            while(isdigit(expression[i])){
                i++;
            }
            expect_number = 0;
        }
        else{
            if(expression[i] == '+' ||
               expression[i] == '-' ||
               expression[i] == '*' ||
               expression[i] == '/'){
                i++;
                expect_number = 1;
            }
            else{
                return 1;
            }
        }
    }
    if(expect_number){
        return 1;
    }
    return 0;
}

int get_precedence(char operator){
    if(operator == '+' || operator == '-'){
        return 1;
    }
    if(operator == '*' || operator == '/'){
        return 2;
    }
    return 0;
}

int calculate_operation(int left_operand, int right_operand, char operator, int *error){
    if(operator == '+'){
        return left_operand + right_operand;
    }
    if(operator == '-'){
        return left_operand - right_operand;
    }
    if(operator == '*'){
        return left_operand * right_operand;
    }
    if(operator == '/'){
        if(right_operand == 0){
            *error = 1;
            return 0;
        }
        return left_operand / right_operand;
    }
    *error = 1;
    return 0;
}

int evaluate_expression(char expression[], int *error){
    int values[MAX_SIZE];
    char operators[MAX_SIZE];
    int value_top = -1;
    int operator_top = -1;
    for(int i = 0; expression[i] != '\0'; i++){
        if(isdigit(expression[i])){
            int value = 0;
            while(isdigit(expression[i])){
                value = (value * 10) + (expression[i] - '0');
                i++;
            }
            values[++value_top] = value;
            i--;
        }
        else{
            while(operator_top != -1 && get_precedence(operators[operator_top]) >= get_precedence(expression[i])){
                int right_operand = values[value_top--];
                int left_operand = values[value_top--];
                char operator = operators[operator_top--];
                values[++value_top] = calculate_operation(left_operand, right_operand,operator, error);
                if(*error){
                    return 0;
                }
            }
            operators[++operator_top] = expression[i];
        }
    }
    while(operator_top != -1){
        int right_operand = values[value_top--];
        int left_operand = values[value_top--];
        char operator = operators[operator_top--];
        values[++value_top] = calculate_operation(left_operand, right_operand, operator, error);
        if(*error){
            return 0;
        }
    }
    return values[value_top];
}

int main(){
    char expression[MAX_SIZE];
    printf("Enter the expression: ");
    scanf("%99[^\n]", expression);
    int j = 0;
    for(int i = 0; expression[i] != '\0'; i++){
        if(expression[i] != ' '){
            expression[j] = expression[i];
            j++;
        }
    }
    expression[j] = '\0';
    if(is_invalid_structure(expression)){
        printf("Error: Invalid expression\n");
        return 1;
    }
    int error = 0;
    int result = evaluate_expression(expression, &error);
    if(error){
        printf("Error: Division by zero\n");
        return 1;
    }
    printf("Result: %d\n", result);
    return 0;
}