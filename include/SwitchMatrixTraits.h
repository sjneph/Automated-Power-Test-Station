// Macro Guard
#ifndef SPTS_SwitchMatrixTraits
#define SPTS_SwitchMatrixTraits

// Files included
#include "Agilent3499AInternal.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct SwitchMatrixTraits {   
    typedef Agilent3499AInternal ModelType;
    typedef ModelType::Relays RelayTypes;
protected: 
    ~SwitchMatrixTraits() { /* */ }
public:
	
};


#endif // SPTS_SwitchMatrixTraits

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
