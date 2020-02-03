// Macro Guard
#ifndef SPTS_INSTRUMENT_BASE_H
#define SPTS_INSTRUMENT_BASE_H


// Files included
#include "GPIB.h"
#include "InstrumentTypes.h"
#include "StandardFiles.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============  
   03/03/05, sjn,
   ==============
     Added 3rd parameter to queryInstr() to pause if command parameter is non-empty.
      The pause should be after command is sent and before actual query.  Defaulted
      to zero.  Time is in seconds.  Should not be used for GPIB most modern bus types:
      just needed for I2C classes.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

template <typename BusType>
class Instrument {
public:
    enum Register { OPSCOMPLETE, ERROR };
    static std::string WhatBusError();
protected:
    bool commandInstr(long address, const std::string& command);
    long getAddress(InstrumentTypes::Types instrType);
    std::string queryInstr(long address, const std::string& query,
                           double pauseIfQueryNotEmpty = 0);
private:
    std::string name();

private:
	static BusType bus_;
    static std::string busError_;
};

} // namespace SPTSInstrument

#include "Instrument.template"  // Microsoft 7.0 workaround

#endif // SPTS_INSTRUMENT_BASE_H


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
