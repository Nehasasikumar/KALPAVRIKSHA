#include <stdio.h>

struct User{
    int id;
    char name[50];
    int age;
};

void clearInputBuffer(){
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF);
}

int readInt(){
    int value;
    while (scanf("%d", &value) != 1){
        printf("Invalid input.\nPlease enter a number: ");
        clearInputBuffer();
    }
    clearInputBuffer();
    return value;
}

int idExists(int id){
    FILE *fp;
    struct User u;
    fp = fopen("users.txt","r");
    if (fp == NULL){
        return 0;
    }
    while (fscanf(fp,"%d,%49[^,],%d",&u.id,u.name,&u.age) == 3){
        if (u.id == id){
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

void createUser(){
    FILE *fp;
    struct User u;
    printf("Enter ID: ");
    u.id = readInt();
    while (u.id <= 0){
        printf("ID must be greater than 0. Enter ID: ");
        u.id = readInt();
    }
    if (idExists(u.id)){
        printf("ID already exists. Please use a unique ID.\n");
        return;
    }
    printf("Enter Name: ");
    scanf(" %49[^,\n]", u.name);
    clearInputBuffer();
    printf("Enter Age: ");
    u.age = readInt();
    while (u.age <= 0 || u.age > 120){
        printf("Invalid age.\nEnter age between 1 and 120: ");
        u.age = readInt();
    }
    fp = fopen("users.txt","a");
    if (fp == NULL){
        printf("Unable to open file.\n");
        return;
    }
    fprintf(fp,"%d,%s,%d\n",u.id, u.name, u.age);
    fclose(fp);
    printf("User added successfully.\n");
}

void readUsers(){
    FILE *fp;
    struct User u;
    int count = 0;
    fp = fopen("users.txt", "r");
    if (fp == NULL){
        printf("No users found.\n");
        return;
    }
    printf("\nID\tName\t\tAge\n");
    while (fscanf(fp,"%d,%49[^,],%d",&u.id, u.name, &u.age) == 3){
        printf("%d\t%-15s\t%d\n",u.id, u.name, u.age);
        count++;
    }
    fclose(fp);
    if (count == 0){
        printf("No users found in the file.\n");
    }
}

void updateUser(){
    FILE *fp, *temp;
    struct User u;
    int id, found = 0;
    fp = fopen("users.txt", "r");
    if (fp == NULL){
        printf("No users found.\n");
        return;
    }
    temp = fopen("temp.txt", "w");
    if (temp == NULL){
        printf("Unable to create temporary file.\n");
        fclose(fp);
        return;
    }
    printf("Enter ID to update: ");
    id = readInt();
    while (fscanf(fp,"%d,%49[^,],%d",&u.id, u.name, &u.age) == 3){
        if (u.id == id){
            printf("Enter new name: ");
            scanf(" %49[^,\n]", u.name);
            clearInputBuffer();
            printf("Enter new age: ");
            u.age = readInt();
            while (u.age <= 0 || u.age > 120){
                printf("Invalid age. Enter age between 1 and 120: ");
                u.age = readInt();
            }
            found = 1;
        }
        fprintf(temp, "%d,%s,%d\n",u.id, u.name, u.age);
    }
    fclose(fp);
    fclose(temp);
    if (found){
        remove("users.txt");
        rename("temp.txt", "users.txt");
        printf("User updated successfully.\n");
    }
    else{
        remove("temp.txt");
        printf("User not found.\n");
    }
}

void deleteUser(){
    FILE *fp, *temp;
    struct User u;
    int id, found = 0;
    fp = fopen("users.txt", "r");
    if (fp == NULL){
        printf("No users found.\n");
        return;
    }
    temp = fopen("temp.txt", "w");
    if (temp == NULL){
        printf("Unable to create temporary file.\n");
        fclose(fp);
        return;
    }
    printf("Enter ID to delete: ");
    id = readInt();
    while (fscanf(fp, "%d,%49[^,],%d",&u.id, u.name, &u.age) == 3){
        if (u.id == id){
            found = 1;
            continue;
        }
        fprintf(temp, "%d,%s,%d\n",u.id, u.name, u.age);
    }
    fclose(fp);
    fclose(temp);
    if (found){
        remove("users.txt");
        rename("temp.txt", "users.txt");
        printf("User deleted successfully.\n");
    }
    else{
        remove("temp.txt");
        printf("User not found.\n");
    }
}

int main(){
    int choice;
    while (1){
        printf("\n1. Create User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        choice = readInt();
        switch (choice){
            case 1:
                createUser();
                break;
            case 2:
                readUsers();
                break;
            case 3:
                updateUser();
                break;
            case 4:
                deleteUser();
                break;
            case 5:
                printf("Program ended.\n");
                return 0;
            default:
                printf("Invalid choice.\n");
        }
    }
    return 0;
}