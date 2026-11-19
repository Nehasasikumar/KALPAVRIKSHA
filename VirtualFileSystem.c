
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BLOCK_SIZE 512
#define NUM_BLOCKS 1024
#define MAX_NAME 51
#define MAX_COMMAND_LENGTH 8192
#define MAX_PATH_LENGTH 4096

unsigned char virtualDisk[NUM_BLOCKS][BLOCK_SIZE];

struct FreeBlock {
    int index;
    struct FreeBlock *next;
    struct FreeBlock *prev;
};

struct FileNode {
    char name[MAX_NAME];
    int isDirectory;
    int size;
    int blockCount;
    int *blockPointers;
    struct FileNode *parent;
    struct FileNode *next;
    struct FileNode *prev;
    struct FileNode *childHead;
};

struct FileNode *root = NULL;
struct FileNode *cwd = NULL;
struct FreeBlock *freeHead = NULL;
struct FreeBlock *freeTail = NULL;
int freeBlockCount = 0;

struct FileNode *createNode(char name[], int isDirectory) {
    struct FileNode *newNode = malloc(sizeof(struct FileNode));
    if (newNode == NULL) {
        return NULL;
    }
    strcpy(newNode->name, name);
    newNode->isDirectory = isDirectory;
    newNode->size = 0;
    newNode->blockCount = 0;
    newNode->blockPointers = NULL;
    newNode->parent = NULL;
    newNode->next = NULL;
    newNode->prev = NULL;
    newNode->childHead = NULL;
    return newNode;
}

int addFreeBlock(int index) {
    struct FreeBlock *newBlock = malloc(sizeof(struct FreeBlock));
    if (newBlock == NULL) {
        return 0;
    }
    newBlock->index = index;
    newBlock->next = NULL;
    newBlock->prev = freeTail;
    if (freeHead == NULL) {
        freeHead = newBlock;
        freeTail = newBlock;
    } else {
        freeTail->next = newBlock;
        freeTail = newBlock;
    }
    freeBlockCount++;
    return 1;
}

int allocateBlock() {
    struct FreeBlock *temp;
    int index;
    if (freeHead == NULL) {
        return -1;
    }
    temp = freeHead;
    index = temp->index;
    freeHead = freeHead->next;
    if (freeHead == NULL) {
        freeTail = NULL;
    } else {
        freeHead->prev = NULL;
    }
    free(temp);
    freeBlockCount--;
    return index;
}

void initializeFileSystem() {
    int i;
    memset(virtualDisk, 0, sizeof(virtualDisk));
    for (i = 0; i < NUM_BLOCKS; i++) {
        if (!addFreeBlock(i)) {
            printf("Failed to initialize free blocks.\n");
            exit(1);
        }
    }
    root = createNode("/", 1);
    if (root == NULL) {
        printf("Failed to create root directory.\n");
        exit(1);
    }
    cwd = root;
}

int validName(char name[]) {
    int i;
    int length = strlen(name);
    if (length == 0 || length >= MAX_NAME) {
        return 0;
    }
    if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
        return 0;
    }
    for (i = 0; i < length; i++) {
        if (name[i] == '/' || name[i] == '\\' || name[i] == ',' || isspace((unsigned char)name[i])) {
            return 0;
        }
    }
    return 1;
}

int validDirectoryName(char name[]) {
    char *dot;
    if (!validName(name)) {
        return 0;
    }
    dot = strrchr(name, '.');
    if (dot != NULL && dot != name) {
        return 0;
    }
    return 1;
}

struct FileNode *findChild(struct FileNode *directory, char name[]) {
    struct FileNode *current;
    if (directory == NULL || directory->childHead == NULL) {
        return NULL;
    }
    current = directory->childHead;
    do {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    } while (current != directory->childHead);
    return NULL;
}

void insertChild(struct FileNode *directory, struct FileNode *newNode) {
    if (directory->childHead == NULL) {
        directory->childHead = newNode;
        newNode->next = newNode;
        newNode->prev = newNode;
    } else {
        struct FileNode *last = directory->childHead->prev;
        last->next = newNode;
        newNode->prev = last;
        newNode->next = directory->childHead;
        directory->childHead->prev = newNode;
    }
    newNode->parent = directory;
}

void removeChild(struct FileNode *directory, struct FileNode *node) {
    if (directory == NULL || node == NULL || directory->childHead == NULL) {
        return;
    }
    if (node->next == node) {
        directory->childHead = NULL;
    } else {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        if (directory->childHead == node) {
            directory->childHead = node->next;
        }
    }
    node->next = NULL;
    node->prev = NULL;
    node->parent = NULL;
}

