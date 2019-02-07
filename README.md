# 4001a4
Memory Management Algorithms

This code implements 4 different algorithms for physically allocating memory:
  1. First fit - allocates the first piece of memory starting from beginning that is big enough
  2. Best fit  - allocates the part of memory that is closest in size to the requested size
  3. Worst fit - allocates the part of memory that is furthest in size to the requested size
  4. Next fit  - allocates the first piece of memory starting from the last allocated memory that is big enough

To compile:
gcc -o memory mymem.c

To test:
./memory
OR
./memory [algorithm]
where [algorithm] is one of 'first', 'best', 'worst' or 'next'


TODO
1. Move prototypes to header file
2. Can we get rid of global variables (e.g. the one that keeps track of last allocated node? Make it static maybe?)
3. Separate linked list implementation from interface - this may be difficult because the nodes are constantly split and merged
4. Implement dynamic allocation based on user input
5. Are there other algorithms out there that can be implemented?
