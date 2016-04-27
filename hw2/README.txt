README

Contains instructions to compile and execute 2 files:
 - vowels.cpp
 - lazyDentist.cpp


vowels.cpp:

	Counts the cumulative number of each vowel in 20 text files.
	
	Accepts the absolute file path for the directory containing the text files
	as command line input.

	Outputs the count for each vowel to the console.

	The given file path must end with a '/' character.

	The text files must be named file1.txt, file2.txt, ... file20.txt

	Can be compiled in BASH using the following command:

		g++ -std=c++11 vowels.cpp -o vowels -pthread

	Can be run using the following command:

		./vowels

	After the above command the program will prompt the user to enter the 
	directory containing the input text files.

lazyDentist.cpp:

	Implements the lazy dentist problem from homework 1.

	Prints messages to the console for each action taken by the dentist
	or patient threads.

	User can specify the number of patient threads as command line input.
	Number of threads must be between 1 and 5 (inclusive).

	Number of dentist threads is always 1.

	Program execution must be stopped using Ctrl-C or similar.
	Program will not stop otherwise.

	Can be compiled in BASH using the following command:

		g++ -std=c++11 lazyDentist.cpp -o lazy -pthread

	Can be run using the following command:

		./lazy

	Program will prompt the user to enter the desired number of patient threads.