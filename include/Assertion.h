// Macro Guard
#ifndef SPTS_ASSERTIONS_H
#define SPTS_ASSERTIONS_H

// Files included
#include "StandardFiles.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

template <typename ExceptionType>
struct Assert {
	explicit Assert(bool toAssert) {
		if ( !toAssert ) 
			throw(ExceptionType());
	} // Assert(Overload1)

	Assert(bool toAssert, const std::string& info) {
		if ( !toAssert ) 
			throw(ExceptionType(info));
	} // Assert(Overload2)

	Assert(bool toAssert, const std::string& info1, const std::string& info2) {
		if ( !toAssert )
			throw(ExceptionType(info1, info2));
	} // Assert(Overload3)
};

#endif // SPTS_ASSERTIONS_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
