// Macro Guard
#ifndef SPTS_SCPI_H
#define SPTS_SCPI_H

// Files included
#include "GenericAlgorithms.h"
#include "StandardFiles.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============
   05/23/05, sjn,
   ==============
     Added BITDELIMMITTER to SCPI<IEEE488>.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=========
// IEEE488
//=========
struct IEEE488 {
	static std::string ClearErrors() 
		{ return("*CLS"); }
	static std::string ErrorBits()
	    { return("2,3,4,5"); } 
	static std::string Identify()
		{ return("*IDN?"); }
    static std::string IsDone()
		{ return("*ESR?"); }
    static std::string IsError()
        { return("*ESR?"); }
	static std::string OpCompleteBits()
		{ return("0"); } 
	static std::string Reset()
        { return("*RST;" + ClearErrors()); }
    static std::string SetOpsComplete()
        { return("*OPC"); }

protected:
    ~IEEE488() { /* */ }
};

//======
// SCPI
//======
template <typename T>
struct SCPI;  // undefined


//===============
// SCPI<IEEE488>
//===============
template<>
struct SCPI<IEEE488> : public IEEE488 {
    static const char BITDELIMMITTER = ',';
	static std::string Concatenate() 
		{ return(";:"); }

protected:
    ~SCPI() { /* */ }
};

#endif  // SPTS_SCPI_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
