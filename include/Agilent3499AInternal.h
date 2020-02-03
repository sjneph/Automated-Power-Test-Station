// Macro Guard
#ifndef SPTS_Agilent3499AInternal_H
#define SPTS_Agilent3499AInternal_H


// Files included
#include "Agilent3499AInternalRelays.h"
#include "GPIB.h"
#include "InstrumentTags.h"
#include "NumberBase.h"
#include "SCPI_SwitchMatrix.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct Agilent3499AInternal {
    typedef SPTSInstrument::Agilent3499AInternalRelays Relays;
    typedef SPTSInstrument::GPIB BusType;
	typedef SCPI<SwitchMatrixTag> Language;
	enum { TotalRegisterBits = 8 };
	enum { BitReturnTypes = NumberBase::DECIMAL };
	
protected:
	~Agilent3499AInternal() {}
};

#endif  // SPTS_Agilent3499AInternal_H


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
