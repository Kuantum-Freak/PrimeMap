/* 
 * Copyright (c) The Gupta Empire - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 *
 * Written by Aryan Gupta <me@theguptaempire.net>, February 2017
 * 
 * =============================================================================
 * @author Aryan Gupta
 * @project Prime Map
 * @title Prime Map
 * @date 2017-02-11 (YYYY-MM-DD)
 * @fversion 3.5.1
 * @description Finds prime numbers and outputs a bitmap displaying the primes
 * =============================================================================
 */
#include "info.h"
#define _PRINTINFO std::cout << '\n' << R_PROJNAME << " v" << R_PVERSION << " by " << R_AUTHOR << '\n' << R_COPYRIGHT << '\n' << R_COMMENTS << "\n\n\n" // Legal and Informational

#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <sstream>
#include <string.h>
#include <string>

#if (defined (LINUX) || defined (__linux__)) // if we are compiling on a linux kernel
	#include <thread>
	#include <mutex>
#elif (defined (_WIN32) || defined (_WIN64)) // if we are compiling on a windows/NT kernel
	#include ".\h\mingw.thread.h"
	#include ".\h\mingw.mutex.h"
#endif

#include ".\h\PrimeMap.h"

typedef unsigned long long ull;

int NUM_OF_THREADS = 4; // stores the number of threads to be used to calculate the primes

const long START = 2; // start of primes
ull END = 10000; // where to end when looking for primes

bool outputBitMap = false; // should we output a bitmap?
bool display = false; // should we display the bitmap?
bool RGBColor = false; // does the user want to change the color of the bit map?

std::mutex testing; // mutex used when pulling a tested number from the number pool
std::mutex found; // mutex used when a prime is found and saving it to the found primes list

std::vector<bool> foundPrimes(END, false); // stores all the foud primes | 0 for nit prime, 1 for prime
std::vector<ull> testedPrimes(1, START - 1); // stores the numbers that have been tested
ull numOfPrimes = 0; // stores the number of primes found

long primeColor[3] = {0, 0, 0}; // stores the color of the primes on the bitmap
long nonPColor[3] = {255, 255, 255}; // stores the background color of the bitmap
int colorCode = 0; // stores what color code we are using | 0 for black and white (BW), 1 for green and black (GB), and 2 for other color (OC)

/* 
	@summary 	Tests if a number is prime or not
	
	@param 		The number to test
	@return 	Returns if the number is a prime or not
 */
bool testForPrime(const ull num) {
	for(ull i = 2; i <= num / 2; ++i) // goes through all numbers less than half of the number to test
		if(num % i == 0) // tests to see if its divisible
			return false; // number is divisible, number is not a prime
	return true; // number is not divisible by any number, it is a prime
}

/* 
	@summary 	Looks for primes. This is used as a multithreaded
				function. Pulls a last number tested and adds 1, then puts
				it back in to the array. This way the next threads knows
				that that number has been tested (or being tested). Then the
				function takes the number and tests if it is a prime or not.
				If the number is a prime, it sets its corisponding foundPrimes
				variable to true and increments numOfPrimes.
				
	@param		The thread ID (this ID is local and not the ID that the Operating system assigns)
	@return		VOID
 */
void lookForPrimes(int id) {
	while(true) { // continue to run until we have run through all the numbers
		testing.lock(); 									// lock the mutex to pull number
		ull num = testedPrimes.back() + 1;					// get number then add one
		testedPrimes.push_back(num);						// take new number and put it back so the next thread can get a new number
		testing.unlock();									// unlock mutex
		
		if(num > END) // tests to see if we have run through all the numbers
			break;
		
		if(testForPrime(num)) { // tests if number is a prime if it is...
			found.lock(); 				// lock mutex to update number is prime
			foundPrimes[num] = true;	// update at the index that it is a prime
			numOfPrimes++;				// update number of found primes
			found.unlock();				// unlock mutex
		}
	}
}

/* 
	@summary	Displays all the numbers from START to END and displays if it is a prime or not
	
	@param		VOID
	@return		VOID
 */
void displayPrimes() {
	using std::cout;
	using std::endl;
	for(unsigned i = 0; i < foundPrimes.size(); ++i) {
		cout << i << ": " << foundPrimes[i] << endl;
	}
}

/* 
	@summary	Creates a bit map with the prime data
	
	@param		VOID
	@return		A string variable holding the location of the outputted bitmap
 */
