#ifndef TEXTLIB_H
   #define TEXTLIB_H

    #if DEBUG_LVL > 0

        #define CHECK_FUNC(arg)                                                       \
        {                                                                              \
            if (arg){                                                                   \
            writeLogs("[%s:%d] --- %s failed\n", __func__, __LINE__, #arg);   \
            return 1;                                                                     \
            }                                                                              \
        }

        #define CHECK_PTR(arg) {                                                               \
            if (!(arg)) {                                                                       \
                writeLogs("[%s:%d] --- %s failed\n\n\n", __func__, __LINE__, #arg);   \
                return 1;                                                                         \
            }                                                                                      \
        }                                                                           
    #else
        #define CHECK_PTR(arg) ;
        #define CHECK_FUNC(arg) arg;

    #endif
    struct Line {
      char* string;
      int length;
    };

    struct Text {
      long file_length;
      int string_amount;
      char* text_buffer;
      Line* text;
    };


    int  constructText(FILE *sourse, Text *text);
    int  input(FILE* fp, int *string_amount, char **text_buffer);
    int  to_strings(char *text_buffer, Line *text, int string_amount);
    int  fileLength(long *file_length, FILE *fp);
    int  output(Line *text, int string_amount, FILE *out);
    int  skipSpace(char *string);
    int  finish_text(Text* Onegin);
    void eatInputBuff();
    int GetLine(char *string);


#endif