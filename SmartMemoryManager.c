#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#define MAX_POOL_SIZE 1000000
#define INITIAL_CAPACITY 4

enum AllocationStrategy {
    FIRST_FIT = 1,
    BEST_FIT,
    WORST_FIT
};

struct Block {
    size_t start;
    size_t size;
    uint64_t allocationID;
    int isAllocated;
};

unsigned char *memoryPool = NULL;
struct Block *blocks = NULL;
size_t totalSize = 0;
size_t blockCount = 0;
size_t capacity = 0;
uint64_t nextAllocationID = 1;
enum AllocationStrategy strategy;

int readNumber(const char *prompt, uint64_t minimum, uint64_t maximum, uint64_t *result) {
    char input[128];
    char *end;
    uintmax_t value;
    size_t i;
    while (1) {
        printf("%s", prompt);
        if (fgets(input, sizeof(input), stdin) == NULL) {
            return 0;
        }
        if (strchr(input, '\n') == NULL && !feof(stdin)) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }
            printf("Input is too long.\n");
            continue;
        }
        i = 0;
        while (isspace((unsigned char)input[i])) {
            i++;
        }
        if (!isdigit((unsigned char)input[i])) {
            printf("Invalid input. Enter a positive number.\n");
            continue;
        }
        errno = 0;
        value = strtoumax(&input[i], &end, 10);
        if (errno == ERANGE) {
            printf("Number is too large.\n");
            continue;
        }
        while (isspace((unsigned char)*end)) {
            end++;
        }
        if (*end != '\0') {
            printf("Invalid input. Enter numbers only.\n");
            continue;
        }
        if (value < minimum || value > maximum) {
            printf("Enter a value between %" PRIu64 " and %" PRIu64 ".\n", minimum, maximum);
            continue;
        }
        *result = (uint64_t)value;
        return 1;
    }
}

int ensureCapacity(size_t required) {
    struct Block *newBlocks;
    size_t newCapacity;
    if (required <= capacity) {
        return 1;
    }
    newCapacity = capacity;
    while (newCapacity < required) {
        if (newCapacity > SIZE_MAX / 2) {
            newCapacity = required;
            break;
        }
        newCapacity *= 2;
    }
    if (newCapacity > SIZE_MAX / sizeof(struct Block)) {
        return 0;
    }
    newBlocks = realloc(blocks, newCapacity * sizeof(struct Block));
    if (newBlocks == NULL) {
        return 0;
    }
    blocks = newBlocks;
    capacity = newCapacity;
    return 1;
}

void removeBlockAt(size_t index) {
    for (size_t i = index; i + 1 < blockCount; i++) {
        blocks[i] = blocks[i + 1];
    }
    blockCount--;
}

int initializeMemoryPool(size_t poolSize) {
    memoryPool = malloc(poolSize);
    if (memoryPool == NULL) {
        printf("Memory pool allocation failed.\n");
        return 0;
    }
    capacity = INITIAL_CAPACITY;
    blocks = malloc(capacity * sizeof(struct Block));
    if (blocks == NULL) {
        printf("Metadata allocation failed.\n");
        free(memoryPool);
        memoryPool = NULL;
        return 0;
    }
    totalSize = poolSize;
    blockCount = 1;
    nextAllocationID = 1;
    memset(memoryPool, 0, poolSize);
    blocks[0].start = 0;
    blocks[0].size = poolSize;
    blocks[0].allocationID = 0;
    blocks[0].isAllocated = 0;
    return 1;
}

int firstFit(size_t requestedSize, size_t *selectedIndex) {
    for (size_t i = 0; i < blockCount; i++) {
        if (blocks[i].isAllocated == 0 &&
            blocks[i].size >= requestedSize) {
            *selectedIndex = i;
            return 1;
        }
    }
    return 0;
}

int bestFit(size_t requestedSize, size_t *selectedIndex) {
    size_t smallestSize = SIZE_MAX;
    int found = 0;
    for (size_t i = 0; i < blockCount; i++) {
        if (blocks[i].isAllocated == 0 &&
            blocks[i].size >= requestedSize) {
            if (blocks[i].size < smallestSize) {
                smallestSize = blocks[i].size;
                *selectedIndex = i;
                found = 1;
            }
        }
    }
    return found;
}

