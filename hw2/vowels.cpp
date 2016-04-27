// Andrew Kramer
// CPSC5042
// Homework 2
// 4/21/2016

// vowels.cpp
// prints the counts of each vowel in 20 different text files
// accepts the file directory as command line input
// assumes files will be named file1.txt, file2.txt, ... file20.txt

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <pthread.h>
using namespace std;

struct threadParam
{
	string directory;
};

int aCount;
int eCount;
int iCount;
int oCount;
int uCount;

const int numThreads = 20;

pthread_mutex_t aCountMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t eCountMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t iCountMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t oCountMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t uCountMutex = PTHREAD_MUTEX_INITIALIZER;

// prompts the user for the directory containing the 20 input files
// returns a string specifying the absolute filepath for the directory
string getDirectory()
{
	string directory;
	cout << endl << "enter directory for file path: ";
	cin >> directory;
	cout << endl;
	return directory;
}

// accepts a string specifying the absolute file path for a text file
// updates the vowel count variables with the number of each vowel 
// found in the specified file 
void* readFile(void* param)
{
	threadParam *filePath = (threadParam*)param; // cast param back to threadParam
	const char* file = filePath->directory.c_str(); 
	ifstream input(file); // open input file
	if (!input) // check if input file opened successfully
	{
		cerr << "input file " << filePath << " not found.";
		exit(1);
	}
	char current;
	while (input >> current) 
	{
		current = tolower(current); // ensure current character is lower case
		switch (current) // update counts if current character is a vowel
		{
			case 'a':
				pthread_mutex_lock(&aCountMutex);
				aCount++;
				pthread_mutex_unlock(&aCountMutex);
				break;
			case 'e':
				pthread_mutex_lock(&eCountMutex);
				eCount++;
				pthread_mutex_unlock(&eCountMutex);
				break;
			case 'i':
				pthread_mutex_lock(&iCountMutex);
				iCount++;
				pthread_mutex_unlock(&iCountMutex);
				break;
			case 'o':
				pthread_mutex_lock(&oCountMutex);
				oCount++;
				pthread_mutex_unlock(&oCountMutex);
				break;
			case 'u':
				pthread_mutex_lock(&uCountMutex);
				uCount++;
				pthread_mutex_unlock(&uCountMutex);
				break;
		}
	}
}

// accepts a string specifying the directory of the 20 files to be read
// string must specify the absolute filepath and end in a '/' character
// creates a thread to read each of the 20 files in the directory specified
// runs each thread to completion before exiting
// assumes files are named file1.txt, file2.txt... file20.txt
void createThreads(string directory)
{
	threadParam filePaths[numThreads];
	// create absolute file paths for each of the 20 input files
	for (int i = 0; i < numThreads; i++) 
	{
		string filePath;
		filePath = directory + "file" + to_string(i + 1) + ".txt";
		filePaths[i].directory = filePath;
	}
	// create a thread for each file path
	pthread_t threads[numThreads];
	for (int i = 0; i < numThreads; i++)
	{
		int result = pthread_create(&threads[i], NULL, &readFile, (void*) &filePaths[i]);
		if (result != 0)
		{
			cout << "error creating thread " << filePaths[i].directory << endl;
		}
	}
	// join threads
	for (int i = 0; i < numThreads; i++)
	{
		pthread_join(threads[i], NULL);
	}
}

// prints the values of the vowel count variables
void printResults()
{
	cout << "Vowel counts for the given files are as follows:" << endl;
	cout << "A: " << aCount << endl;
	cout << "E: " << eCount << endl;
	cout << "I: " << iCount << endl;
	cout << "O: " << oCount << endl;
	cout << "U: " << uCount << endl << endl;
}

int main() 
{
	aCount = 0;
	eCount = 0;
	iCount = 0;
	oCount = 0;
	uCount = 0;
	string directory = getDirectory();
	createThreads(directory);
	printResults();
	return 0;
}
