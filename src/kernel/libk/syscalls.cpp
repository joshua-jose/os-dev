#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <stdio.h>

#include "kernel/kapi.hpp"
#include "kernel/devices/keyboard.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

extern "C"{

char *__env[1] = { 0 };
char **environ = __env; // pointer to array of char * strings that define the current environment variables 


void _exit (int __status){
    asm volatile("cli");
    asm volatile("hlt");
};

int close(int file) {
    return -1;
}

int execve(char *name, char **argv, char **env){
    return -1;
};
int fork(){
    return -1;
};
int fstat(int file, struct stat *st){
    st->st_mode = S_IFCHR;
    return 0;
};
int getpid(){
    return 1;
};
int isatty(int file){
    return 1;
};
int kill(int pid, int sig){
    return -1;
};
int link(char *old, char *_new){
    return -1;
};
int lseek(int file, int ptr, int dir){
    return 0;
};
int open(const char *name, int flags, ...){
    return -1;
};
int read(int file, char* buf, int len){
    if (file != STDIN_FILENO)
        return -1;

    int read = 0;
    for (; len>0; len--){
        char c =  keyboard_buffer_getc();
        *buf++ = c;
        read++;
    }

    return read;
};

caddr_t sbrk(int incr) {
    extern uintptr_t _end;		/* Defined by the linker */
    static uintptr_t* heap_end = NULL;
    uintptr_t* prev_heap_end;
    
    if (heap_end == NULL) {
        heap_end = &_end;
    }
    prev_heap_end = heap_end;
    /*
    if (heap_end + incr > stack_ptr) {
        write (1, "Heap and stack collision\n", 25);
        abort ();
    }
    */
    heap_end += incr;
    return (caddr_t) prev_heap_end;
}

int stat(const char *file, struct stat *st){
    st->st_mode = S_IFCHR;
    return 0;
};
clock_t times(struct tms *buf){
    return -1;
};
int unlink(char *name){
    return -1; 
};
int wait(int *status){
    return -1;
};
int write(int file, char *ptr, int len){
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO))
        return -1;
    
    for (int todo = 0; todo < len; todo++) {
        fputc(*ptr++);
    }
    return len;
};
//int gettimeofday(struct timeval *p, struct timezone *z);

}

#pragma GCC diagnostic pop
