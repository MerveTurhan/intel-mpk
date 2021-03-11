#define _GNU_SOURCE
#define  PAGE_SIZE   4096UL
#define  PAGE_ALIGN(addr)   ((addr) & -PAGE_SIZE)

#define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                        } while (0)



typedef int(*f_ptr)(void);

void addition();
