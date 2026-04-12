/**
 * @file StudentInfo.c
 * @brief Implements an in-memory student records manager with interactive console menu.
 *
 * The module stores up to MAX_STUDENTS entries, each capturing personal information,
 * CGPA, and enrollment across MAX_COURSES course identifiers. It provides routines to:
 *   - Add new students, enforcing capacity limits and validating user input.
 *   - Search records by roll number, first name, or course participation.
 *   - Display comprehensive details for matching entries.
 *   - Update individual fields or entire course lists for a specified roll number.
 *   - Delete records by roll number, maintaining array continuity.
 *   - Report current population and remaining capacity.
 *
 * Input handling checks for read errors and exits on invalid data to preserve consistency.
 * The main loop drives a text-based menu, dispatching to the appropriate operations
 * until the user opts to exit.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 50
#define MAX_NAME_LEN 50
#define MAX_COURSES 5

typedef struct {
    char fname[MAX_NAME_LEN];
    char lname[MAX_NAME_LEN];
    int roll;
    float cgpa;
    int courses[MAX_COURSES];
} Student;

static Student students[MAX_STUDENTS];
static size_t student_count = 0;

/* Display every field of a student record. */
static void print_student(const Student *student) {
    printf("The Students Details are\n");
    printf("The First name is %s\n", student->fname);
    printf("The Last name is %s\n", student->lname);
    printf("The Roll Number is %d\n", student->roll);
    printf("The CGPA is %.2f\n", student->cgpa);

    for (size_t course_idx = 0; course_idx < MAX_COURSES; ++course_idx) {
        printf("The course ID is %d\n", student->courses[course_idx]);
    }
}

/* Locate the index of a student by roll number. */
static size_t find_student_index_by_roll(int roll) {
    for (size_t index = 0; index < student_count; ++index) {
        if (students[index].roll == roll) {
            return index;
        }
    }
    return student_count;
}

/* Read every course ID for a student. */
static void read_courses(int courses[MAX_COURSES]) {
    printf("Enter the course ID of each course\n");

    for (size_t course_idx = 0; course_idx < MAX_COURSES; ++course_idx) {
        if (scanf("%d", &courses[course_idx]) != 1) {
            fprintf(stderr, "Input error.\n");
            exit(EXIT_FAILURE);
        }
    }
}

/* Add a new student to the collection. */
void add_student(void) {
    if (student_count >= MAX_STUDENTS) {
        printf("Student limit reached\n");
        return;
    }

    Student *student = &students[student_count];

    printf("Add the Students Details\n");
    printf("-------------------------\n");

    printf("Enter the first name of student\n");
    if (scanf("%49s", student->fname) != 1) {
        fprintf(stderr, "Input error.\n");
        exit(EXIT_FAILURE);
    }

    printf("Enter the last name of student\n");
    if (scanf("%49s", student->lname) != 1) {
        fprintf(stderr, "Input error.\n");
        exit(EXIT_FAILURE);
    }

    printf("Enter the Roll Number\n");
    if (scanf("%d", &student->roll) != 1) {
        fprintf(stderr, "Input error.\n");
        exit(EXIT_FAILURE);
    }

    printf("Enter the CGPA you obtained\n");
    if (scanf("%f", &student->cgpa) != 1) {
        fprintf(stderr, "Input error.\n");
        exit(EXIT_FAILURE);
    }

    read_courses(student->courses);
    ++student_count;
}

/* Find and print a student by roll number. */
void find_rl(void) {
    int roll;

    printf("Enter the Roll Number of the student\n");
    if (scanf("%d", &roll) != 1) {
        fprintf(stderr, "Input error.\n");
        exit(EXIT_FAILURE);
    }

    size_t student_index = find_student_index_by_roll(roll);
    if (student_index == student_count) {
        printf("Roll Number not Found\n");
        return;
    }

    print_student(&students[student_index]);
}

/* Find and print students by first name. */
void find_fn(void) {
    char name[MAX_NAME_LEN];

    printf("Enter the First Name of the student\n");
    if (scanf("%49s", name) != 1) {
        fprintf(stderr, "Input error.\n");
        exit(EXIT_FAILURE);
    }

    bool found = false;

    for (size_t index = 0; index < student_count; ++index) {
        if (strcmp(students[index].fname, name) == 0) {
            print_student(&students[index]);
            found = true;
        }
    }

    if (!found) {
        printf("The First Name not Found\n");
    }
}