int worstFit(size_t requestedSize, size_t *selectedIndex) {
    size_t largestSize = 0;
    int found = 0;
    for (size_t i = 0; i < blockCount; i++) {
        if (blocks[i].isAllocated == 0 &&
            blocks[i].size >= requestedSize) {
            if (blocks[i].size > largestSize) {
                largestSize = blocks[i].size;
                *selectedIndex = i;
                found = 1;
            }
        }
    }
    return found;
}

int findSuitableBlock(size_t requestedSize, size_t *selectedIndex) {
    switch (strategy) {
        case FIRST_FIT:
            return firstFit(requestedSize, selectedIndex);
        case BEST_FIT:
            return bestFit(requestedSize, selectedIndex);
        case WORST_FIT:
            return worstFit(requestedSize, selectedIndex);
        default:
            return 0;
    }
}

int findAllocationByID(uint64_t allocationID, size_t *selectedIndex) {
    for (size_t i = 0; i < blockCount; i++) {
        if (blocks[i].isAllocated == 1 &&
            blocks[i].allocationID == allocationID) {
            *selectedIndex = i;
            return 1;
        }
    }
    return 0;
}

int findAllocationByIDAndStart(uint64_t allocationID, size_t start, size_t *selectedIndex) {
    for (size_t i = 0; i < blockCount; i++) {
        if (blocks[i].isAllocated == 1 &&
            blocks[i].allocationID == allocationID &&
            blocks[i].start == start) {
            *selectedIndex = i;
            return 1;
        }
    }
    return 0;
}

void allocateAtIndex(size_t index, size_t requestedSize, uint64_t allocationID) {
    struct Block original = blocks[index];
    if (original.size > requestedSize) {
        for (size_t i = blockCount; i > index + 1; i--) {
            blocks[i] = blocks[i - 1];
        }
        blocks[index + 1].start = original.start + requestedSize;
        blocks[index + 1].size = original.size - requestedSize;
        blocks[index + 1].allocationID = 0;
        blocks[index + 1].isAllocated = 0;
        blockCount++;
    }
    blocks[index].start = original.start;
    blocks[index].size = requestedSize;
    blocks[index].allocationID = allocationID;
    blocks[index].isAllocated = 1;
}

void allocateMemory(size_t requestedSize) {
    size_t selectedIndex;
    uint64_t allocationID;
    if (requestedSize == 0 || requestedSize > totalSize) {
        printf("Invalid allocation size.\n");
        return;
    }
    if (nextAllocationID == 0) {
        printf("Allocation ID limit reached.\n");
        return;
    }
    if (!findSuitableBlock(requestedSize, &selectedIndex)) {
        printf("Allocation failed: no contiguous free block is large enough.\n");
        return;
    }
    if (blocks[selectedIndex].size > requestedSize) {
        if (!ensureCapacity(blockCount + 1)) {
            printf("Allocation failed: metadata expansion failed.\n");
            return;
        }
    }
    allocationID = nextAllocationID;
    allocateAtIndex(selectedIndex, requestedSize, allocationID);
    if (nextAllocationID == UINT64_MAX) {
        nextAllocationID = 0;
    }
    else {
        nextAllocationID++;
    }
    printf("\nAllocation successful!\n");
    printf("Allocation ID: %" PRIu64 "\n", allocationID);
    printf("Start: %zu\n", blocks[selectedIndex].start);
    printf("Size: %zu bytes\n", requestedSize);
}

void mergeFreeBlocks(void) {
    size_t i = 0;
    while (i + 1 < blockCount) {
        if (blocks[i].isAllocated == 0 &&
            blocks[i + 1].isAllocated == 0) {
            blocks[i].size += blocks[i + 1].size;
            removeBlockAt(i + 1);
        }
        else {
            i++;
        }
    }
}

