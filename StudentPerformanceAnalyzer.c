#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define MAX_NAME_LENGTH 100
#define MAX_STUDENTS 100
int total_students;
struct Student {
    int roll_number;
    char name[MAX_NAME_LENGTH];
    int marks[3];
};

int calculate_total(int subject1, int subject2, int subject3) {
    int total = subject1 + subject2 + subject3;
    return total;
}

float calculate_average(int total) {
    float average = total / 3.0;
    return average;
}

char assign_grade(float average) {
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

void print_pattern(char grade) {
    if (grade == 'A')
        printf("*****\n");
    else if (grade == 'B')
        printf("****\n");
    else if (grade == 'C')
        printf("***\n");
    else if (grade == 'D')
        printf("**\n");
}

void trim_name(char name[]) {
    int start = 0;
    int end = (int)strlen(name) - 1;
    while (name[start] != '\0' && isspace((unsigned char)name[start]))
        start++;
    while (end >= start && isspace((unsigned char)name[end]))
        end--;
    int length = end - start + 1;
    memmove(name, name + start, length);
    name[length] = '\0';
}

int roll_number_exists(struct Student students[], int count, int roll_number) {
    for (int i = 0; i < count; i++) {
        if (students[i].roll_number == roll_number)
            return 1;
    }
    return 0;
}

int read_student_count(void) {
    char input[100];
    int n;
    char extra;
    while (1) {
        printf("Enter number of students: ");
        if (fgets(input, sizeof(input), stdin) == NULL)
            return 0;
        if (sscanf(input, " %d %c", &n, &extra) != 1) {
            printf("Invalid input. Enter a number between 1 and 100.\n");
            continue;
        }
        if (n < 1 || n > MAX_STUDENTS) {
            printf("Enter a valid number between 1 and 100.\n");
            continue;
        }
        return n;
    }
}

void read_student_details(struct Student students[], int index) {
    char input[300];
    int count;
    while (1) {
        printf("Enter roll number, name and three marks: ");
        if (fgets(input, sizeof(input), stdin) == NULL)
            return;
        count = sscanf(input, "%d %99[^0-9] %d %d %d",
                       &students[index].roll_number,
                       students[index].name,
                       &students[index].marks[0],
                       &students[index].marks[1],
                       &students[index].marks[2]);
        if (count != 5) {
            printf("Invalid input. Enter again:\n");
            continue;
        }
        trim_name(students[index].name);
        if (students[index].name[0] == '\0') {
            printf("Name cannot be empty. Enter again:\n");
            continue;
        }
        if (roll_number_exists(students, index, students[index].roll_number)) {
            printf("Roll number already exists. Enter a unique roll number:\n");
            continue;
        }
        if (students[index].marks[0] < 0 ||
            students[index].marks[0] > 100 ||
            students[index].marks[1] < 0 ||
            students[index].marks[1] > 100 ||
            students[index].marks[2] < 0 ||
            students[index].marks[2] > 100) {
            printf("Enter valid marks between 0 and 100:\n");
            continue;
        }
        break;
    }
}

void display_student(struct Student student) {
    int total = calculate_total(student.marks[0], student.marks[1], student.marks[2]);
    float average = calculate_average(total);
    char grade = assign_grade(average);
    printf("\n");
    printf("Roll: %d\n", student.roll_number);
    printf("Name: %s\n", student.name);
    printf("Total: %d\n", total);
    printf("Average: %.2f\n", average);
    printf("Grade: %c\n", grade);
    if (grade == 'F')
        return;
    printf("Performance: ");
    print_pattern(grade);
    printf("\n");
}

void display_all_students(struct Student students[], int count) {
    for (int i = 0; i < count; i++) {
        display_student(students[i]);
    }
}

void display_roll_numbers(struct Student students[], int count, int index) {
    if (index >= count)
        return;
    printf("%d ", students[index].roll_number);
    display_roll_numbers(students, count, index + 1);
}

int main(void) {
    struct Student students[MAX_STUDENTS];
    total_students = read_student_count();
    if (total_students == 0)
        return 1;
    for (int i = 0; i < total_students; i++) {
        read_student_details(students, i);
    }
    display_all_students(students, total_students);
    printf("\n");
    printf("List of Roll Numbers (via recursion): ");
    display_roll_numbers(students, total_students, 0);
    printf("\n");
    return 0;
}