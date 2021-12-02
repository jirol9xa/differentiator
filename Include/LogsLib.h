#ifndef LOGSLIB_H
    #define LOGSLIB_H

    int openLogs(const char *logs_file_name);
    void writeLogs(const char *format, ...);
    int closeLogs();
    void FFlush();
    void printReshetka();
    int FFFFFree(const char *file);

    #define PRINT_LINE printf("[%s:%d]\n", __func__, __LINE__);
#endif