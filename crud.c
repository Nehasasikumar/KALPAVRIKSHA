#include <stdio.h>
#include <string.h>
#define MAX_NAME_LENGTH 50
#define MAX_AGE 120

struct User{
    int id;
    char name[MAX_NAME_LENGTH];
    int age;
};

void clear_input_buffer(){
    int ch;
    while((ch = getchar()) != '\n' && ch != EOF);
}

int read_int(){
    int value;
    while(scanf("%d", &value) != 1){
        printf("Invalid input.\nPlease enter a number: ");
        clear_input_buffer();
    }
    clear_input_buffer();
    return value;
}

void create_file_if_not_exists(){
    FILE *file;
    file = fopen("users.txt", "a");
    if(file == NULL){
        printf("Unable to create users file.\n");
        return;
    }
    fclose(file);
}

int id_exists(int id){
    FILE *file;
    struct User user;
    file = fopen("users.txt", "r");
    if(file == NULL){
        return 0;
    }
    while(fscanf(file, "%d,%49[^,],%d", &user.id, user.name, &user.age) == 3){
        if(user.id == id){
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

int read_name(char name[]){
    printf("Enter Name: ");
    scanf(" %49[^\n]", name);
    clear_input_buffer();
    if(strpbrk(name, ",$!@#%^&*():;><?/}{[]|+=-_") != NULL){
    printf("Invalid name. Special characters are not allowed.\n");
    return 0;
    }
    return 1;
}

void create_user(){
    FILE *file;
    struct User user;
    create_file_if_not_exists();
    printf("Enter ID: ");
    user.id = read_int();
    while(user.id <= 0){
        printf("ID must be greater than 0. Enter ID: ");
        user.id = read_int();
    }
    if(id_exists(user.id)){
        printf("ID already exists. Please use a unique ID.\n");
        return;
    }
    while(!read_name(user.name)){
    }
    printf("Enter Age: ");
    user.age = read_int();
    while(user.age <= 0 || user.age > MAX_AGE){
        printf("Invalid age.\nEnter age between 1 and %d: ",MAX_AGE);
        user.age = read_int();
    }
    file = fopen("users.txt", "a");
    if(file == NULL){
        printf("Unable to open file.\n");
        return;
    }
    fprintf(file, "%d,%s,%d\n", user.id, user.name, user.age);
    fclose(file);
    printf("User added successfully.\n");
}

void read_users(){
    FILE *file;
    struct User user;
    int count = 0;
    file = fopen("users.txt", "r");
    if(file == NULL){
        printf("No users found.\n");
        return;
    }
    printf("\nID\tName\t\tAge\n");
    while(fscanf(file, "%d,%49[^,],%d", &user.id, user.name, &user.age) == 3){
        printf("%d\t%-15s\t%d\n", user.id, user.name, user.age);
        count++;
    }
    fclose(file);
    if(count == 0){
        printf("No users found in the file.\n");
    }
}

void update_user(){
    FILE *file;
    FILE *temporary_file;
    struct User user;
    int id;
    int found = 0;
    printf("Enter ID to update: ");
    id = read_int();
    file = fopen("users.txt", "r");
    if(file == NULL){
        printf("No users found.\n");
        return;
    }
    temporary_file = fopen("temp.txt", "w");
    if(temporary_file == NULL){
        printf("Unable to create temporary file.\n");
        fclose(file);
        return;
    }

    while(fscanf(file, "%d,%49[^,],%d", &user.id, user.name, &user.age) == 3){
        if(user.id == id){
            while(!read_name(user.name)){
            }
            printf("Enter new age: ");
            user.age = read_int();
            while(user.age <= 0 || user.age > MAX_AGE){
                printf("Invalid age. Enter age between 1 and %d: ",MAX_AGE);
                user.age = read_int();
            }
            found = 1;
        }
        fprintf(temporary_file, "%d,%s,%d\n",
                user.id, user.name, user.age);
    }
    fclose(file);
    fclose(temporary_file);
    if(found){
        remove("users.txt");
        rename("temp.txt", "users.txt");
        printf("User updated successfully.\n");
    }
    else{
        remove("temp.txt");
        printf("User not found.\n");
    }
}

void delete_user(){
    FILE *file;
    FILE *temporary_file;
    struct User user;
    int id;
    int found = 0;
    printf("Enter ID to delete: ");
    id = read_int();
    file = fopen("users.txt", "r");
    if(file == NULL){
        printf("No users found.\n");
        return;
    }
    temporary_file = fopen("temp.txt", "w");
    if(temporary_file == NULL){
        printf("Unable to create temporary file.\n");
        fclose(file);
        return;
    }
    while(fscanf(file, "%d,%49[^,],%d", &user.id, user.name, &user.age) == 3){
        if(user.id == id){
            found = 1;
            continue;
        }
        fprintf(temporary_file, "%d,%s,%d\n",
                user.id, user.name, user.age);
    }
    fclose(file);
    fclose(temporary_file);
    if(found){
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
    while(1){
        printf("\n1. Create User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        choice = read_int();
        switch(choice){
            case 1:
                create_user();
                break;
            case 2:
                read_users();
                break;
            case 3:
                update_user();
                break;
            case 4:
                delete_user();
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