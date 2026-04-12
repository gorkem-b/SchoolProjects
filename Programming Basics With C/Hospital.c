/**
 * Hospital Management System demo providing interactive console options to display hospitals and patients,
 * sort hospitals by various criteria (bed price, bed count, name, weighted rating), and filter hospitals by city.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HOSPITAL_COUNT 5
#define PATIENTS_PER_HOSPITAL 3

struct Hospital {
    char name[50];
    char city[50];
    int beds;
    float price;
    float rating;
    int reviews;
};

struct Patient {
    char name[50];
    int age;
};

static void printHospital(const struct Hospital *hosp) {
    printf("Hospital Name: %s\n", hosp->name);
    printf("City: %s\n", hosp->city);
    printf("Total Beds: %d\n", hosp->beds);
    printf("Price per Bed: $%.2f\n", hosp->price);
    printf("Rating: %.1f\n", hosp->rating);
    printf("Reviews: %d\n\n", hosp->reviews);
}

static void printAllHospitals(const struct Hospital hospitals[], int count) {
    for (int i = 0; i < count; ++i) {
        printHospital(&hospitals[i]);
    }
}

static void printPatient(const struct Patient *patient) {
    printf("Patient Name: %s\n", patient->name);
    printf("Age: %d\n\n", patient->age);
}

static int comparePriceAsc(const void *a, const void *b) {
    const struct Hospital *lhs = a;
    const struct Hospital *rhs = b;
    return (lhs->price > rhs->price) - (lhs->price < rhs->price);
}

static int compareBedsDesc(const void *a, const void *b) {
    const struct Hospital *lhs = a;
    const struct Hospital *rhs = b;
    return (rhs->beds > lhs->beds) - (rhs->beds < lhs->beds);
}

static int compareNameAsc(const void *a, const void *b) {
    const struct Hospital *lhs = a;
    const struct Hospital *rhs = b;
    return strcmp(lhs->name, rhs->name);
}

static int compareWeightedRatingDesc(const void *a, const void *b) {
    const struct Hospital *lhs = a;
    const struct Hospital *rhs = b;
    double lhsScore = lhs->rating * lhs->reviews;
    double rhsScore = rhs->rating * rhs->reviews;
    return (rhsScore > lhsScore) - (rhsScore < lhsScore);
}

static int equalsIgnoreCase(const char *lhs, const char *rhs) {
    while (*lhs && *rhs) {
        int lc = tolower((unsigned char)*lhs++);
        int rc = tolower((unsigned char)*rhs++);
        if (lc != rc) {
            return 0;
        }
    }
    return *lhs == '\0' && *rhs == '\0';
}

static void printHospitalsInCity(const struct Hospital hospitals[], int count) {
    char city[50];
    int found = 0;

    printf("Enter city name: ");
    if (scanf("%49s", city) != 1) {
        return;
    }

    printf("Hospitals in %s:\n", city);
    for (int i = 0; i < count; ++i) {
        if (equalsIgnoreCase(hospitals[i].city, city)) {
            printHospital(&hospitals[i]);
            ++found;
        }
    }

    if (!found) {
        printf("No hospitals found in %s\n", city);
    }
}

int main(void) {
    struct Hospital hospitals[HOSPITAL_COUNT] = {
        {"Hospital A", "X", 100, 250.0f, 4.5f, 100},
        {"Hospital B", "Y", 150, 200.0f, 4.2f, 80},
        {"Hospital C", "X", 200, 180.0f, 4.0f, 120},
        {"Hospital D", "Z", 80, 300.0f, 4.8f, 90},
        {"Hospital E", "Y", 120, 220.0f, 4.6f, 110}
    };

    struct Patient patients[HOSPITAL_COUNT][PATIENTS_PER_HOSPITAL] = {
        {{"Amar", 35}, {"Manish", 45}, {"Atul", 28}},
        {{"Elvish", 62}, {"Debolina", 18}, {"Shruti", 55}},
        {{"Zafar", 50}, {"Rahul", 30}, {"Priya", 40}},
        {{"Amir", 22}, {"Asif", 38}, {"Prince", 60}},
        {{"Aditya", 28}, {"Aman", 48}, {"Sahil", 33}}
    };

    int choice;

    do {
        printf("\n*********** Hospital Management System Menu ***********\n\n");
        printf("1. Print Hospital Data\n");
        printf("2. Print Patients Data\n");
        printf("3. Sort Hospitals by Bed Price (Ascending)\n");
        printf("4. Sort Hospitals by Available Beds (Descending)\n");
        printf("5. Sort Hospitals by Name (Ascending)\n");
        printf("6. Sort Hospitals by Rating and Reviews (Descending)\n");
        printf("7. Print Hospitals in a Specific City\n");
        printf("8. Exit\n\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            return 0;
        }

        switch (choice) {
            case 1:
                printf("\nPrinting Hospital Data:\n\n");
                printAllHospitals(hospitals, HOSPITAL_COUNT);
                break;
            case 2:
                printf("Printing Patients Data:\n\n");
                for (int i = 0; i < HOSPITAL_COUNT; ++i) {
                    printf("Hospital: %s\n", hospitals[i].name);
                    for (int j = 0; j < PATIENTS_PER_HOSPITAL; ++j) {
                        printPatient(&patients[i][j]);
                    }
                }
                break;
            case 3:
                printf("Sorting Hospitals by Bed Price (Ascending):\n");
                qsort(hospitals, HOSPITAL_COUNT, sizeof(struct Hospital), comparePriceAsc);
                printAllHospitals(hospitals, HOSPITAL_COUNT);
                break;
            case 4:
                printf("Sorting Hospitals by Available Beds (Descending):\n");
                qsort(hospitals, HOSPITAL_COUNT, sizeof(struct Hospital), compareBedsDesc);
                printAllHospitals(hospitals, HOSPITAL_COUNT);
                break;
            case 5:
                printf("Sorting Hospitals by Name (Ascending):\n");
                qsort(hospitals, HOSPITAL_COUNT, sizeof(struct Hospital), compareNameAsc);
                printAllHospitals(hospitals, HOSPITAL_COUNT);
                break;
            case 6:
                printf("Sorting Hospitals by Rating and Reviews (Descending):\n");
                qsort(hospitals, HOSPITAL_COUNT, sizeof(struct Hospital), compareWeightedRatingDesc);
                printAllHospitals(hospitals, HOSPITAL_COUNT);
                break;
            case 7:
                printHospitalsInCity(hospitals, HOSPITAL_COUNT);
                break;
            case 8:
                printf("Exiting the program.\n");
                break;
            default:
                printf("Invalid choice. Please enter a valid option.\n");
                break;
        }
    } while (choice != 8);

    return 0;
}
