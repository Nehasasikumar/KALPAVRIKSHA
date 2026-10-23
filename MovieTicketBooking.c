#include <stdio.h>
#define MAX_NAME_LENGTH 100
#define CATEGORY_NAME_LENGTH 20
#define MIN_TICKETS 1
#define MAX_TICKETS 20
const int STANDARD_PRICE = 150;
const int PREMIUM_PRICE = 250;
const int RECLINER_PRICE = 400;
const float DISCOUNT_RATE = 0.10f;
typedef enum {
    STANDARD = 1,
    PREMIUM,
    RECLINER
} TicketCategory;

int validateTicketCount(int tickets) {
    return tickets >= MIN_TICKETS && tickets <= MAX_TICKETS;
}

int readCustomerName(char customerName[]) {
    int ch;
    int length = 0;
    int hasSpace = 0;
    int isNameTooLong = 0;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        if (ch == ' ' || ch == '\t') {
            hasSpace = 1;
        }
        if (length < MAX_NAME_LENGTH - 1) {
            customerName[length++] = (char)ch;
        } else {
            isNameTooLong = 1;
        }
    }
    customerName[length] = '\0';
    if (length == 0 || hasSpace || isNameTooLong) {
        printf("Invalid customer name. Enter a single word.\n");
        return 0;
    }
    return 1;
}

int readBookingDetails(char customerName[], int *tickets, TicketCategory *category) {
    int categoryInput;
    int isTicketCountValid;
    int isCategoryValid;
    if (!readCustomerName(customerName)) {
        return 0;
    }
    if (scanf("%d", tickets) != 1) {
        printf("Invalid ticket count input.\n");
        return 0;
    }
    if (scanf("%d", &categoryInput) != 1) {
        printf("Invalid category input.\n");
        return 0;
    }
    isTicketCountValid = validateTicketCount(*tickets);
    isCategoryValid = categoryInput >= STANDARD && categoryInput <= RECLINER;
    if (!isTicketCountValid) {
        printf("Invalid ticket count. Enter between %d and %d.\n",
               MIN_TICKETS, MAX_TICKETS);
    }
    if (!isCategoryValid) {
        printf("Invalid ticket category. Choose 1, 2, or 3.\n");
    }
    if (!isTicketCountValid || !isCategoryValid) {
        return 0;
    }
    *category = (TicketCategory)categoryInput;
    return 1;
}

int getTicketDetails(TicketCategory category, int *ticketPrice, char categoryName[]) {
    switch (category) {
        case STANDARD:
            *ticketPrice = STANDARD_PRICE;
            snprintf(categoryName, CATEGORY_NAME_LENGTH, "Standard");
            break;
        case PREMIUM:
            *ticketPrice = PREMIUM_PRICE;
            snprintf(categoryName, CATEGORY_NAME_LENGTH, "Premium");
            break;
        case RECLINER:
            *ticketPrice = RECLINER_PRICE;
            snprintf(categoryName, CATEGORY_NAME_LENGTH, "Recliner");
            break;
        default:
            return 0;
    }
    return 1;
}

float calculateSubtotal(int tickets, int ticketPrice) {
    return tickets * ticketPrice;
}

float calculateDiscount(int tickets, float subtotal) {
    if (tickets >= 5) {
        return subtotal * DISCOUNT_RATE;
    }
    return 0.0f;
}

void displayBill(char customerName[], char categoryName[], int tickets, float subtotal, float discount, float finalBill) {
    printf("Customer: %s\n", customerName);
    printf("Ticket Category: %s\n", categoryName);
    printf("Tickets: %d\n", tickets);
    printf("Subtotal: %.2f\n", subtotal);
    printf("Discount: %.2f\n", discount);
    printf("Final Bill: %.2f\n", finalBill);
}

int main(void) {
    char customerName[MAX_NAME_LENGTH];
    char categoryName[CATEGORY_NAME_LENGTH];
    int tickets, ticketPrice;
    TicketCategory category;
    float subtotal, discount, finalBill;
    if (!readBookingDetails(customerName, &tickets, &category)) {
        return 1;
    }
    if (!getTicketDetails(category, &ticketPrice, categoryName)) {
        printf("Invalid ticket category. Choose 1, 2, or 3.\n");
        return 1;
    }
    subtotal = calculateSubtotal(tickets, ticketPrice);
    discount = calculateDiscount(tickets, subtotal);
    finalBill = subtotal - discount;
    displayBill(customerName, categoryName, tickets, subtotal, discount, finalBill);
    return 0;
}