#include "../libc/stdio/stdio.h"
#include "../libc/stdlib/stdlib.h"
#include "../libc/string/string.h"

// Calendar program
// Usage: cal [month] [year]
// If no arguments, displays current month (placeholder - needs RTC)

// Days in each month (non-leap year)
static const int days_in_month[] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

// Month names
static const char* month_names[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

// Day names
static const char* day_names[] = {
    "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"
};

// Check if year is leap year
static int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Get days in month
static int get_days_in_month(int month, int year) {
    if (month == 1 && is_leap_year(year)) {
        return 29; // February in leap year
    }
    return days_in_month[month];
}

// Calculate day of week for January 1st of given year
// Uses Zeller's congruence (simplified)
static int get_january_1_weekday(int year) {
    // Zeller's congruence for January 1st
    int y = year;
    int m = 13; // January is treated as month 13 of previous year
    int k = y % 100;
    int j = y / 100;
    
    int h = (1 + (13 * (m + 1)) / 5 + k + k / 4 + j / 4 - 2 * j) % 7;
    
    // Convert to Sunday = 0 format
    return (h + 5) % 7;
}

// Calculate day of week for first day of given month/year
static int get_month_start_weekday(int month, int year) {
    // Start from January 1st
    int weekday = get_january_1_weekday(year);
    
    // Add days for each month before target month
    for (int m = 0; m < month; m++) {
        weekday += get_days_in_month(m, year);
    }
    
    return weekday % 7;
}

// Print calendar for given month/year
static void print_calendar(int month, int year) {
    // Print header
    printf("    %s %d\n", month_names[month], year);
    printf("Su Mo Tu We Th Fr Sa\n");
    
    // Get first day of month
    int start_day = get_month_start_weekday(month, year);
    int days = get_days_in_month(month, year);
    
    // Print leading spaces
    for (int i = 0; i < start_day; i++) {
        printf("   ");
    }
    
    // Print days
    for (int day = 1; day <= days; day++) {
        printf("%2d", day);
        
        // Check if we need a newline
        int current_day = (start_day + day - 1) % 7;
        if (current_day == 6) {
            printf("\n");
        } else {
            printf(" ");
        }
    }
    
    // Print final newline if needed
    int last_day = (start_day + days - 1) % 7;
    if (last_day != 6) {
        printf("\n");
    }
}

int main(int argc, char* argv[]) {
    int month = 0;  // 0-11 (January = 0)
    int year = 2024; // Default year
    
    if (argc == 1) {
        // No arguments - use current month/year (placeholder)
        // TODO: Get from RTC when available
        month = 11; // December
        year = 2024;
    } else if (argc == 2) {
        // Only year specified
        year = atoi(argv[1]);
        if (year < 1 || year > 9999) {
            printf("Error: Invalid year\n");
            return 1;
        }
        // Display entire year
        for (int m = 0; m < 12; m++) {
            print_calendar(m, year);
            printf("\n");
        }
        return 0;
    } else if (argc == 3) {
        // Month and year specified
        month = atoi(argv[1]) - 1; // Convert to 0-based
        year = atoi(argv[2]);
        
        if (month < 0 || month > 11) {
            printf("Error: Invalid month (1-12)\n");
            return 1;
        }
        if (year < 1 || year > 9999) {
            printf("Error: Invalid year\n");
            return 1;
        }
    } else {
        printf("Usage: cal [month] [year]\n");
        printf("  cal          - Display current month\n");
        printf("  cal <year>   - Display entire year\n");
        printf("  cal <month> <year> - Display specific month\n");
        return 1;
    }
    
    print_calendar(month, year);
    return 0;
}

