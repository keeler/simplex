#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iostream>
#include <cstdlib>
#include <string>

// Change to 0 to turn them off
#define ASSERTIONS_ON 1

/***************************************
* Assertion Macros
***************************************/
#if ASSERTIONS_ON
	// Asserts the condition, and terminates the program if false.
	#define fatalAssert( X, MSG ) if( !( X ) ) { std::cout << "Fatal assertion failed: " << #X << ", File: " << __FILE__ << ", Line: " << __LINE__ << std::endl << "\tMessage: " << MSG << std::endl; exit( -1 ); }
	// Asserts the condition, but does not terminate the program if false.
	#define softAssert( X, MSG ) if( !( X ) ) { std::cout << "Soft assertion failed: " << #X << ", File: " << __FILE__ << ", Line: " << __LINE__ << std::endl << "\tMessage: " << MSG << std::endl; }
	// A fatal assert that always occurs if executed.
	#define alwaysAssert( MSG ) { std::cout << "Always assert here: " << "File: " << __FILE__ << ", Line: " << __LINE__ << std::endl << "\tMessage: " << MSG << std::endl; exit( -1 ); }
#else
	#define fatalAssert( X, MSG ) {}
	#define softAssert( X, MSG ) {}
	#define alwaysAssert( MSG ) {}
#endif

#endif
