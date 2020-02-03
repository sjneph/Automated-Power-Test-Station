// Macro Guard
#ifndef SPTS_HP6624A_H
#define SPTS_HP6624A_H

// Files included
#include "GPIB.h"
#include "HP6624ALanguage.h"
#include "NumberBase.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct HP6624A {
	typedef HP6624ALanguage Language;
    typedef SPTSInstrument::GPIB BusType;
	enum { TOTALREGISTERBITS = 8 };
	enum { BITRETURNTYPES = NumberBase::DECIMAL };
    enum { DualRanges = true };
protected:
	~HP6624A() { /* */ }
};

#endif  // SPTS_HP6624A_H


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