/* Find and print students enrolled in a specific course. */
void find_c(void) {
    int course_id;

    printf("Enter the course ID \n");
    if (scanf("%d", &course_id) != 1) {
        fprintf(stderr, "Input error.\n");
        exit(EXIT_FAILURE);
    }

    bool found = false;

    for (size_t student_index = 0; student_index < student_count; ++student_index) {
        for (size_t course_idx = 0; course_idx < MAX_COURSES; ++course_idx) {
            if (students[student_index].courses[course_idx] == course_id) {
                print_student(&students[student_index]);
                found = true;
                break;
            }
        }
    }

    if (!found) {
        printf("Course ID not Found\n");
    }
}

/* Print totals and remaining capacity. */
void tot_s(void) {
    printf("The total number of Student is %zu\n", student_count);
    printf("\n you can have a max of %d students\n", MAX_STUDENTS);
    printf("you can have %zu more students\n", (size_t)(MAX_STUDENTS - student_count));
}

/* Delete a student by roll number. */
void del_s(void) {
    int roll;

    printf("Enter the Roll Number which you want to delete\n");
    if (scanf("%d", &roll) != 1) {
        fprintf(stderr, "Input error.\n");
        exit(EXIT_FAILURE);
    }

    size_t student_index = find_student_index_by_roll(roll);
    if (student_index == student_count) {
        printf("Roll Number not Found\n");
        return;
    }

    if (student_index + 1 < student_count) {
        memmove(
            &students[student_index],
            &students[student_index + 1],
            (student_count - student_index - 1) * sizeof(Student)
        );
    }

    --student_count;
    printf("DELETED SUCCESSFULLY.\n");
}

/* Update selected fields for an existing student. */
void up_s(void) {
    int roll;

    printf("Enter the roll number to update the entry : ");
    if (scanf("%d", &roll) != 1) {
        fprintf(stderr, "Input error.\n");
        exit(EXIT_FAILURE);
    }

    size_t student_index = find_student_index_by_roll(roll);
    if (student_index == student_count) {
        printf("Roll Number not Found\n");
        return;
    }

    printf(
        "1. first name\n"
        "2. last name\n"
        "3. roll no.\n"
        "4. CGPA\n"
        "5. courses\n"
    );

    int option;
    if (scanf("%d", &option) != 1) {
        fprintf(stderr, "Input error.\n");
        exit(EXIT_FAILURE);
    }

    switch (option) {
        case 1:
            printf("Enter the new first name : \n");
            if (scanf("%49s", students[student_index].fname) != 1) {
                fprintf(stderr, "Input error.\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 2:
            printf("Enter the new last name : \n");
            if (scanf("%49s", students[student_index].lname) != 1) {
                fprintf(stderr, "Input error.\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 3:
            printf("Enter the new roll number : \n");
            if (scanf("%d", &students[student_index].roll) != 1) {
                fprintf(stderr, "Input error.\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 4:
            printf("Enter the new CGPA : \n");
            if (scanf("%f", &students[student_index].cgpa) != 1) {
                fprintf(stderr, "Input error.\n");
                exit(EXIT_FAILURE);
            }
            break;

        case 5:
            printf("Enter the new courses \n");
            read_courses(students[student_index].courses);
            break;

        default:
            printf("Invalid option\n");
            return;
    }

    printf("UPDATED SUCCESSFULLY.\n");
}

/* Present the console menu for managing students. */
int main(void) {
    for (;;) {
        int choice;

        printf("The Task that you want to perform\n");
        printf("1. Add the Student Details\n");
        printf("2. Find the Student Details by Roll Number\n");
        printf("3. Find the Student Details by First Name\n");
        printf("4. Find the Student Details by Course Id\n");
        printf("5. Find the Total number of Students\n");
        printf("6. Delete the Students Details by Roll Number\n");
        printf("7. Update the Students Details by Roll Number\n");
        printf("8. To Exit\n");
        printf("Enter your choice to find the task\n");

        if (scanf("%d", &choice) != 1) {
            fprintf(stderr, "Input error.\n");
            return EXIT_FAILURE;
        }

        switch (choice) {
            case 1:
                add_student();
                break;

            case 2:
                find_rl();
                break;

            case 3:
                find_fn();
                break;

            case 4:
                find_c();
                break;

            case 5:
                tot_s();
                break;

            case 6:
                del_s();
                break;

            case 7:
                up_s();
                break;

            case 8:
                return 0;

            default:
                printf("Invalid choice\n");
                break;
        }
    }
}