std::string createBitMap() {
	FILE * f;
	unsigned char file[14] = { // BITMAPFILEHEADER
		'B','M',		// magic				- bfType
		0,0,0,0,		// size in bytes 		- bfSize
		0,0,			// app data				- bfReserved1
		0,0,			// app data				- bfReserved2
		40+14,0,0,0		// start of data offset - bfOffBits
	};
	unsigned char info[40] = { // BITMAPINFOHEADER
		40,0,0,0,		// info hd size			- biSize
		0,0,0,0,		// width				- biWidth
		0,0,0,0,		// heigth				- biHeight
		1,0,			// number color planes	- biPlanes
		24,0,			// bits per pixel		- biBitCount
		0,0,0,0,		// compression is none	- biCompression
		0,0,0,0,		// image bits size		- biSizeImage
		0x13,0x0B,0,0, 	// horz resoluition		- biXPelsPerMeter (in pixel / m)
		0x13,0x0B,0,0, 	// vert resolutions		- biYPelsPerMeter (0x03C3 = 96 dpi, 0x0B13 = 72 dpi)
		0,0,0,0, 		// #colors in pallete	- biClrUsed
		0,0,0,0, 		// #important colors	- biClrImportant
	};

	int w = sqrt(END); // get width
	int h = sqrt(END); // get height

	int padSize  = (4 - (w * 3) % 4) % 4; // calculate pad size
	int sizeData = w * h * 3 + h * padSize;
	int sizeAll  = sizeData + sizeof(file) + sizeof(info);
	
	// Set the size up (bfSize)
	file[ 2] = (unsigned char)(sizeAll      );
	file[ 3] = (unsigned char)(sizeAll >>  8);
	file[ 4] = (unsigned char)(sizeAll >> 16);
	file[ 5] = (unsigned char)(sizeAll >> 24);
	
	// Set the width up (biWidth)
	info[ 4] = (unsigned char)(w      );
	info[ 5] = (unsigned char)(w >>  8);
	info[ 6] = (unsigned char)(w >> 16);
	info[ 7] = (unsigned char)(w >> 24);
	
	// Set the height up (biHeight)
	info[ 8] = (unsigned char)(h      );
	info[ 9] = (unsigned char)(h >>  8);
	info[10] = (unsigned char)(h >> 16);
	info[11] = (unsigned char)(h >> 24);
	
	// Set the size of the file up (biSizeImage)
	info[20] = (unsigned char)(sizeData      );
	info[21] = (unsigned char)(sizeData >>  8);
	info[22] = (unsigned char)(sizeData >> 16);
	info[23] = (unsigned char)(sizeData >> 24);
	
	std::string name;
	std::stringstream ss;
	ss << ".\\bmp\\" << w << " x " << w << ((colorCode != 0)? (colorCode == 1)? " GB":" OC":" BW") <<".bmp"; // name of the file
	name = ss.str();
	
	ss.str(std::string()); // reset string stream
	ss.clear();
	
	f = fopen(name.c_str(), "wb"); // open file
	fwrite(file, 1, 14, f); // write file header
	fwrite(info, 1, 40, f); // wite info header
	
	unsigned char pad[3] = {0,0,0};

	for (int y = h - 1; y >= 0; y--) {
		for (int x = 0; x < w; x++) {
			long red = (foundPrimes[w * y + x])? primeColor[0]:nonPColor[0];   // set red hue
			long green = (foundPrimes[w * y + x])? primeColor[1]:nonPColor[1]; // set green hue
			long blue = (foundPrimes[w * y + x])? primeColor[2]:nonPColor[2];  // set blue hue
			
			if (red < 0) red = 0;			// make sure that ll the values are withing acceptable limits
			if (red > 255) red = 255;
			
			if (green < 0) green = 0;
			if (green > 255) green = 255;
			
			if (blue < 0) blue = 0;
			if (blue > 255) blue = 255;
			
			unsigned char pixel[3];	// create pixel
			pixel[0] = blue;
			pixel[1] = green;
			pixel[2] = red;

			fwrite((char*)pixel, 1, 3, f); // write pixel
		}
		fwrite((char*)pad,1, padSize,f ); // write padding
	}
	
	fclose(f);
	
	ss << "\"" << name << "\""; // fet full file name in quotes
	std::string fullFile = ss.str();
	return fullFile; // return file name in quotes
}

/* 
	@summary	Displays the help text
	
	@param 		VOID
	@return 	VOID
 */
