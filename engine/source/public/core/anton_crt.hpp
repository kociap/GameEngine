#pragma once

#if defined(_WIN64)
    #define ANTON_NOEXCEPT
    #define ANTON_CRT_IMPORT __declspec(dllimport)
    #define size_t unsigned long long
#else 
    #define ANTON_NOEXCEPT noexcept
    #define ANTON_CRT_IMPORT
    #define size_t unsigned long int
#endif

// C Runtime Forward Declarations

extern "C" {
    // stddef.h

    #ifndef offsetof
        #define offsetof(s,m) __builtin_offsetof(s,m)
    #endif

    // time.h

    struct timespec;

    // math.h

    ANTON_CRT_IMPORT float powf(float, float) ANTON_NOEXCEPT;
    ANTON_CRT_IMPORT float sqrtf(float) ANTON_NOEXCEPT;
    ANTON_CRT_IMPORT float cbrtf(float) ANTON_NOEXCEPT;
    ANTON_CRT_IMPORT float roundf(float) ANTON_NOEXCEPT;
    ANTON_CRT_IMPORT float floorf(float) ANTON_NOEXCEPT;
    ANTON_CRT_IMPORT float ceilf(float) ANTON_NOEXCEPT;
    ANTON_CRT_IMPORT float modff(float, float*) ANTON_NOEXCEPT;
    ANTON_CRT_IMPORT float sinf(float) ANTON_NOEXCEPT;
    ANTON_CRT_IMPORT float cosf(float) ANTON_NOEXCEPT;
    ANTON_CRT_IMPORT float tanf(float) ANTON_NOEXCEPT;
    ANTON_CRT_IMPORT float asinf(float) ANTON_NOEXCEPT;
    ANTON_CRT_IMPORT float acosf(float) ANTON_NOEXCEPT;
    ANTON_CRT_IMPORT float atanf(float) ANTON_NOEXCEPT;
    ANTON_CRT_IMPORT float atan2f(float, float) ANTON_NOEXCEPT;
    ANTON_CRT_IMPORT float expf(float) ANTON_NOEXCEPT;
    ANTON_CRT_IMPORT float logf(float) ANTON_NOEXCEPT;
    ANTON_CRT_IMPORT float log10f(float) ANTON_NOEXCEPT;
    ANTON_CRT_IMPORT float log2f(float) ANTON_NOEXCEPT;


    // string.h
    // memset, memmove, memcpy, strlen don't use dllimport on win.
    
    void* memset(void* dest, int value, size_t count);
    void* memcpy(void* dest, void const* src, size_t count);
    void* memmove(void* dest, void const* src, size_t count);
    int memcmp(void const* lhs, void const* rhs, size_t count);

    size_t strlen(char const* string);

    // stdlib.h

    ANTON_CRT_IMPORT float strtof(char const*, char**);
    ANTON_CRT_IMPORT double strtod(char const*, char**);
    ANTON_CRT_IMPORT long long strtoll(char const*, char**, int base);
    ANTON_CRT_IMPORT unsigned long long strtoull(char const*, char**, int base);

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

    #if defined(_WIN64)
        #define ANTON_CRT_STDIO_NOEXCEPT
    #else
        #define ANTON_CRT_STDIO_NOEXCEPT noexcept
    #endif

    ANTON_CRT_IMPORT FILE* fopen(char const* filename, char const* modes);
    ANTON_CRT_IMPORT FILE* freopen(char const* filename, char const* mode, FILE* stream);
    ANTON_CRT_IMPORT int fclose(FILE* stream);
    ANTON_CRT_IMPORT int fflush(FILE* stream);
    ANTON_CRT_IMPORT void setbuf(FILE* stream, char* buffer) ANTON_CRT_STDIO_NOEXCEPT;
    ANTON_CRT_IMPORT int setvbuf(FILE* stream, char* buffer, int mode, size_t size) ANTON_CRT_STDIO_NOEXCEPT;
    ANTON_CRT_IMPORT size_t fread(void* buffer, size_t size, size_t count, FILE* stream);
    ANTON_CRT_IMPORT size_t fwrite(void const* buffer, size_t size, size_t count, FILE* stream);
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
    ANTON_CRT_IMPORT int ferror(FILE* stream) ANTON_CRT_STDIO_NOEXCEPT;
    ANTON_CRT_IMPORT int feof(FILE* stream) ANTON_CRT_STDIO_NOEXCEPT;
    ANTON_CRT_IMPORT void clearerr(FILE* stream) ANTON_CRT_STDIO_NOEXCEPT;

    #undef ANTON_CRT_STDIO_NOEXCEPT
}

// C++ Runtime Forward Declarations

void* operator new(size_t size, void*) noexcept;
void operator delete(void* ptr, void* place) noexcept;

#undef ANTON_CRT_IMPORT
#undef size_t
