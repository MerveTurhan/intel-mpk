
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

char opcode[] = {
    0xb8, 0x2a, 0x00, 0x00, 0x00, //mov    $0x2a,%eax
    0xc3                          //retq
  };


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
int pkey, pkey_1;
long int *buffer;
int sum;
long int *shell_ptr;
long int *aligned_ptr;
f_ptr shellcodeFn;
unsigned long shellcodeAddr   = (unsigned long)opcode;
unsigned long pageAlignedAddr = shellcodeAddr & ~(PAGE_SIZE-1);


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

printf("you can read the buffer and buffer contains: %p\n", *buffer);


/*
* Execute opcode 
*/
shellcodeFn = (f_ptr)opcode;
status =  mprotect((void*)pageAlignedAddr,
        (shellcodeAddr - pageAlignedAddr) + sizeof(opcode),
        PROT_WRITE|PROT_READ|PROT_EXEC);

printf(" you can execute the buffer %d\n", shellcodeFn());


/*
* Allocate a protection key:
*/
pkey = pkey_alloc(0,PKEY_DISABLE_ACCESS | PKEY_DISABLE_WRITE);
// pkey_1 = pkey_alloc(0,PKEY_DISABLE_ACCESS);
if (pkey == -1 || pkey_1 == -1)
    errExit("pkey_alloc");

/*
* Disable access to any memory with "pkey" set,
* even though there is none right now
*/
pkey_set(pkey, PKEY_DISABLE_ACCESS | PKEY_DISABLE_WRITE); 
    if (status)
    errExit("pkey_set");

// pkey_set(pkey_1, PKEY_DISABLE_WRITE); 
//     if (status)
//     errExit("pkey_set");

/*
* Set the protection key on "buffer".
* Note that it is still read/write as far as mprotect() is
* concerned and the previous pkey_set() overrides it.
*/

// status = pkey_mprotect (buffer, getpagesize(),
//                        PROT_WRITE|PROT_READ,pkey);



// pkey_set(pkey, 0); 
//     if (status)
//     errExit("pkey_set");

// status = mprotect (buffer, getpagesize(),
//                         PROT_WRITE|PROT_READ);

if (status == -1)
    errExit("pkey_mprotect");

printf("STATUS PKEY PROTECT about to read buffer again...\n");

// printf("you can read the buffer and buffer contains: %p\n", *buffer);

status = pkey_get(pkey);

printf("status pkey %d\n", status);


status = pkey_get(pkey_1);

printf("status pkey_1%d\n", status);


// pkey = pkey_alloc(0,0);
// pkey_set(pkey, 0); 
// if (status== -1)
//     errExit("pkey_set");

// status = pkey_mprotect (buffer, getpagesize(),
//                       0,pkey);
// printf("After_pkeysetagainbuffer contains: %d\n", *buffer);



/*
* This will crash, because we have disallowed access
*/
printf("buffer contains: %d\n", *buffer);

status =  pkey_mprotect((void*)pageAlignedAddr,
        (shellcodeAddr - pageAlignedAddr) + sizeof(opcode),
        PROT_WRITE|PROT_READ,pkey);

printf(" you can execute the buffer %d\n", shellcodeFn());
printf(" %d\n",shellcodeFn());

status = pkey_free(pkey);

if (status == -1)
    errExit("pkey_free");

exit(EXIT_SUCCESS);

/*TEST-CASE 1  */


}

