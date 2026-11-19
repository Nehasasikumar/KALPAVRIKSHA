
#include <stdio.h>
#include <stdlib.h>
struct Node {
    int data;
    struct Node* next;
};
struct Node* createNode(int data) {
    struct Node* newNode = malloc(sizeof(struct Node));
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}
struct Node* reverse(struct Node* head) {
    struct Node* prev = NULL;
    struct Node* current = head;
    struct Node* next = NULL;
    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    return prev;
}
void display(struct Node* head) {
    while (head != NULL) {
        printf("%d ", head->data);
        head = head->next;
    }
    printf("\n");
}
int main() {
    int t, n, value;
    printf("Enter number of test cases: ");
    scanf("%d", &t);
    while (t--) {
        struct Node* head = NULL;
        struct Node* tail = NULL;
        printf("Enter number of nodes: ");
        scanf("%d", &n);
        for (int i = 0; i < n; i++) {
            scanf("%d", &value);
            struct Node* newNode = createNode(value);
            if (head == NULL) {
                head = newNode;
                tail = newNode;
            } else {
                tail->next = newNode;
                tail = newNode;
            }
        }
        head = reverse(head);
        printf("Reversed list: ");
        display(head);
        while (head != NULL) {
            struct Node* temp = head;
            head = head->next;
            free(temp);
        }
    }
    return 0;
}
