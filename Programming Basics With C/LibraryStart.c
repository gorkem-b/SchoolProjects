/**
 * @file LibraryStart.c
 * @brief Console-based e-library management program for storing and querying book records.
 *
 * This application maintains up to 100 books, allowing users to:
 *   - Add a new book with its name, author, page count, and price.
 *   - Display all stored books.
 *   - List books by a specific author.
 *   - View the total number of books currently stored.
 *
 * Input is validated to handle non-numeric menu selections gracefully.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct library {
    char book_name[20];
    char author[20];
    int pages;
    float price;
};

int main() {
    struct library lib[100];
    char ar_nm[30];
    int i, input, count;
    i = input = count = 0;

    while (input != 5) {
        printf("\n\n********######"
               "WELCOME TO E-LIBRARY "
               "#####********\n");
        printf("\n\n1. Add book information\n2. Display book information\n");
        printf("3. List all books of given author\n");
        printf("4. List the count of books in the library\n");
        printf("5. Exit");
        printf("\n\nEnter one of the above: ");
        int scanres = scanf("%d", &input);
        if (scanres != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            input = 0;
            continue;
        }

        switch (input) {
            case 1:
                if (count >= 100) {
                    printf("Library full\n");
                    break;
                }
                printf("Enter book name = ");
                scanf("%19s", lib[count].book_name);
                printf("Enter author name = ");
                scanf("%19s", lib[count].author);
                printf("Enter pages = ");
                scanf("%d", &lib[count].pages);
                printf("Enter price = ");
                scanf("%f", &lib[count].price);
                count++;
                break;
            case 2:
                printf("you have entered"
                       " the following "
                       "information\n");
                for (i = 0; i < count; i++) {
                    printf("book name = %s", lib[i].book_name);
                    printf("\t author name = %s", lib[i].author);
                    printf("\t  pages = %d", lib[i].pages);
                    printf("\t  price = %f", lib[i].price);
                }
                break;
            case 3:
                printf("Enter author name : ");
                scanf("%s", ar_nm);
                for (i = 0; i < count; i++) {
                    if (strcmp(ar_nm, lib[i].author) == 0)
                        printf("%s %s %d %f", lib[i].book_name, lib[i].author, lib[i].pages, lib[i].price);
                }
                break;
            case 4:
                printf("\n No of books in library : %d", count);
                break;
            case 5:
                exit(0);
        }
    }
    return 0;
}
