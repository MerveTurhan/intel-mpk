
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <example.h>
#include <string.h>






static inline void  wrpkru(unsigned int pkru)
{
    unsigned int eax = pkru;
    unsigned int ecx = 0;
    unsigned int edx = 0;

    asm volatile(".byte 0x0f,0x01,0xef\n\t"
                : : "a" (eax), "c" (ecx), "d" (edx));
}

  typedef int(*FUNKY_POINTER)(void);

  char shellcode[] = {
    0xb8, 0x2a, 0x00, 0x00, 0x00, //mov    $0x2a,%eax
    0xc3                          //retq
  };


    #define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
                                } while (0)
    void _pkey_set(int pkey, unsigned long rights, unsigned long flags)
       {
           unsigned int pkru = (rights << (2 * pkey));
           return wrpkru(pkru);
       }
     int _pkey_alloc(void)
       {
           return syscall(SYS_pkey_alloc, 0, 0);
       }
    
    int main(void)
    {
        int status;
        int pkey;
        long int *buffer;
        int sum;
        long int *shell_ptr;
        long int *aligned_ptr;
        void (*fun_ptr)(int) = &addition;
        

        unsigned long pageSize        = 4096;
        unsigned long shellcodeAddr   = (unsigned long)shellcode;
        unsigned long pageAlignedAddr = shellcodeAddr & ~(pageSize-1);
    
        FUNKY_POINTER shellcodeFn = (FUNKY_POINTER)shellcode;
        /*
        *Allocate one page of memory
        */

        
        buffer = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);



        if (buffer == MAP_FAILED)
            errExit("mmap");

        
        /*
        * Put some random data into the page (still OK to touch)
        */


        *buffer = __LINE__;
        buffer[0]= 0xb8;
        buffer[1]= 0x2a;
        buffer[2]= 0x00;
        buffer[3]= 0x00;
        buffer[4]= 0x00;
        buffer[5]= 0xc3;

        printf("buffer adress: %p\n", buffer);
        printf("buffer contains: %s\n", buffer[0]);

        aligned_ptr = PAGE_ALIGN((long int)buffer);
        
        printf("adress: %p\n",aligned_ptr);
        printf("adress: %p\n",buffer - aligned_ptr);
        printf("shell_adress: %p\n",shell_ptr); 
        status = mprotect((void*)aligned_ptr,
             ((long int *)buffer - aligned_ptr) + sizeof(shellcode),
             PROT_EXEC|PROT_WRITE|PROT_READ);

        if (status == -1)
            errExit("mprotect");
        printf("mprotect: %d\n");
        FUNKY_POINTER shellcodeFn = (FUNKY_POINTER)buffer;
 
        shellcodeFn();
        
        printf("adress: %p\n",fun_ptr);
        printf("adress: %p\n",(long int)fun_ptr);

        printf("adress: %p\n",PAGE_ALIGN((long int)fun_ptr));
        /*
        * Allocate a protection key:
        */
        pkey = _pkey_alloc();
        if (pkey == -1)
            errExit("pkey_alloc");

        /*
        * Disable access to any memory with "pkey" set,
        * even though there is none right now
        */
        _pkey_set(pkey, PKEY_DISABLE_ACCESS, 0);
                    /*
            if (status)
            errExit("pkey_set");
            */

        /*
        * Set the protection key on "buffer".
        * Note that it is still read/write as far as mprotect() is
        * concerned and the previous pkey_set() overrides it.
        */

        status = pkey_mprotect (buffer, getpagesize(),
                                PROT_READ,pkey);
        if (status == -1)
            errExit("pkey_mprotect");

        status =  pkey_mprotect((void*)pageAlignedAddr,
             (shellcodeAddr - pageAlignedAddr) + sizeof(shellcode),
             PROT_EXEC|PROT_WRITE|PROT_READ,pkey);

        printf("The answer to the ultimate question of life, "
           "the universe and everything is %d\n",
           shellcodeFn());

        printf("about to read buffer again...\n");

        /*
        * This will crash, because we have disallowed access
        */
        printf("buffer contains: %d\n", *buffer);

        status = pkey_free(pkey);
    
        if (status == -1)
            errExit("pkey_free");
        printf("about to read buffer again...\n");

        exit(EXIT_SUCCESS);
    }

