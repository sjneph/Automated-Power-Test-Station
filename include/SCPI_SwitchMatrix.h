// Macro Guard
#ifndef SPTS_SWITCHMATRIX_SCPI
#define SPTS_SWITCHMATRIX_SCPI

// Files included
#include "InstrumentTags.h"
#include "SCPI.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

template <>
struct SCPI<SwitchMatrixTag> : public SCPI<IEEE488> { 
    static std::string Close(long relay) 
        { return("CLOS (@" + convert<std::string>(relay) + ")"); }
	static std::string Initialize() 
        { return(ClearErrors()); }
    static std::string Open(long relay) 
        { return("OPEN (@" + convert<std::string>(relay) + ")"); }   
    static std::string WhatError()
		{ return("SYST:ERR?"); }
protected:
	~SCPI<SwitchMatrixTag>() { /* */ }  
};


#endif // SPTS_SWITCHMATRIX_SCPI

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
