#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <errno.h>
#define NAME_LENGTH 100
#define DESIGNATION_LENGTH 100
#define INPUT_LENGTH 256

struct Employee;
struct ReportNode {
    struct Employee *employee;
    struct ReportNode *next;
};

struct Employee {
    int id;
    char name[NAME_LENGTH];
    char designation[DESIGNATION_LENGTH];
    struct Employee *manager;
    struct ReportNode *reports;
    struct Employee *next;
};

struct Employee *employeeHead = NULL;

int isBlank(const char *text) {
    while (*text != '\0') {
        if (!isspace((unsigned char)*text)) {
            return 0;
        }
        text++;
    }
    return 1;
}

void trimWhitespace(char text[]) {
    size_t start = 0;
    size_t length;
    while (text[start] != '\0' && isspace((unsigned char)text[start])) {
        start++;
    }
    if (start > 0) {
        memmove(text, text + start, strlen(text + start) + 1);
    }
    length = strlen(text);
    while (length > 0 && isspace((unsigned char)text[length - 1])) {
        text[length - 1] = '\0';
        length--;
    }
}

int isCEODesignation(const char *designation) {
    return strlen(designation) == 3 &&
           tolower((unsigned char)designation[0]) == 'c' &&
           tolower((unsigned char)designation[1]) == 'e' &&
           tolower((unsigned char)designation[2]) == 'o';
}

int readLine(const char *prompt, char buffer[], size_t size) {
    int ch;
    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL) {
        return 0;
    }
    if (strchr(buffer, '\n') == NULL) {
        ch = getchar();
        if (ch == '\n' || ch == EOF) {
            return 1;
        }
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
        printf("Input is too long. Please try again.\n");
        return -1;
    }
    buffer[strcspn(buffer, "\n")] = '\0';
    return 1;
}

int readInteger(const char *prompt, int *value) {
    char input[INPUT_LENGTH];
    char *start;
    char *end;
    long number;
    int result;
    result = readLine(prompt, input, sizeof(input));
    if (result != 1) {
        return result;
    }
    start = input;
    while (isspace((unsigned char)*start)) {
        start++;
    }
    if (*start == '\0') {
        printf("Input cannot be empty.\n");
        return -1;
    }
    errno = 0;
    number = strtol(start, &end, 10);
    if (start == end || errno == ERANGE || number < INT_MIN || number > INT_MAX) {
        printf("Please enter a valid integer.\n");
        return -1;
    }
    while (isspace((unsigned char)*end)) {
        end++;
    }
    if (*end != '\0') {
        printf("Please enter only an integer.\n");
        return -1;
    }
    *value = (int)number;
    return 1;
}