void helpText() {
	using std::cout;
	using std::endl;
	cout << R_DESCRIPTION << endl << endl;
	cout << "HELP:\n" << "Argument\tDefault\t\tDetails" << endl;
	cout << "-t \t\t"		<< 4 			<< "\t\tUse this argument to specify number of threads\n"
		 << "-m \t\t"		<< 10000        << "\t\tUse this argument to specify the maximum number\n"
		 << "-o \t\t"		<< "False"		<< "\t\tUse this switch to specify whether to output a bitmap\n"
		 << "-d \t\t"		<< "False"		<< "\t\tUse this switch to specify whether to display the bitmap\n"
		 << "-gbc \t\t"	    << "None"    	<< "\t\tUse this switch to use green on black color scheme\n"
		 << "-p \t\t"	    << "000000" 	<< "\t\tUse this argument to specify prime color on the bitmap (HEX format)\n"
		 << "-b \t\t"		<< "FFFFFF"		<< "\tUse this argument to specify background color on the bitmap (HEX format)\n"
		 << endl;
	cout << "Usage:   .\\PrimeMap.exe [-t <threads>] [-e <end>] [-o] [-d] [[-gbc] | [-p <HEX CODE>] [-b <HEX CODE>]]" << endl;
	cout << "Example: .\\PrimeMap.exe -t 7 -e 500000 -o -d -gbc" << endl;
	cout << "Example: .\\PrimeMap.exe -t 7 -e 500000 -o -p 010101 -d 245 12 155" << endl;
	
	cout << endl << endl;
		 
	exit(1);
}

/* 
	@summary	Parses the command line arguments
	
	@param		An int containing the amount of arguments
				supplied, and a char array holding the actuall
				command line arguments
	@return		VOID
 */
void parse(int argc, char* argv[]) {
	using std::string;
	for(int i = 1; i < argc; ++i) {
		if       (strcmp(argv[i],"-t") == 0) { // number of threads to use
			NUM_OF_THREADS = atoi(argv[i + 1]);
			i++;
		} else if(strcmp(argv[i], "-m") == 0) { // number of primes to find up to
			END = atoll(argv[i + 1]);
			i++;
		} else if(strcmp(argv[i], "-p") == 0) { // prime color RGB
			primeColor[0] = strtol(string(argv[i + 1]).substr(0,2).c_str(), NULL, 16); 	// first convert c_string to string to take
			primeColor[1] = strtol(string(argv[i + 1]).substr(2,2).c_str(), NULL, 16);	// substring, then convert it back to a 
			primeColor[2] = strtol(string(argv[i + 1]).substr(4,2).c_str(), NULL, 16);	// c_string to convert from base 16 to base 10
			colorCode = 2;
			i++;
		} else if(strcmp(argv[i], "-b") == 0) { // background color RGB
			nonPColor[0] = strtol(string(argv[i + 1]).substr(0,2).c_str(), NULL, 16);
			nonPColor[1] = strtol(string(argv[i + 1]).substr(2,2).c_str(), NULL, 16);
			nonPColor[2] = strtol(string(argv[i + 1]).substr(4,2).c_str(), NULL, 16);
			colorCode = 2;
			i++;
		} else if(strcmp(argv[i], "-gbc") == 0) { // shortcut to Green on Black
			primeColor[0] = 70;
			primeColor[1] = 192;
			primeColor[2] = 22;
			
			nonPColor[0] = 1;
			nonPColor[1] = 1;
			nonPColor[2] = 1;
			colorCode = 1;
		} else if(strcmp(argv[i], "-o") == 0) { // output a bitmap
			outputBitMap = true;
		} else if(strcmp(argv[i], "-d") == 0) { // display/open the bitmap
			display = true;
		} else if(strcmp(argv[i], "/?") == 0 || strcmp(argv[i], "-?") == 0 ) // display the help text
			helpText();
		else // somthing wrong happened, display help text
			helpText();
	}
}

/* 
	@summary	MAIN
	
	@param		Command line arguments
	@return		exit code
 */
int main(int argc, char* argv[]) {
	_PRINTINFO;
	
	using namespace std::chrono; // used for timing run
	using namespace std;
	
	parse(argc, argv); // get parse info
	
	vector<thread> threads;
	
	threads.reserve(NUM_OF_THREADS);
	foundPrimes.reserve(END + 100); // reserve vector memory for faster storage
	
	auto startTime = steady_clock::now(); // start clock to time the run
	for(int i = 0; i < NUM_OF_THREADS; ++i) // run each thread
		threads.push_back(thread(lookForPrimes, i));
	
	for(auto& t: threads) t.join(); // wait for the other threads to finish
	
	auto endTime = steady_clock::now(); // stop the timer
	auto diff = endTime - startTime; // get time
	auto nseconds = double(diff.count()) * steady_clock::period::num / steady_clock::period::den; // convert to seconds
	
	cout << numOfPrimes << " primes found between " << START << " and "
		 << END << " using " << NUM_OF_THREADS << " cores in " << nseconds
		 << " seconds." << endl; 									// display run dignostics
	
	if(outputBitMap) { // run this if we want to output a bitmap
		string file = createBitMap(); // create bitmap and get the file URL
		cout << "\nCreated Bitmap: " << file << endl << endl;
		if(display) // run if we want to open the file
			system(file.c_str()); // display the file
	}
	
	cout << "Press enter to exit...";
	std::cin.get();
	
	return 0; // SUCCESS
}