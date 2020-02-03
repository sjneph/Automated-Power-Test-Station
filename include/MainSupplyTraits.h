// Macro Guard
#ifndef SPTSMainSupplyTraits_H
#define SPTSMainSupplyTraits_H

// Files included
#include "Factory.h"
#include "GPIB.h"
#include "SingletonType.h"
#include "StandardFiles.h"
#include "SupplyInterface.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct SupplyInterface;

class MainSupplyTraits {
    typedef Factory<SupplyInterface, std::string> FactoryType;
public:
    typedef SPTSInstrument::GPIB BusType;
    typedef SingletonType<FactoryType> SupplyFactoryType;
    enum Supply { PS1, PS2, PS3 };
protected:
    ~MainSupplyTraits() { /* */ }
};
   
#endif  // SPTSMainSupplyTraits_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
