// Macro Guard
#ifndef SPTS_ControlMatrixTraits
#define SPTS_ControlMatrixTraits

// Files included
#include "Agilent3499AExternal.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct ControlMatrixTraits {    
    typedef Agilent3499AExternal ModelType;
    typedef ModelType::Relays RelayTypes;
protected:
    ~ControlMatrixTraits() { /* */ }
};


#endif // SPTS_ControlMatrixTraits

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
