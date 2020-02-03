// Macro Guard
#ifndef SPTS_TekTCPA300_H
#define SPTS_TekTCPA300_H

// Files included
#include "I2C.h"
#include "TekTCPA300Language.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct TekTCPA300 {
	typedef TekTCPA300Language Language;
    typedef SPTSInstrument::I2C BusType;
    enum { REQUIRESSPECIALTIMER = true };
    enum { TIMEINSECONDSAFTERDEGAUSS = 10 };
    static const double TIMEINSECONDSAFTERLASTCOMMAND;
    /*
       Pause times are big, but SPTS class is designed to talk to the instrument
       only when necessary --> enough time should go by with no explicit pausing
       the majority of the time
    */
protected:
	~TekTCPA300() { /* */ }
};

#endif  // SPTS_TekTCPA300_H


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
