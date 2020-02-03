// Macro Guard
#ifndef SPTS_GPIB_H
#define SPTS_GPIB_H

// Files included
#include "StandardFiles.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============  
   03/03/05, sjn,
   ==============
     Added 3rd parameter to query() to pause if command parameter is non-empty.  The
      pause will take place after command is sent and before actual query.  Defaulted
      to zero.  Time is in seconds.  Should not be used for GPIB: just needed for
      compatibility with Instrument<> and I2C classes.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

// Forward Declaration
template <typename BusType>
class Instrument;

class GPIB {
    friend class Instrument<GPIB>;

    GPIB();
    ~GPIB();
    bool isError();
	long maxAddress() const;
	std::string query(long address, const std::string& command = "",
                      double pauseAfterCommand = 0);
	void talk(long address, const std::string& command);
    std::string name() const;
    std::string whatError() const;

    typedef std::map<long, int> MapType;
    std::auto_ptr<MapType> map_;
};

} // namespace SPTSInstrument

#endif // SPTS_GPIB_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
