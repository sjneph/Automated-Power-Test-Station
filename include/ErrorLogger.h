// Macro Guard
#ifndef SPTS_ERRORLOGGER_H
#define SPTS_ERRORLOGGER_H

// Files included
#include "Converter.h"
#include "DateTime.h"
#include "Messenger.h"
#include "NoCopy.h"
#include "OperatorInterface.h"
#include "SingletonType.h"
#include "StandardFiles.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes (in Main) <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   04/26/05, sjn,
   ==============
     Modified checking of ofstream object in Archive() (comparing explicitly against 0)
       to get rid of performance warnings from the current compiler.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes (in Main) <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct ErrorLogger : public Messenger, private NoCopy {

    //===========
    // Archive()
    //===========
    void Archive() {
        if ( Messenger::IsEmpty() ) // nothing to record
            return;

        try { // Get as much information into the ErrorLogger as possible
            /*
              Realize that the error log should be the one thing that is always
                available.  It is a hardcoded path below because of this.  It doesn't
                really make sense to place a error log in the current working directory
                because that directory may change depending on where the application is
                built - we need a dependable repository that doesn't move and have
                different variations throughout the computer system.  It should not be
                on the network, because we need to report that the network is down, etc.
                No matter what else cannot be read in properly, the error log must be
                accessible to record that information.
            */
            static char* file = "C:\\ErrorLog\\ErrorLog.txt";
            static std::string header = "/*======================================*/";
            OperatorInterface* oi = 0;
            try {
                oi = SingletonType<OperatorInterface>::Instance();
            } catch(...) { oi = 0; }

            Converter* dut = 0;
            try {
                dut = SingletonType<Converter>::Instance();
            } catch(...) { dut = 0; }
            std::ofstream eFile(file, std::ofstream::app);
            Assert<StationExceptionTypes::FileError>(eFile != 0, name());       
            eFile << std::endl     << header               << std::endl;
            eFile << "Date:      " << Date::CurrentDate()  << std::endl;
            eFile << "Time:      " << Clock::CurrentTime() << std::endl;        
            eFile << "Message:   " << *this;
            if ( oi )
                eFile << "OpID:      " << oi->GetOperatorID()  << std::endl;                 

            if ( dut ) {
                eFile << "PN:        " << dut->FamilyNumber()  << "-" 
                                       << dut->DashNumber()    << std::endl;
            }

            if ( oi )
                eFile << "Test Type: " << oi->GetTestType()    << std::endl;
            eFile << header        << std::endl;
            eFile.close();
        } catch(...) { // ? cannot record information
            return;
        }
    }

private:
    friend class SingletonType<ErrorLogger>;
    ErrorLogger()  { /* */ }
    ~ErrorLogger() { /* */ }
    std::string name() { return("Error Logger Class"); }
};

#endif // SPTS_ERRORLOGGER_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
