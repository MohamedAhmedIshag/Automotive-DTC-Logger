#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    char code[8];
    char description[256];
    char severity[32];
} DTC_Database;

DTC_Database dtc_lookup[] = {
    {"P0300", "Random/Multiple Cylinder Misfire Detected (تعدد فقد الإشعال في المحرك)", "CRITICAL (حرج)"},
    {"P0171", "System Too Lean - Bank 1 (خليط الوقود فقير جداً)", "MEDIUM (متوسط)"},
    {"P0420", "Catalyst System Efficiency Below Threshold (ضعف كفاءة دبة التلوث)", "LOW (خفيف)"},
    {"P0117", "Engine Coolant Temp Sensor Low Input (خلل حساس حرارة السائل)", "CRITICAL (حرج)"},
    {"P0700", "Transmission Control System Malfunction (عطل في نظام التحكم بالجير)", "HIGH (عالي)"}
};

int total_known_codes = sizeof(dtc_lookup) / sizeof(dtc_lookup[0]);

int diagnose_code(const char *input_code, DTC_Database *result) {
    for (int i = 0; i < total_known_codes; i++) {
        if (strcmp(input_code, dtc_lookup[i].code) == 0) {
            *result = dtc_lookup[i];
            return 1;
        }
    }
    return 0;
}

void log_dtc_event(const char *vin, DTC_Database dtc) {
    FILE *file = fopen("dtc_diagnostic_history.txt", "a");
    
    if (file == NULL) {
        printf("[ERROR] Failed to open log file!\n");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char date_time_str[30];
    strftime(date_time_str, sizeof(date_time_str), "%Y-%m-%d %H:%M:%S", t);

    fprintf(file, "====================================================\n");
    fprintf(file, "Date & Time   : %s\n", date_time_str);
    fprintf(file, "VIN           : %s\n", vin);
    fprintf(file, "DTC Code      : %s\n", dtc.code);
    fprintf(file, "Severity      : %s\n", dtc.severity);
    fprintf(file, "Description   : %s\n", dtc.description);
    fprintf(file, "====================================================\n\n");

    fclose(file);
    printf("\n[SUCCESS] Alert logged to 'dtc_diagnostic_history.txt'\n");
}

int main() {
    char user_vin[32];
    char user_code[16];
    DTC_Database current_diagnostic;
    int choice;

    printf("======================================================\n");
    printf("        Automotive DTC Scanner & Logger              \n");
    printf("======================================================\n\n");

    printf("Enter Vehicle VIN [17 Characters]: ");
    scanf("%31s", user_vin);

    do {
        printf("\nEnter DTC Code (e.g. P0300): ");
        scanf("%15s", user_code);

        if (diagnose_code(user_code, &current_diagnostic)) {
            printf("\n--- DIAGNOSTIC RESULT ---\n");
            printf("Code        : %s\n", current_diagnostic.code);
            printf("Severity    : %s\n", current_diagnostic.severity);
            printf("Description : %s\n", current_diagnostic.description);

            log_dtc_event(user_vin, current_diagnostic);
        } else {
            printf("\n[WARNING] Code %s not found in database.\n", user_code);
        }

        printf("\nDiagnose another code? (1: Yes / 0: Exit): ");
        scanf("%d", &choice);

    } while (choice != 0);

    printf("\nExiting System...\n");
    return 0;
}