/**
 * @brief Runs a simple console-based digital clock that displays the current local time.
 *
 * Continuously retrieves the system's local time each second and refreshes the console output
 * to show the current hour, minute, and second in HH:MM:SS format.
 */
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

int main(void) {
    time_t rawtime = 0;
    struct tm *pTime = NULL;
    bool isRunning = true;

    printf("DIGITAL CLOCK\n");

    while (isRunning) {
        time(&rawtime);
        pTime = localtime(&rawtime);

        if (pTime != NULL) {
            printf("\r%02d:%02d:%02d", pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
            fflush(stdout);
        }

#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }

    return 0;
}