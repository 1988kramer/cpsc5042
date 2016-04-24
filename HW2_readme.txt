Includes 2 .cpp files: vowels.cpp and lazyDentist.cpp


vowels.cpp:

Counts the number of each vowel in 20 text files
and prints the counts to the console.

Files must be named file1.txt, file2.txt... file20.txt.
Files must be in the same directory.

Absolute file path must be provided via command line.
File path must end with a "/" character.

vowels.cpp can be compiled with the following command:

   g++ -std=c++11 vowels.cpp -o vowels -pthread

Can be run using the following command:

   ./vowels


lazyDentist.cpp:

Implements the lazy dentist problem from homework 1 using semaphores.

Can be compliled using with the following command:

   g++ lazyDentist.cpp -o lazyDentist -pthread

Can be run using the following command:

   ./lazyDentist
