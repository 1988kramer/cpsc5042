// Andrew Kramer
// CPSC5042
// Homework 2
// 4/22/2016

// lazyDentist.cpp
// implements the lazy dentist problem from homework 1

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

struct threadArgs
{
	int num;
};

sem_t dentistReady;
sem_t seatCountWriteAccess; // if 1, the number of seats in the waiting room
							// can be incremented or decremented
sem_t patientReady; // the number of patients in the waiting room ready
					// to be served
int numFreeSeats; // number of free seats in the waiting room

void initialize()
{
	sem_init(&dentistReady, 0, 0);
	sem_init(&seatCountWriteAccess, 0, 1);
	sem_init(&patientReady, 0, 0);
	numFreeSeats = 3; // waiting room starts with 3 seats
}

void* dentist(void* unused)
{
	while (true)
	{
		cout << "Dentist trying to acquire patient..." << endl;
		sem_wait(&patientReady); // try to acquire patient
		cout << "Dentist trying to acquire seat count write access..." << endl;
		sem_wait(&seatCountWriteAccess); // try to get access to numFreeSeats
		numFreeSeats++; // one waiting room seat becomes free
		cout << "Incremented number of free seats to " 
				 << numFreeSeats << "." <<endl;
		cout << "Dentist ready to consult... " << endl;
		sem_post(&dentistReady); // dentist is available
		cout << "Dentist releasing seat count write access..." << endl;
		sem_post(&seatCountWriteAccess); // no longer need lock on chairs
		cout << "Dentist consulting with patient." << endl;
	}
}

void* patient(void* unused)
{
	//threadArgs* number = (threadArgs*) args;
	//int patientNum = number->num;
	pthread_t patientNum = pthread_self();
	while (true)
	{
		cout << "Patient " << patientNum << " trying to acquire seat count "
				 << "write access..." << endl;
		sem_wait(&seatCountWriteAccess); // try to get access to numFreeSeats
		if (numFreeSeats > 0)
		{
			numFreeSeats--; // patient takes seat in waiting room
			cout << "Patient " << patientNum << " seated; remaining seats = "
					 << numFreeSeats << "." << endl;
			cout << "Patient " << patientNum 
					 << " notifying dentist patientReady..." << endl;
			sem_post(&patientReady); // notify the dentist a patient is ready
			cout << "Patient " << patientNum 
					 << " releasing seat count write access." << endl;
			sem_post(&seatCountWriteAccess); // no longer need lock on numFreeSeats
			cout << "Patient " << patientNum << " waiting for dentist..." << endl;
			sem_wait(&dentistReady); // wait until dentist is ready
			cout << "Patient " << patientNum 
					 << " is consulting with dentist" << endl;
		}
		else
		{
			cout << "Patient " << patientNum << " leaving without consulting,"
					 << " no chairs available." << endl;
			sem_post(&seatCountWriteAccess); // no longer need lock on seats
		}

	}
}

// takes an int, the number of patients as a parameter
// creates the specified number of patient threads
void createPatients(int numPatients)
{
	pthread_t *patients;
	patients = new pthread_t[numPatients];
	for (int i = 0; i < numPatients; i++)
		pthread_create(&patients[i], NULL, &patient, NULL);
}

// prompts the user for the desired number of patient threads
// accepts a reference to an int as a parameter
// stores the user input in the given reference
void getNumPatients(int &numPatients) {
	cout << "Enter the number of patients: ";
	cin >> numPatients;
	cout << endl;
	while (numPatients < 1 || numPatients > 5)
	{
		cout << "The number of patients must be between 1 and 5." << endl;
		cout << "Enter the number of patients: ";
		cin >> numPatients;
		cout << endl;
	}
}

int main() 
{
	initialize();
	pthread_t dentist1;
	int numPatients;
	getNumPatients(numPatients);
	pthread_create(&dentist1, NULL, &dentist, NULL);
	createPatients(numPatients);
	pthread_join(dentist1, NULL); // prevents the main thread from exiting and
								  // cancelling the dentist and patient threads
	return 0;
}