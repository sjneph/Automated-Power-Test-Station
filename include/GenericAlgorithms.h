// Macro Guard
#ifndef GenericAlgorithms_H
#define GenericAlgorithms_H

// Files included
#include "StringAlgorithms.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//==========
// Int2Type
//==========
template <int Val>
struct Int2Type {
	enum { val = Val };
};

//===========
// Type2Type
//===========
template <typename T>
struct Type2Type {
    typedef T OriginalType;
};

//==============
// convert<,>()
//==============
template <typename RtnType, typename From>
extern RtnType convert(From from);


#include "GenericAlgorithms.template"  // Microsoft 7.0 workaround

#endif  // GenericAlgorithms_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
