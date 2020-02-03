// Macro Guard
#ifndef SPTS_AgilentN3300A_H
#define SPTS_AgilentN3300A_H

// Files included
#include "AgilentN3300ALanguage.h"
#include "GPIB.h"
#include "InstrumentTags.h"
#include "NumberBase.h"
#include "OScopeParameters.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   11/02/04, sjn,
   ==============
   Changed Language from SCPI_ElectronicLoad to AgilentN3300ALanguage.  The latter
   inherits 99% of its interface from the previous.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct AgilentN3300A {
	typedef AgilentN3300ALanguage Language;
    typedef SPTSInstrument::GPIB BusType;
	enum { TotalRegisterBits = 8 };
	enum { BitReturnTypes = NumberBase::DECIMAL };
    static const OScopeParameters::SlopeType LoadTriggerSlope = /* falling edge */
                                                           OScopeParameters::NEGATIVE;
protected:
	~AgilentN3300A() { /* */ }
};


#endif  // SPTS_AgilentN3300A_H


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