void releaseMemory(uint64_t allocationID) {
    size_t selectedIndex;
    if (!findAllocationByID(allocationID, &selectedIndex)) {
        printf("Invalid allocation ID. Block not found.\n");
        return;
    }
    blocks[selectedIndex].isAllocated = 0;
    blocks[selectedIndex].allocationID = 0;
    mergeFreeBlocks();
    printf("Allocation %" PRIu64 " released successfully.\n", allocationID);
}

void resizeAllocation(uint64_t allocationID, size_t newSize) {
    size_t selectedIndex;
    size_t oldStart;
    size_t oldSize;
    size_t extraSize;
    size_t destinationIndex;
    size_t destinationStart;
    size_t oldIndex;
    size_t newIndex;
    if (newSize == 0 || newSize > totalSize) {
        printf("Invalid resize size.\n");
        return;
    }
    if (!findAllocationByID(allocationID, &selectedIndex)) {
        printf("Invalid allocation ID.\n");
        return;
    }
    oldStart = blocks[selectedIndex].start;
    oldSize = blocks[selectedIndex].size;
    if (newSize == oldSize) {
        printf("Allocation already has the requested size.\n");
        return;
    }
    if (newSize < oldSize) {
        if (!ensureCapacity(blockCount + 1)) {
            printf("Resize failed: metadata expansion failed.\n");
            return;
        }
        for (size_t i = blockCount; i > selectedIndex + 1; i--) {
            blocks[i] = blocks[i - 1];
        }
        blocks[selectedIndex].size = newSize;
        blocks[selectedIndex + 1].start = oldStart + newSize;
        blocks[selectedIndex + 1].size = oldSize - newSize;
        blocks[selectedIndex + 1].allocationID = 0;
        blocks[selectedIndex + 1].isAllocated = 0;
        blockCount++;
        mergeFreeBlocks();
        printf("Allocation shrunk successfully.\n");
        return;
    }
    extraSize = newSize - oldSize;
    if (selectedIndex + 1 < blockCount &&
        blocks[selectedIndex + 1].isAllocated == 0 &&
        blocks[selectedIndex + 1].size >= extraSize) {
        blocks[selectedIndex].size = newSize;
        if (blocks[selectedIndex + 1].size == extraSize) {
            removeBlockAt(selectedIndex + 1);
        }
        else {
            blocks[selectedIndex + 1].start += extraSize;
            blocks[selectedIndex + 1].size -= extraSize;
        }
        printf("Allocation expanded in place successfully.\n");
        return;
    }
    if (!findSuitableBlock(newSize, &destinationIndex)) {
        printf("Resize failed: no suitable free block found.\n");
        return;
    }
    destinationStart = blocks[destinationIndex].start;
    if (blocks[destinationIndex].size > newSize) {
        if (!ensureCapacity(blockCount + 1)) {
            printf("Resize failed: metadata expansion failed.\n");
            return;
        }
    }
    allocateAtIndex(destinationIndex, newSize, allocationID);
    memcpy(memoryPool + destinationStart, memoryPool + oldStart, oldSize);
    if (findAllocationByIDAndStart(allocationID, oldStart, &oldIndex)) {
        blocks[oldIndex].isAllocated = 0;
        blocks[oldIndex].allocationID = 0;
    }
    mergeFreeBlocks();
    if (findAllocationByID(allocationID, &newIndex)) {
        printf("Allocation moved and resized successfully.\n");
        printf("New start: %zu\n", blocks[newIndex].start);
        printf("New size: %zu bytes\n", newSize);
        printf("Existing data preserved.\n");
    }
}

void displayMemoryMap(void) {
    printf("\nMEMORY MAP\n");
    printf("Total memory: %zu bytes\n", totalSize);
    for (size_t i = 0; i < blockCount; i++) {
        if (blocks[i].isAllocated == 1) {
            printf("[Allocated #%" PRIu64 ": %zu bytes, Start: %zu]\n",
                   blocks[i].allocationID, blocks[i].size, blocks[i].start);
        }
        else {
            printf("[Free: %zu bytes, Start: %zu]\n",
                   blocks[i].size, blocks[i].start);
        }
    }
}

