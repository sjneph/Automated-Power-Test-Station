// Macro Guard
#ifndef SPTS_SigmaC4_H
#define SPTS_SigmaC4_H

// Files included
#include "GPIB.h"
#include "NumberBase.h"
#include "SigmaC4Language.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct SigmaC4 {
    typedef SPTSInstrument::GPIB BusType;
	typedef SigmaC4Language Language;
	enum { TotalRegisterBits = 8 };
	enum { BitReturnTypes = NumberBase::HEXIDECIMAL };
protected:
	~SigmaC4() { /* */ }
};

#endif  // SPTS_SigmaC4_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
