#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME_LENGTH 100
#define CATEGORY_NAME_LENGTH 20

const int STANDARD_PRICE = 150;
const int PREMIUM_PRICE = 250;
const int RECLINER_PRICE = 400;
const float DISCOUNT_RATE = 0.10f;

int readCustomerName(char customerName[]) {
    int ch;
    int length = 0;
    int hasSpace = 0;
    int tooLong = 0;

    while ((ch = getchar()) != '\n' && ch != EOF) {
        if (ch == ' ' || ch == '\t') {
            hasSpace = 1;
        }
        if (length < MAX_NAME_LENGTH - 1) {
            customerName[length++] = (char)ch;
        } else {
            tooLong = 1;
        }
    }
    customerName[length] = '\0';
    if (length == 0 || hasSpace || tooLong) {
        printf("Invalid customer name. Enter a single word.\n");
        return 0;
    }
    return 1;
}

int readBookingDetails(char customerName[], int *tickets, int *category) {
    if (!readCustomerName(customerName)) {
        return 0;
    }
    if (scanf("%d", tickets) != 1) {
        printf("Invalid ticket count input.\n");
        return 0;
    }
    if (scanf("%d", category) != 1) {
        printf("Invalid category input.\n");
        return 0;
    }
    return 1;
}

int validateTicketCount(int tickets) {
    return tickets >= 1 && tickets <= 20;
}

int getTicketDetails(int category, int *ticketPrice, char categoryName[]) {
    switch (category) {
        case 1:
            *ticketPrice = STANDARD_PRICE;
            snprintf(categoryName, CATEGORY_NAME_LENGTH,
                     "Standard");
            break;
        case 2:
            *ticketPrice = PREMIUM_PRICE;
            snprintf(categoryName, CATEGORY_NAME_LENGTH,
                     "Premium");
            break;
        case 3:
            *ticketPrice = RECLINER_PRICE;
            snprintf(categoryName, CATEGORY_NAME_LENGTH,
                     "Recliner");
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
    int tickets, category, ticketPrice;
    float subtotal, discount, finalBill;
    if (!readBookingDetails(customerName, &tickets, &category))
        return 1;
    if (!validateTicketCount(tickets)) {
        printf("Invalid ticket count. Enter between 1 and 20.\n");
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
