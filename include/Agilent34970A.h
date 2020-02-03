// Macro Guard
#ifndef SPTS_Agilent34970A_H
#define SPTS_Agilent34970A_H

// Files included
#include "GPIB.h"
#include "InstrumentTags.h"
#include "NumberBase.h"
#include "SCPI_DMM.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct Agilent34970A {
	typedef SCPI<DMMWithMultiplexerTag> Language;
    typedef SPTSInstrument::GPIB BusType;
	enum { TOTALREGISTERBITS = 8 };
	enum { BITRETURNTYPES = NumberBase::DECIMAL };
    enum { AUTORANGE = -1 };
    enum { 
           MULTIPLEXERCARDEXISTS   = true, 
           DCVOLTAGERELAYCHANNEL   = 101,
           OHMSRELAYCHANNEL        = DCVOLTAGERELAYCHANNEL,
           TEMPERATURERELAYCHANNEL = 102 
         };
protected:
	~Agilent34970A() { /* */ }
};


#endif  // SPTS_Agilent34970A_H


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
