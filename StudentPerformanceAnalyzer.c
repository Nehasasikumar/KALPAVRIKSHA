#include <stdio.h>
#define MAX_NAME_LENGTH 100
#define MAX_STUDENTS 100

struct Student{
    int roll_number;
    char name[MAX_NAME_LENGTH];
    int marks[3];
};

int calculate_total(int subject1,int subject2,int subject3){
    int total = subject1 + subject2 + subject3;
    return total;
}

float calculate_average(int total){
    float average = total/3.0;
    return average;
}

char assign_grade(float average){
    if (average >= 85)
        return 'A';
    else if (average >= 70)
        return 'B';
    else if (average >= 50)
        return 'C';
    else if (average >= 35)
        return 'D';
    else
        return 'F';
}

void print_pattern(char grade){
    if (grade == 'A')
        printf("*****\n");
    else if (grade == 'B')
        printf("****\n");
    else if (grade == 'C')
        printf("***\n");
    else if (grade == 'D')
        printf("**\n");
}

void print_roll_numbers(int current, int n){
    if (current > n)
        return;
    printf("%d ", current);
    print_roll_numbers(current + 1, n);
}

int main()
{
    struct Student students[MAX_STUDENTS];
    int n;

    do{
        printf("Enter number of students: ");
        scanf("%d", &n);
        if (n < 1 || n > MAX_STUDENTS){
            printf("Enter a valid number between 1 and 100\n");
        }

    } while (n < 1 || n > MAX_STUDENTS);

    getchar();

    for (int i = 0; i < n; i++)
    {
        char input[300];
        while (1)
        {
            fgets(input, sizeof(input), stdin);
            int count = sscanf(input, "%d %99[^0-9] %d %d %d",
                               &students[i].roll_number,
                               students[i].name,
                               &students[i].marks[0],
                               &students[i].marks[1],
                               &students[i].marks[2]);

            if (count != 5){
                printf("Invalid input. Enter again:\n");
                continue;
            }

            if (students[i].marks[0] < 0 || students[i].marks[0] > 100 ||
                students[i].marks[1] < 0 || students[i].marks[1] > 100 ||
                students[i].marks[2] < 0 || students[i].marks[2] > 100){
                printf("Enter valid marks between 0 and 100:\n");
                continue;
            }
            break;
        }
    }

    for (int i = 0; i < n; i++)
    {
        int total = calculate_total(students[i].marks[0], students[i].marks[1], students[i].marks[2]);
        float average = calculate_average(total);
        char grade = assign_grade(average);
        printf("\n");
        printf("Roll: %d\n", students[i].roll_number);
        printf("Name: %s\n", students[i].name);
        printf("Total: %d\n", total);
        printf("Average: %.2f\n", average);
        printf("Grade: %c\n", grade);
        if (average < 35){
            continue;
        }
        printf("Performance: ");
        print_pattern(grade);
        printf("\n");
    }
    
    printf("\n");
    printf("List of Roll Numbers (via recursion): ");
    print_roll_numbers(1, n);
    

    return 0;
}