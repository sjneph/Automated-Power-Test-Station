// Macro Guard
#ifndef SPTS_CONTROLMATRIX_SCPI
#define SPTS_CONTROLMATRIX_SCPI

// Files included
#include "InstrumentTags.h"
#include "SCPI.h"

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

template <>
struct SCPI<ControlMatrixTag> : public SCPI<IEEE488> { 
    // Public Interface
	static std::string Close(long relay)
        { return("SOUR:DIG:DATA:BIT " + convert<std::string>(relay) + ",0"); }
	static std::string Initialize() 
        { return(ClearErrors()); }
	static std::string Open(long relay)
        { return("SOUR:DIG:DATA:BIT " + convert<std::string>(relay) + ",1"); }			
	static std::string WhatError()
		{ return("SYST:ERR?"); }
	
protected:
	~SCPI<ControlMatrixTag>() { /* */ }  
};

#endif // SPTS_CONTROLMATRIX_SCPI

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
