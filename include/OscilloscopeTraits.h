// Macro Guard
#ifndef SPTSOscilloscopeTraits_H
#define SPTSOscilloscopeTraits_H

// Files included
#include "Factory.h"
#include "GPIB.h"
#include "SingletonType.h"
#include "StandardFiles.h"
#include "OScopeInterface.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

class OscilloscopeTraits {
    typedef Factory<OScopeInterface, std::string> FactoryType;
public:
    typedef SingletonType<FactoryType> ScopeFactoryType;
    typedef SPTSInstrument::GPIB BusType;
protected:
    ~OscilloscopeTraits() { /* */ }
};
   
#endif  // SPTSOscilloscopeTraits_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
