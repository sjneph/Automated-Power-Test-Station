// Files included for Aardvark-specific I2C control
#include "Aardvark.h"

// Files included
#include "Assertion.h"
#include "Functions.h"
#include "GenericAlgorithms.h"
#include "I2C.h"
#include "SPTSException.h"

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

// The current implementation of this class is very specific to the Aardvark
//  USB-2-I2C adapter.  It is further limited to having only one possible slave
//  currently as it is very unlikely that multiple instruments in the station would
//  ever communicate via I2C.  If it does become necessary, this implementation will
//  need to be extended.  See GPIB.h/GPIB.cpp for an example.
//  portNumber_, commandReg_ and queryReg_ are all hardcoded below for this
//  hardware-specific implementation.

// We use typedef's defined in Aardvark.h: aa_u08 and aa_u16

namespace SPTSInstrument {

//=============
// Constructor
//=============
I2C::I2C() : portNumber_(0),
             commandReg_(static_cast<char>(static_cast<short>(0x37))),
             queryReg_(static_cast<char>(static_cast<short>(0x36))), noStop_(false),
             numBytes_(2), bytesRead_(0), queryBytes_(1), setQueryReg_(false) {

    handle_ = aa_open(portNumber_);
    if ( handle_ <= 0 ) {

        whatError_ = "Unable to open USB-2-I2C device on port ";
        whatError_ += convert<std::string>(portNumber_);
        whatError_ += " Error Code: " + convert<std::string>(handle_);
        throw(BusError(whatError_));
    }
    aa_configure(handle_, AA_CONFIG_SPI_I2C);
    aa_i2c_pullup(handle_, AA_I2C_PULLUP_BOTH);
    aa_target_power(handle_, AA_TARGET_POWER_BOTH);
    aa_i2c_bitrate(handle_, 6); // 6kHz
    aa_i2c_free_bus(handle_);
}

//============
// Destructor
//============
I2C::~I2C() {
    if ( handle_ > 0 )
        aa_close(handle_);
}

//===========
// isError()
//===========
bool I2C::isError() {
    return(!whatError_.empty());
}

//==============
// maxAddress()
//==============
long I2C::maxAddress() const {
	return(100);
}

//========
// name()
//========
std::string I2C::name() const {
    return("I2C");
}

//=========
// query()
//=========
std::string I2C::query(long address, const std::string& command, double toWait) {
	if ( !command.empty() )
		talk(address, command);
     
    int keep = numBytes_;
    try {
        noStop_ = true;
        setQueryReg_ = true;
        char tmp = static_cast<char>(queryReg_);
        std::string str;
        str += tmp;   
        numBytes_ = 1;
        talk(address, str);
        Pause(toWait); // pause between talk and query for toWait
        noStop_ = false;
        setQueryReg_ = false;
        numBytes_ = keep;
    } catch(...) {
        numBytes_ = keep;
        noStop_ = false;
        setQueryReg_ = false;
        aa_i2c_free_bus(handle_);
        throw;
    }

	const static long maxSize = 1000;
    aa_u08 buffer[maxSize+1];
    char toRtn[maxSize + 1];
    aa_u16 numRead;
    try {
	    aa_i2c_read_ext(handle_, static_cast<aa_u16>(address),
                        AA_I2C_NO_FLAGS, static_cast<aa_u16>(numBytes_),
                        buffer, &numRead);
        bytesRead_ = numRead;
        Assert<BusError>(numRead > 0, "I2C Bus Query Error", command);
    } catch(...) {
        throw(BusError(name() +
              " address: " + 
              convert<std::string>(address))
             );
    }
    Assert<BusError>(!isError(),
                     name() + " " + whatError() + 
                     " talking to address: " + convert<std::string>(address));
    numRead = 1; // no read we will ever do goes beyond 0x05
	buffer[numRead] = '\0';
    
    // convert from aa_u08 to char
    aa_u16 i = 0;
    for ( ; i < numRead; ++i )
        toRtn[i] = static_cast<char>(buffer[i]);
    toRtn[i] = '\0';
    return(std::string(toRtn));
}

//========
// talk()
//========
void I2C::talk(long address, const std::string& command) {
    try {
        std::string tmp;
        if ( !setQueryReg_ ) {
            char hex = static_cast<char>(commandReg_);
            tmp += hex;
        }
        tmp += command;
        static const int MAX = 1000;
        aa_u08 cpy[MAX];
        Assert<UnexpectedState>(MAX > tmp.length(), name());
        std::size_t i = 0;
        for ( ; i < tmp.size(); ++i )
            cpy[i] = tmp[i];
        cpy[i] = '\0';
        AA_I2C_FLAGS flag = AA_I2C_NO_FLAGS;
        if ( noStop_ )
            flag = AA_I2C_NO_STOP;
        aa_u16 numWrite = 0;
	    aa_i2c_write_ext(handle_, static_cast<aa_u16>(address),
                         flag, static_cast<aa_u16>(numBytes_),
                         cpy, &numWrite);
        if ( numWrite <= 0 )
            whatError_ = "Bus Command Error: ";
    } catch(...) {
        throw(BusError(name() +
              " talking to address: " + 
              convert<std::string>(address))
             );
    }    
    std::string strAdd = convert<std::string>(address);
    Assert<BusError>(!isError(), name() + " " + whatError() + " address: " + strAdd);
}

//=============
// whatError()
//=============
std::string I2C::whatError() const {
	return(whatError_);
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

