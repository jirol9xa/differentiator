#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include "LogsLib.h"

static FILE *logs = nullptr;


int openLogs(const char *logs_file_name)
{
    logs = fopen(logs_file_name, "w");

    if (logs == nullptr)
    {
        printf("!!! ERROR can't open log file !!!\n");
        return -1;
    }

    return 0;
}


int closeLogs()
{
    fclose(logs);
    return 0;
}


void writeLogs(const char *format, ...)
{
    va_list args;
    va_start(args, format);

    vfprintf(logs, format, args);

    va_end(args);
}


void FFlush()
{
    fflush(logs);
}


int FFFFFree(const char *file)
{
    FILE *file_ptr = fopen(file, "w");
    if (!file_ptr)
    {
        printf("!!! ERROR Can't open file %s !!!\n", file);
        return 1;
    }

    fclose(file_ptr);
    return 0;
}



void printReshetka()
{
    writeLogs("#################################################################\n");
}