struct Employee *findEmployeeById(int id) {
    struct Employee *current = employeeHead;
    while (current != NULL) {
        if (current->id == id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

struct Employee *findCEO(void) {
    struct Employee *current = employeeHead;
    while (current != NULL) {
        if (isCEODesignation(current->designation)) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int countDirectReports(struct Employee *employee) {
    int count = 0;
    struct ReportNode *current = employee->reports;
    while (current != NULL) {
        count++;
        current = current->next;
    }
    return count;
}

int addReport(struct Employee *manager, struct Employee *employee) {
    struct ReportNode *newNode;
    struct ReportNode *current;
    newNode = malloc(sizeof(struct ReportNode));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        return 0;
    }
    newNode->employee = employee;
    newNode->next = NULL;
    if (manager->reports == NULL) {
        manager->reports = newNode;
    } else {
        current = manager->reports;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
    return 1;
}

int removeReportLink(struct Employee *manager, struct Employee *employee) {
    struct ReportNode *current = manager->reports;
    struct ReportNode *previous = NULL;
    while (current != NULL) {
        if (current->employee == employee) {
            if (previous == NULL) {
                manager->reports = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            return 1;
        }
        previous = current;
        current = current->next;
    }
    return 0;
}

void addEmployee(void) {
    int id;
    int managerId;
    int result;
    char name[NAME_LENGTH];
    char designation[DESIGNATION_LENGTH];
    struct Employee *manager = NULL;
    struct Employee *newEmployee;
    struct Employee *current;
    result = readInteger("Enter Employee ID: ", &id);
    if (result != 1) {
        return;
    }
    if (id <= 0) {
        printf("Employee ID must be positive.\n");
        return;
    }
    if (findEmployeeById(id) != NULL) {
        printf("Employee ID already exists.\n");
        return;
    }
    result = readLine("Enter Name: ", name, sizeof(name));
    if (result != 1) {
        return;
    }
    trimWhitespace(name);
    if (isBlank(name)) {
        printf("Name cannot be empty.\n");
        return;
    }
    result = readLine("Enter Designation: ",designation, sizeof(designation));
    if (result != 1) {
        return;
    }
    trimWhitespace(designation);
    if (isBlank(designation)) {
        printf("Designation cannot be empty.\n");
        return;
    }
    if (employeeHead == NULL && !isCEODesignation(designation)) {
        printf("The first employee must be the CEO.\n");
        printf("Designation must be CEO.\n");
        return;
    }
    if (isCEODesignation(designation)) {
        if (findCEO() != NULL) {
            printf("Only one CEO is allowed.\n");
            return;
        }
        result = readInteger("Enter Reporting Manager ID (0 for CEO): ", &managerId);
        if (result != 1) {
            return;
        }
        if (managerId != 0) {
            printf("The CEO cannot have a reporting manager.\n");
            return;
        }
    } else {
        result = readInteger(
            "Enter Reporting Manager ID: ", &managerId);
        if (result != 1) {
            return;
        }
        if (managerId <= 0) {
            printf("Every non-CEO employee must have a manager.\n");
            return;
        }
        manager = findEmployeeById(managerId);
        if (manager == NULL) {
            printf("Reporting manager does not exist.\n");
            return;
        }
    }
    newEmployee = malloc(sizeof(struct Employee));
    if (newEmployee == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    newEmployee->id = id;
    strcpy(newEmployee->name, name);
    strcpy(newEmployee->designation, designation);
    newEmployee->manager = manager;
    newEmployee->reports = NULL;
    newEmployee->next = NULL;
    if (manager != NULL && !addReport(manager, newEmployee)) {
        free(newEmployee);
        return;
    }
    if (employeeHead == NULL) {
        employeeHead = newEmployee;
    } else {
        current = employeeHead;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newEmployee;
    }
    printf("Employee added successfully.\n");
}

struct Employee *findSiblingWithFewestReports(struct Employee *employee) {
    struct ReportNode *current;
    struct Employee *selected = NULL;
    int minimumReports = INT_MAX;
    if (employee->manager == NULL) {
        return NULL;
    }
    current = employee->manager->reports;
    while (current != NULL) {
        struct Employee *sibling = current->employee;
        if (sibling != employee) {
            int reportCount = countDirectReports(sibling);
            if (reportCount < minimumReports) {
                minimumReports = reportCount;
                selected = sibling;
            }
        }
        current = current->next;
    }
    return selected;
}

void transferReports(struct Employee *employee, struct Employee *newManager) {
    struct ReportNode *current;
    struct ReportNode *last;
    if (employee->reports == NULL) {
        return;
    }
    current = employee->reports;
    while (current != NULL) {
        current->employee->manager = newManager;
        if (current->next == NULL) {
            break;
        }
        current = current->next;
    }
    last = current;
    if (newManager->reports == NULL) {
        newManager->reports = employee->reports;
    } else {
        current = newManager->reports;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = employee->reports;
    }
    last->next = NULL;
    employee->reports = NULL;
}

void removeEmployee(void) {
    int id;
    int result;
    struct Employee *employee;
    struct Employee *manager;
    struct Employee *newManager;
    struct Employee *current;
    struct Employee *previous = NULL;
    result = readInteger("Enter Employee ID to remove: ", &id);
    if (result != 1) {
        return;
    }
    employee = findEmployeeById(id);
    if (employee == NULL) {
        printf("Employee not found.\n");
        return;
    }
    manager = employee->manager;
    if (manager == NULL) {
        printf("The CEO cannot be removed.\n");
        return;
    }
    current = employeeHead;
    while (current != employee) {
        previous = current;
        current = current->next;
    }
    newManager = findSiblingWithFewestReports(employee);
    if (newManager == NULL) {
        newManager = manager;
    }
    if (!removeReportLink(manager, employee)) {
        printf("Hierarchy is inconsistent. Removal cancelled.\n");
        return;
    }
    transferReports(employee, newManager);
    if (previous == NULL) {
        employeeHead = employee->next;
    } else {
        previous->next = employee->next;
    }
    free(employee);
    printf("Employee removed successfully.\n");
}

int countEmployeesUnder(struct Employee *employee) {
    int count = 0;
    struct ReportNode *current = employee->reports;
    while (current != NULL) {
        count += 1 + countEmployeesUnder(current->employee);
        current = current->next;
    }
    return count;
}

void displayHierarchyRecursive(struct Employee *employee, int level) {
    struct ReportNode *current = employee->reports;
    int i;
    for (i = 0; i < level; i++) {
        printf("    ");
    }
    printf("%d - %s - %s\n",
           employee->id,
           employee->name,
           employee->designation);
    while (current != NULL) {
        displayHierarchyRecursive(current->employee, level + 1);
        current = current->next;
    }
}

void displayEmployeeHierarchy(void) {
    int id;
    int result;
    struct Employee *employee;
    result = readInteger("Enter Employee ID: ", &id);
    if (result != 1) {
        return;
    }
    employee = findEmployeeById(id);
    if (employee == NULL) {
        printf("Employee not found.\n");
        return;
    }
    printf("\nEMPLOYEE HIERARCHY\n");
    displayHierarchyRecursive(employee, 0);
    printf("\nTotal Employees Under %d: %d\n",
           employee->id,
           countEmployeesUnder(employee));
}

void displayCompleteHierarchy(void) {
    struct Employee *current;
    int totalEmployees = 0;
    if (employeeHead == NULL) {
        printf("No employees in the organization.\n");
        return;
    }
    printf("\nORGANIZATION HIERARCHY\n");
    current = employeeHead;
    while (current != NULL) {
        if (current->manager == NULL) {
            displayHierarchyRecursive(current, 0);
            totalEmployees += 1 + countEmployeesUnder(current);
        }
        current = current->next;
    }
    printf("\nTotal Employees: %d\n", totalEmployees);
}

void freeOrganization(void) {
    struct Employee *current = employeeHead;
    while (current != NULL) {
        struct Employee *nextEmployee = current->next;
        struct ReportNode *report = current->reports;
        while (report != NULL) {
            struct ReportNode *nextReport = report->next;
            free(report);
            report = nextReport;
        }
        free(current);
        current = nextEmployee;
    }
    employeeHead = NULL;
}

int main(void) {
    int choice = 0;
    int result;
    do {
        printf("\nEMPLOYEE HIERARCHY MANAGEMENT\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");
        result = readInteger("Enter your choice: ", &choice);
        if (result == 0) {
            break;
        }
        if (result == -1) {
            continue;
        }
        switch (choice) {
            case 1:
                addEmployee();
                break;
            case 2:
                removeEmployee();
                break;
            case 3:
                displayEmployeeHierarchy();
                break;
            case 4:
                displayCompleteHierarchy();
                break;
            case 5:
                printf("Exiting Employee Hierarchy Management System.\n");
                break;
            default:
                printf("Invalid choice. Please select 1 to 5.\n");
        }
    } while (choice != 5);
    freeOrganization();
    return 0;
}