void displayStatistics(void) {
    size_t allocatedBytes = 0;
    size_t freeBytes = 0;
    size_t allocatedBlocks = 0;
    size_t freeBlocks = 0;
    size_t largestFreeBlock = 0;
    for (size_t i = 0; i < blockCount; i++) {
        if (blocks[i].isAllocated == 1) {
            allocatedBytes += blocks[i].size;
            allocatedBlocks++;
        }
        else {
            freeBytes += blocks[i].size;
            freeBlocks++;
            if (blocks[i].size > largestFreeBlock) {
                largestFreeBlock = blocks[i].size;
            }
        }
    }

    printf("\nMEMORY STATISTICS\n");
    printf("Total memory: %zu bytes\n", totalSize);
    printf("Allocated memory: %zu bytes\n", allocatedBytes);
    printf("Free memory: %zu bytes\n", freeBytes);
    printf("Allocated blocks: %zu\n", allocatedBlocks);
    printf("Free blocks: %zu\n", freeBlocks);
    printf("Largest free block: %zu bytes\n", largestFreeBlock);
}

void changeStrategy(void) {
    uint64_t choice;
    printf("\nSelect allocation strategy:\n");
    printf("1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");
    if (!readNumber("Choice: ", 1, 3, &choice)) {
        return;
    }
    strategy = (enum AllocationStrategy)choice;
    printf("Allocation strategy changed successfully.\n");
}

void displayMenu(void) {
    printf("\nSMART MEMORY MANAGER\n");
    printf("1. Allocate memory\n");
    printf("2. Release memory\n");
    printf("3. Resize an allocation\n");
    printf("4. Display memory map\n");
    printf("5. Display memory statistics\n");
    printf("6. Change allocation strategy\n");
    printf("7. Exit\n");
}

void freeMemoryPool(void) {
    free(memoryPool);
    free(blocks);
    memoryPool = NULL;
    blocks = NULL;
    blockCount = 0;
    capacity = 0;
    totalSize = 0;
}

int main(void) {
    uint64_t input;
    int running = 1;
    if (!readNumber("Enter memory pool size: ", 1, MAX_POOL_SIZE, &input)) {
        printf("Input ended.\n");
        return 0;
    }
    if (!initializeMemoryPool((size_t)input)) {
        freeMemoryPool();
        return 1;
    }
    printf("\nMemory pool initialized successfully.\n");
    printf("\nSelect allocation strategy:\n");
    printf("1. First Fit\n");
    printf("2. Best Fit\n");
    printf("3. Worst Fit\n");
    if (!readNumber("Choice: ", 1, 3, &input)) {
        freeMemoryPool();
        return 0;
    }
    strategy = (enum AllocationStrategy)input;
    printf("Strategy selected successfully.\n");
    while (running) {
        displayMenu();
        if (!readNumber("Choice: ", 1, 7, &input)) {
            printf("Input ended. Exiting.\n");
            break;
        }
        switch (input) {
            case 1:
                if (!readNumber("Requested size: ",
                                1, totalSize, &input)) {
                    running = 0;
                    break;
                }
                allocateMemory((size_t)input);
                break;

            case 2:
                if (!readNumber("Allocation ID: ",
                                1, UINT64_MAX, &input)) {
                    running = 0;
                    break;
                }
                releaseMemory(input);
                break;

            case 3: {
                uint64_t allocationID;
                uint64_t newSize;
                if (!readNumber("Allocation ID: ",
                                1, UINT64_MAX, &allocationID)) {
                    running = 0;
                    break;
                }
                if (!readNumber("New size: ",
                                1, totalSize, &newSize)) {
                    running = 0;
                    break;
                }
                resizeAllocation(allocationID, (size_t)newSize);
                break;
            }

            case 4:
                displayMemoryMap();
                break;

            case 5:
                displayStatistics();
                break;

            case 6:
                changeStrategy();
                break;

            case 7:
                running = 0;
                printf("\nExiting Smart Memory Manager.\n");
                break;
        }
    }
    freeMemoryPool();
    printf("All memory released successfully.\n");
    return 0;
}