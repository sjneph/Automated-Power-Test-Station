// Files included for NI-specific GPIB control
#include <windows.h>
#include "ni488.h"

// Files included
#include "Assertion.h"
#include "Functions.h"
#include "GenericAlgorithms.h"
#include "GPIB.h"
#include "SPTSException.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============  
   03/03/05, sjn,
   ==============
     Modified talk(): changed arguments to internal call to ibwrt() from a static_cast<>
       to void* from a const_cast<> on a std::string's .c_str() member function all to
       static_cast<> to void* from a char array.  This is a safer idea to keep away from
       const_cast<>.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    typedef StationExceptionTypes::BusError        BusError;
    typedef StationExceptionTypes::UnexpectedState UnexpectedState;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

//=============
// Constructor
//=============
GPIB::GPIB() : map_(new MapType)
{ /* */ }

//============
// Destructor
//============
GPIB::~GPIB() { 
    MapType::iterator beg = map_->begin(), end = map_->end();
    while ( beg != end ) {
        ibonl(beg->second, 0);   /* Take the device offline */	        
        ++beg;
    }    
}

//===========
// isError()
//===========
bool GPIB::isError() {
    return(0 != (ibsta & ERR));
}

//==============
// maxAddress()
//==============
long GPIB::maxAddress() const {
	return(30);
}

//========
// name()
//========
std::string GPIB::name() const {
    return("GPIB");
}

//=========
// query()
//=========
std::string GPIB::query(long address, const std::string& command,
                        double pauseAfterCommand) {

	if ( !command.empty() ) {
		talk(address, command);
        Pause(pauseAfterCommand);
    }

	const static long maxSize = 100;

    MapType::iterator found = map_->find(address);
    if ( found == map_->end() ) {
        // Create a Device
	    // copied from NI sample software
	    int  Device = ibdev(        /* Create a unit descriptor handle         */
            0,                      /* Board Index (GPIB0 = 0, GPIB1 = 1, ...) */
            address,                /* Device primary address                  */
            0,				        /* Device secondary address                */
            T10s,                   /* Timeout setting (T10s = 10 seconds)     */
            1,                      /* Assert EOI line at end of write         */
            0);   

        Assert<BusError>(Device != -1, name() + " Address:" +
                         convert<std::string>(address) + " ibdev");
        ibclr(Device);

        std::pair<MapType::iterator, bool> p;
        p = map_->insert(std::make_pair(address, Device));
        Assert<UnexpectedState>(p.second, name());
        found = p.first;
    }   

    char Buffer[maxSize+1];
    try {	    
	    ibrd(found->second, Buffer, maxSize);  
    } catch(...) {
        throw(BusError(name() +
              " address: " + 
              convert<std::string>(found->first))
             );
    }
    if ( isError() ) {
        std::string error = whatError();
        Assert<BusError>(error.empty(), name() + " " + error + " address: " 
                         + convert<std::string>(found->first));
    }

	Buffer[ibcntl] = '\0';   
	return(std::string(Buffer));
}

//========
// talk()
//========
void GPIB::talk(long address, const std::string& command) {
	MapType::iterator found = map_->find(address);
    if ( found == map_->end() ) {
        // Create a Device
	    // copied from NI sample software
	    int  Device = ibdev(        /* Create a unit descriptor handle         */
            0,                      /* Board Index (GPIB0 = 0, GPIB1 = 1, ...) */
            address,                /* Device primary address                  */
            0,				        /* Device secondary address                */
            T10s,                   /* Timeout setting (T10s = 10 seconds)     */
            1,                      /* Assert EOI line at end of write         */
            0);   

        Assert<BusError>(Device != -1, name() + " Address:" +
                         convert<std::string>(address) + " ibdev");
        ibclr(Device);

        std::pair<MapType::iterator, bool> p;
        p = map_->insert(std::make_pair(address, Device));
        Assert<UnexpectedState>(p.second, name());
        found = p.first;
    } 

    try {
        static const int MAX = 1000;
        char cpy[MAX];
        Assert<UnexpectedState>(MAX > command.length(), name());
        long i = 0, sz = static_cast<long>(command.size());
        for ( ; i < sz; ++i )
            cpy[i] = command[i];
        cpy[i] = '\0';

	    ibwrt(found->second, static_cast<void*>(cpy), sz);
    } catch(...) {
        throw(BusError(name() +
              " address: " + 
              convert<std::string>(found->first))
             );
    }
    if ( isError() ) {
        std::string error = whatError();
        Assert<BusError>(error.empty(), name() + " " + error + " address: "
                         + convert<std::string>(found->first));
    }
}

//=============
// whatError()
//=============
std::string GPIB::whatError() const {
	std::string s = "";
    if ( ibsta & ERR )  s += " ERR";
    if ( ibsta & TIMO ) s += " TIMO";
    if ( ibsta & SRQI)  s += " SRQI";
    if ( ibsta & RQS )  s += " RQS";
    if ( ibsta & LOK )  s += " LOK";
    if ( ibsta & REM )  s += " REM";
    if ( ibsta & CIC )  s += " CIC";
    if ( ibsta & ATN )  s += " ATN";
    if ( ibsta & TACS)  s += " TACS";
    if ( ibsta & LACS)  s += " LACS";
    if ( ibsta & DTAS)  s += " DTAS";
    if ( ibsta & DCAS)  s += " DCAS";

    if ( iberr == ECIC ) s += " ECIC <Not Controller-In-Charge>\n";
    if ( iberr == ENOL ) s += " ENOL <No Listener>\n";
    if ( iberr == EADR ) s += " EADR <Address error>\n";
    if ( iberr == EARG ) s += " EARG <Invalid argument>\n";
    if ( iberr == ESAC ) s += " ESAC <Not System Controller>\n";
    if ( iberr == EABO ) s += " EABO <Operation aborted>\n";
    if ( iberr == ENEB ) s += " ENEB <No GPIB board>\n";
    if ( iberr == EOIP ) s += " EOIP <Async I/O in progress>\n";
    if ( iberr == ECAP ) s += " ECAP <No capability>\n";
    if ( iberr == EFSO ) s += " EFSO <File system error>\n";
    if ( iberr == EBUS ) s += " EBUS <Command error>\n";
    if ( iberr == ESTB ) s += " ESTB <Status byte lost>\n";
    if ( iberr == ESRQ ) s += " ESRQ <SRQ stuck on>\n";
    if ( iberr == ETAB ) s += " ETAB <Table Overflow>\n";

	return(s);	
}

} // namespace SPTSInstrument

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
