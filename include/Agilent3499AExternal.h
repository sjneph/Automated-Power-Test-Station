// Macro Guard
#ifndef SPTS_Agilent3499AExternal_H
#define SPTS_Agilent3499AExternal_H


// Files included
#include "Agilent3499AExternalRelays.h"
#include "GPIB.h"
#include "InstrumentTags.h"
#include "NumberBase.h"
#include "SCPI_ControlMatrix.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct Agilent3499AExternal {
    typedef SPTSInstrument::Agilent3499AExternalRelays Relays;
    typedef SPTSInstrument::GPIB BusType;
	typedef SCPI<ControlMatrixTag> Language;
	enum { TotalRegisterBits = 8 };
	enum { BitReturnTypes = NumberBase::DECIMAL };

protected:
	~Agilent3499AExternal() {}
};


#endif  // SPTS_Agilent3499AExternal_H


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
