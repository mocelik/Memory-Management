Memory Management Algorithms

This code implements 4 different algorithms for physically allocating memory:
  1. First fit - allocates the first piece of memory starting from beginning that is big enough
  2. Best fit  - allocates the part of memory that is closest in size to the requested size
  3. Worst fit - allocates the part of memory that is furthest in size to the requested size
  4. Next fit  - allocates the first piece of memory starting from the last allocated memory that is big enough

To compile:
make

To test:
./memory [algorithm]
where [algorithm] is one of 'first', 'best', 'worst' or 'next'.
If unspecified, it will default to first.


TODO
1. Implement dynamic allocation based on user input
2. Are there other algorithms out there that can be implemented?
