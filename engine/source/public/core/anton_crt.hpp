#ifndef CORE_ANTON_CRT_HPP_INCLUDE
#define CORE_ANTON_CRT_HPP_INCLUDE

extern "C" {
    #if defined(_WIN64)
        #define ANTON_CRT_IMPORT __declspec(dllimport)
    #endif

    // stddef.h

    #ifndef offsetof
        #define offsetof(s,m) __builtin_offsetof(s,m)
    #endif

    // time.h

    struct timespec;

    // math.h

    ANTON_CRT_IMPORT float powf(float, float);
    ANTON_CRT_IMPORT float sqrtf(float);
    ANTON_CRT_IMPORT float sinf(float);
    ANTON_CRT_IMPORT float cosf(float);
    ANTON_CRT_IMPORT float roundf(float);
    ANTON_CRT_IMPORT float floorf(float);
    ANTON_CRT_IMPORT float ceilf(float);
    ANTON_CRT_IMPORT float tanf(float);

    // string.h
    // memset, memmove, memcpy, strlen don't use dllimport on win.
    
    void* memset(void* dest, int value, unsigned long long count);
    void* memcpy(void* dest, void const* src, unsigned long long count);
    void* memmove(void* dest, void const* src, unsigned long long count);
    int memcmp(void const* lhs, void const* rhs, unsigned long long count);

    unsigned long long strlen(char const* string);

    // stdio.h

    #if defined(_WIN64)
        #ifndef _FILE_DEFINED
            #define _FILE_DEFINED

            typedef struct _iobuf {
                void* _Placeholder;
            } FILE;
        #endif

        ANTON_CRT_IMPORT FILE* __acrt_iob_func(unsigned _Ix);

        #define stdin  (__acrt_iob_func(0))
        #define stdout (__acrt_iob_func(1))
        #define stderr (__acrt_iob_func(2))
    #else
        #ifndef __FILE_defined
            #define __FILE_defined 1
            typedef struct _IO_FILE FILE;
        #endif

        extern FILE* stdin;
        extern FILE* stdout;
        extern FILE* stderr;
    #endif

    #define SEEK_SET	0
    #define SEEK_CUR	1
    #define SEEK_END	2

    #define EOF    (-1)

    ANTON_CRT_IMPORT FILE* fopen(char const* filename, char const* modes);
    ANTON_CRT_IMPORT FILE* freopen(char const* filename, char const* mode, FILE* stream);
    ANTON_CRT_IMPORT int fclose(FILE* stream);
    ANTON_CRT_IMPORT int fflush(FILE* stream);
    ANTON_CRT_IMPORT void setbuf(FILE* stream, char* buffer);
    ANTON_CRT_IMPORT int setvbuf(FILE* stream, char* buffer, int mode, unsigned long long size);
    ANTON_CRT_IMPORT unsigned long long fread(void* buffer, unsigned long long size, unsigned long long count, FILE* stream);
    ANTON_CRT_IMPORT unsigned long long fwrite(void const* buffer, unsigned long long size, unsigned long long count, FILE* stream);
    ANTON_CRT_IMPORT int fgetc(FILE* stream);
    ANTON_CRT_IMPORT char* fgets(char*, int count, FILE* stream);
    ANTON_CRT_IMPORT char* gets(char* string);
    ANTON_CRT_IMPORT int getchar(void);
    ANTON_CRT_IMPORT int fputc(int ch, FILE* stream);
    ANTON_CRT_IMPORT int fputs(char const* string, FILE* stream);
    ANTON_CRT_IMPORT int puts(char const* string);
    ANTON_CRT_IMPORT int putchar(int ch);
    ANTON_CRT_IMPORT int ungetc(int ch, FILE* stream);
    ANTON_CRT_IMPORT long ftell(FILE* stream);
    ANTON_CRT_IMPORT int fseek(FILE* stream, long offset, int origin);
    ANTON_CRT_IMPORT void rewind(FILE* stream);
    ANTON_CRT_IMPORT int ferror(FILE* stream);
    ANTON_CRT_IMPORT int feof(FILE* stream);
    ANTON_CRT_IMPORT void clearerr(FILE* stream);

    #undef ANTON_CRT_IMPORT
}

#endif // !CORE_ANTON_CRT_HPP_INCLUDE
