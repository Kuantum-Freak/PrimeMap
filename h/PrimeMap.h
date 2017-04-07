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
 * @title Prime Map Header
 * @date 2017-02-11 (YYYY-MM-DD)
 * @fversion 1.2.0
 * @description Finds prime numbers and outputs a bitmap displaying the primes
 * =============================================================================
 */

#ifndef _PRIMEMAP_H_
#define _PRIMEMAP_H_

typedef unsigned long long ull;


bool testForPrime(const ull num);

void lookForPrimes(int id);

void displayPrimes();

std::string createBitMap();

void helpText();

void parse(int argc, char* argv[]);

int main(int argc, char* argv[]);

#endif