// Macro Guard
#ifndef SPTS_I2C_BUS_TYPE_H
#define SPTS_I2C_BUS_TYPE_H

// Files included
#include "StandardFiles.h"

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

// Forward Declaration
template <typename BusType>
class Instrument;


// The current implementation of this class is very specific to the Aardvark
//  USB-2-I2C adapter.  It is further limited to having only one possible slave
//  currently as it is very unlikely that multiple instruments in the station would
//  ever communicate via I2C.  If it does become necessary, this implementation will
//  need to be extended.  See GPIB.h/GPIB.cpp for an example.

class I2C {
    friend class Instrument<I2C>;

    I2C();
    ~I2C();
    bool isError();
	long maxAddress() const;
	std::string query(long address, const std::string& command = "",
                      double toWait = 0);
	void talk(long address, const std::string& command);
    std::string name() const;
    std::string whatError() const;


    int handle_;
    int portNumber_;
    char commandReg_;
    char queryReg_;
    bool noStop_;
    bool setQueryReg_;
    int numBytes_;
    int bytesRead_;
    int queryBytes_;
    std::string whatError_;
};

} // namespace SPTSInstrument

#endif // SPTS_I2C_BUS_TYPE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
