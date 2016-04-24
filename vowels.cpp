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

int aCount;
int eCount;
int iCount;
int oCount;
int uCount;

pthread_mutex_t aCountMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t eCountMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t iCountMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t oCountMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t uCountMutex = PTHREAD_MUTEX_INITIALIZER;

string getDirectory()
{
	string directory;
	cout << "enter directory for file path: ";
	cin >> directory;
	return directory;
}

// accepts a string specifying the absolute file path for a text file
// updates the vowel count variables with the number of each vowel 
// found in the specified file 
void readFile(void* param)
{
	string *filePath = *static_cast<string*>(param); // cast param back to string
	const char* file = (*filePath).c_str(); 
	ifstream input(file); // open input file
	if (!input) // check if input file opened successfully
	{
		cerr << "input file " << filePath << " not found.";
		exit(1);
	}
	char current;
	while (input >> current) 
	{
		tolower(current); // ensure current character is lower case
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
// creates a thread to read each of the 20 files in the directory specified
// runs each thread to completion before exiting
// assumes files are named file1.txt, file2.txt... file20.txt
void createThreads(string directory)
{
	string filePaths[20];
	// create absolute file paths for each of the 20 input files
	for (int i = 0; i < 20; i++) 
	{
		string filePath;
		// assumes file path is in linux
		if (directory.back() != '/') directory += '/';
		filePath = directory + "file" + (i + 1) + ".txt";
		filePaths[i] = filePath;
		cout << filePath << endl; // for testing purposes
	}
	// create a thread for each file path
	pthread_t threads[20];
	for (int i = 0; i < 20; i++)
	{
		pthread_create(&threads[i], NULL, &readFile, (void*) filePaths[i]);
	}
	// join threads
	for (int i = 0, i < 20; i++)
		pthread_join(threads[i], NULL);
}

// prints the values of the vowel count variables
void printResults()
{
	cout << "Vowel counts for the given files are as follows:" << endl;
	cout << "A: " << aCount << endl;
	cout << "E: " << eCount << endl;
	cout << "I: " << iCount << endl;
	cout << "O: " << oCount << endl;
	cout << "U: " << uCount << endl;
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
