# intel-mpk

Intel MPK supports three access types:  read/write, read-only, or no access. pkey_alloc() allocates a protection key (pkey). pkey_mprotect() is a system call helps tag a page with pkey. In order to control the page with pkey value, the page table entry must be set as writable and readable. Otherwise, pkey doesnt work. After doing this, we can change pkey value using pkey_set()  instead of using system call. pkey_set() is not system call. When we try to reach a page, if the pkey is set for the page, the request is evaluate according to page table entry and pkey value, respectively.  

There are two definitions here:  PKEY_DISABLE_ACCESS 0x1, PKEY_DISABLE_WRITE 0x2.  

 

The question is that can I protect a page from execution with Intel-MPK? 

 For this experiment, an execution-enabled buffer is needed. After creation execution enabled buffer, I set the pkey with non-writable and non-readable. I tried executing the buffer. And It works. We cannot prevent the page table from execution with using Intel-MPK 

Followed the below steps:  

 

EXECUTION ENABLED BUFFER:  

Define char array opcode:  

char opcode[] = { 

    0xb8, 0x21, 0x00, 0x00, 0x00, //mov    $0x2a,%eax 

    0xc3                          //retq 

  }; 

Assign funcpointer to opcode  

Execute enable for the opcode´s page table with using mprotect(PROT_EXEC).  

Call the function pointer. WORK and return 0x21 

Without execution enable, SEGMENTATION FAULT 
