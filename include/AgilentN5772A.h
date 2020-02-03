// Macro Guard
#ifndef AGILENTN5772A_H
#define AGILENTN5772A_H

// Files included
#include "InstrumentTags.h"
#include "MainSupplyTraits.h"
#include "SCPI_Powersupply.h"

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct AgilentN5772A : public SCPI<SingleSupplyTag> {
    static std::string Name() 
        { return("AGILENTN5772A"); }
    virtual ~AgilentN5772A() 
        { /* */ }	
}; 

#endif // AGILENTN5772A_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
