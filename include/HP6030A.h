// Macro Guard
#ifndef HP6030A_H
#define HP6030A_H

// Files included
#include "InstrumentTags.h"
#include "MainSupplyTraits.h"
#include "SCPI_PowerSupply.h"

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct HP6030A : public SCPI<SingleSupplyTag> {
    static std::string Name() 
        { return("HP6030A"); }
    virtual ~HP6030A() 
        { /* */ }	
}; 

#endif // HP6030A_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