void makeDirectory(char name[]) {
    struct FileNode *newDirectory;
    if (!validDirectoryName(name)) {
        printf("Invalid directory name. Directory names cannot have file extensions.\n");
        return;
    }
    if (findChild(cwd, name) != NULL) {
        printf("A file or directory with this name already exists.\n");
        return;
    }
    newDirectory = createNode(name, 1);
    if (newDirectory == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    insertChild(cwd, newDirectory);
    printf("Directory created successfully.\n");
}

void createFile(char name[]) {
    struct FileNode *newFile;
    if (!validName(name)) {
        printf("Invalid file name.\n");
        return;
    }
    if (findChild(cwd, name) != NULL) {
        printf("A file or directory with this name already exists.\n");
        return;
    }
    newFile = createNode(name, 0);
    if (newFile == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    insertChild(cwd, newFile);
    printf("File created successfully.\n");
}

int releaseFileBlocks(struct FileNode *file) {
    int i;
    for (i = 0; i < file->blockCount; i++) {
        if (!addFreeBlock(file->blockPointers[i])) {
            printf("Could not return a block to the free list.\n");
            return 0;
        }
    }
    free(file->blockPointers);
    file->blockPointers = NULL;
    file->blockCount = 0;
    file->size = 0;
    return 1;
}

void writeFile(char name[], char content[]) {
    struct FileNode *file;
    int length = strlen(content);
    int requiredBlocks;
    int *newPointers;
    int i;
    int copied = 0;
    file = findChild(cwd, name);
    if (file == NULL) {
        printf("File not found.\n");
        return;
    }
    if (file->isDirectory) {
        printf("Cannot write to a directory.\n");
        return;
    }
    requiredBlocks = (length + BLOCK_SIZE - 1) / BLOCK_SIZE;
    if (requiredBlocks > freeBlockCount + file->blockCount) {
        printf("Not enough disk space.\n");
        return;
    }
    if (requiredBlocks == 0) {
        if (releaseFileBlocks(file)) {
            printf("File written successfully.\n");
        }
        return;
    }
    newPointers = malloc(requiredBlocks * sizeof(int));
    if (newPointers == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }
    if (!releaseFileBlocks(file)) {
        free(newPointers);
        return;
    }
    for (i = 0; i < requiredBlocks; i++) {
        newPointers[i] = allocateBlock();
        if (newPointers[i] == -1) {
            int j;
            for (j = 0; j < i; j++) {
                addFreeBlock(newPointers[j]);
            }
            free(newPointers);
            printf("Block allocation failed.\n");
            return;
        }
    }
    for (i = 0; i < requiredBlocks; i++) {
        int remaining = length - copied;
        int bytesToCopy = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
        memcpy(virtualDisk[newPointers[i]], content + copied, bytesToCopy);
        copied += bytesToCopy;
    }
    file->blockPointers = newPointers;
    file->blockCount = requiredBlocks;
    file->size = length;
    printf("File written successfully.\n");
}

void readFile(char name[]) {
    struct FileNode *file;
    int remaining;
    int i;
    file = findChild(cwd, name);
    if (file == NULL) {
        printf("File not found.\n");
        return;
    }
    if (file->isDirectory) {
        printf("Cannot read a directory.\n");
        return;
    }
    remaining = file->size;
    for (i = 0; i < file->blockCount; i++) {
        int bytesToRead = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
        fwrite(virtualDisk[file->blockPointers[i]], 1, bytesToRead, stdout);
        remaining -= bytesToRead;
    }
    printf("\n");
}

void deleteFile(char name[]) {
    struct FileNode *file;
    file = findChild(cwd, name);
    if (file == NULL) {
        printf("File not found.\n");
        return;
    }
    if (file->isDirectory) {
        printf("Cannot delete a directory using delete. Use rmdir.\n");
        return;
    }
    if (!releaseFileBlocks(file)) {
        return;
    }
    removeChild(cwd, file);
    free(file);
    printf("File deleted successfully.\n");
}

void removeDirectory(char name[]) {
    struct FileNode *directory;
    directory = findChild(cwd, name);
    if (directory == NULL) {
        printf("Directory not found.\n");
        return;
    }
    if (!directory->isDirectory) {
        printf("The specified name is a file, not a directory.\n");
        return;
    }
    if (directory->childHead != NULL) {
        printf("Directory is not empty.\n");
        return;
    }
    removeChild(cwd, directory);
    free(directory);
    printf("Directory removed successfully.\n");
}

struct FileNode *resolvePath(char path[]) {
    char pathCopy[MAX_PATH_LENGTH];
    char *token;
    struct FileNode *current;
    if (strlen(path) == 0 || strlen(path) >= MAX_PATH_LENGTH) {
        return NULL;
    }
    strcpy(pathCopy, path);
    current = pathCopy[0] == '/' ? root : cwd;
    token = strtok(pathCopy, "/");
    while (token != NULL) {
        if (strcmp(token, ".") == 0) {
            token = strtok(NULL, "/");
            continue;
        }
        if (strcmp(token, "..") == 0) {
            if (current->parent != NULL) {
                current = current->parent;
            }
            token = strtok(NULL, "/");
            continue;
        }
        if (!current->isDirectory) {
            return NULL;
        }
        current = findChild(current, token);
        if (current == NULL) {
            return NULL;
        }
        token = strtok(NULL, "/");
        if (token != NULL && !current->isDirectory) {
            return NULL;
        }
    }
    return current;
}

int buildPath(struct FileNode *directory, char path[], size_t size) {
    char parentPath[MAX_PATH_LENGTH];
    int result;
    if (directory == NULL || size == 0) {
        return 0;
    }
    if (directory == root) {
        return snprintf(path, size, "/") < (int)size;
    }
    if (!buildPath(directory->parent, parentPath, sizeof(parentPath))) {
        return 0;
    }
    if (strcmp(parentPath, "/") == 0) {
        result = snprintf(path, size, "/%s", directory->name);
    } else {
        result = snprintf(path, size, "%s/%s", parentPath, directory->name);
    }
    return result >= 0 && (size_t)result < size;
}

void printWorkingDirectory() {
    char path[MAX_PATH_LENGTH];
    if (buildPath(cwd, path, sizeof(path))) {
        printf("%s\n", path);
    } else {
        printf("Path is too long.\n");
    }
}

void listDirectory(char path[]) {
    struct FileNode *directory;
    struct FileNode *current;
    directory = strlen(path) == 0 ? cwd : resolvePath(path);
    if (directory == NULL) {
        printf("Path not found.\n");
        return;
    }
    if (!directory->isDirectory) {
        printf("The specified path is a file.\n");
        return;
    }
    if (directory->childHead == NULL) {
        printf("Directory is empty.\n");
        return;
    }
    current = directory->childHead;
    do {
        if (current->isDirectory) {
            printf("[DIR]  %s\n", current->name);
        } else {
            printf("[FILE] %s (%d bytes)\n", current->name, current->size);
        }
        current = current->next;
    } while (current != directory->childHead);
}

void changeDirectory(char path[]) {
    struct FileNode *directory;
    if (strlen(path) == 0) {
        printf("Please provide a directory path.\n");
        return;
    }
    directory = resolvePath(path);
    if (directory == NULL) {
        printf("Directory not found.\n");
        return;
    }
    if (!directory->isDirectory) {
        printf("Cannot change directory to a file.\n");
        return;
    }
    cwd = directory;
    printf("Directory changed successfully.\n");
}

void displayDiskUsage() {
    int usedBlocks = NUM_BLOCKS - freeBlockCount;
    printf("Total blocks: %d\n", NUM_BLOCKS);
    printf("Used blocks: %d\n", usedBlocks);
    printf("Free blocks: %d\n", freeBlockCount);
    printf("Block size: %d bytes\n", BLOCK_SIZE);
    printf("Total disk size: %d bytes\n", NUM_BLOCKS * BLOCK_SIZE);
    printf("Used disk space: %d bytes\n", usedBlocks * BLOCK_SIZE);
    printf("Free disk space: %d bytes\n", freeBlockCount * BLOCK_SIZE);
}

void freeDirectoryContents(struct FileNode *directory) {
    while (directory->childHead != NULL) {
        struct FileNode *child = directory->childHead;
        if (child->isDirectory) {
            freeDirectoryContents(child);
        } else {
            free(child->blockPointers);
            child->blockPointers = NULL;
        }
        removeChild(directory, child);
        free(child);
    }
}

void freeFileSystem() {
    struct FreeBlock *current;
    if (root != NULL) {
        freeDirectoryContents(root);
        free(root);
        root = NULL;
        cwd = NULL;
    }
    current = freeHead;
    while (current != NULL) {
        struct FreeBlock *next = current->next;
        free(current);
        current = next;
    }
    freeHead = NULL;
    freeTail = NULL;
    freeBlockCount = 0;
}

char *trimWhitespace(char *text) {
    char *end;
    while (isspace((unsigned char)*text)) {
        text++;
    }
    if (*text == '\0') {
        return text;
    }
    end = text + strlen(text) - 1;
    while (end > text && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';
    return text;
}

void processWriteCommand(char arguments[]) {
    char *name;
    char *content;
    char *quoteEnd;
    char *separator;
    arguments = trimWhitespace(arguments);
    if (*arguments == '\0') {
        printf("Usage: write filename \"content\"\n");
        return;
    }
    name = arguments;
    separator = name;
    while (*separator != '\0' && !isspace((unsigned char)*separator)) {
        separator++;
    }
    if (*separator == '\0') {
        printf("Please provide file content.\n");
        return;
    }
    *separator = '\0';
    content = trimWhitespace(separator + 1);
    if (*content == '"') {
        content++;
        quoteEnd = strrchr(content, '"');
        if (quoteEnd == NULL) {
            printf("Missing closing quotation mark.\n");
            return;
        }
        *quoteEnd = '\0';
        if (*trimWhitespace(quoteEnd + 1) != '\0') {
            printf("Unexpected text after the closing quotation mark.\n");
            return;
        }
    }
    if (!validName(name)) {
        printf("Invalid file name.\n");
        return;
    }
    writeFile(name, content);
}

int main() {
    char input[MAX_COMMAND_LENGTH];
    char command[32];
    char *arguments;
    char *separator;
    size_t commandLength;
    initializeFileSystem();
    printf("Virtual File System Started.\n");
    printf("Type help to see available commands.\n");
    while (1) {
        printf("VFS> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        if (strchr(input, '\n') == NULL && !feof(stdin)) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }
            printf("Command is too long.\n");
            continue;
        }
        arguments = trimWhitespace(input);
        if (*arguments == '\0') {
            continue;
        }
        separator = arguments;
        while (*separator != '\0' && !isspace((unsigned char)*separator)) {
            separator++;
        }
        commandLength = separator - arguments;
        if (commandLength == 0 || commandLength >= sizeof(command)) {
            printf("Invalid command name.\n");
            continue;
        }
        memcpy(command, arguments, commandLength);
        command[commandLength] = '\0';
        if (*separator == '\0') {
            arguments = separator;
        } else {
            *separator = '\0';
            arguments = trimWhitespace(separator + 1);
        }
        if (strcmp(command, "mkdir") == 0) {
            if (*arguments == '\0') {
                printf("Usage: mkdir directory_name\n");
            } else {
                makeDirectory(arguments);
            }
        } else if (strcmp(command, "create") == 0) {
            if (*arguments == '\0') {
                printf("Usage: create filename\n");
            } else {
                createFile(arguments);
            }
        } else if (strcmp(command, "write") == 0) {
            processWriteCommand(arguments);
        } else if (strcmp(command, "read") == 0) {
            if (*arguments == '\0') {
                printf("Usage: read filename\n");
            } else {
                readFile(arguments);
            }
        } else if (strcmp(command, "delete") == 0) {
            if (*arguments == '\0') {
                printf("Usage: delete filename\n");
            } else {
                deleteFile(arguments);
            }
        } else if (strcmp(command, "rmdir") == 0) {
            if (*arguments == '\0') {
                printf("Usage: rmdir directory_name\n");
            } else {
                removeDirectory(arguments);
            }
        } else if (strcmp(command, "ls") == 0) {
            listDirectory(arguments);
        } else if (strcmp(command, "cd") == 0) {
            changeDirectory(arguments);
        } else if (strcmp(command, "pwd") == 0) {
            if (*arguments != '\0') {
                printf("pwd does not take arguments.\n");
            } else {
                printWorkingDirectory();
            }
        } else if (strcmp(command, "df") == 0) {
            if (*arguments != '\0') {
                printf("df does not take arguments.\n");
            } else {
                displayDiskUsage();
            }
        } else if (strcmp(command, "help") == 0) {
            printf("mkdir directory_name\n");
            printf("create filename\n");
            printf("write filename \"content\"\n");
            printf("read filename\n");
            printf("delete filename\n");
            printf("rmdir directory_name\n");
            printf("ls [path]\n");
            printf("cd path\n");
            printf("pwd\n");
            printf("df\n");
            printf("exit\n");
        } else if (strcmp(command, "exit") == 0) {
            if (*arguments != '\0') {
                printf("exit does not take arguments.\n");
            } else {
                break;
            }
        } else {
            printf("Unknown command. Type help to see available commands.\n");
        }
    }
    freeFileSystem();
    printf("Virtual File System closed.\n");
    return 0;
}
