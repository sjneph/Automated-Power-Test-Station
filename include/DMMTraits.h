// Macro Guard
#ifndef SPTS_DMMTraits
#define SPTS_DMMTraits

// Files included
#include "Agilent34970A.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct DMMTraits {   
    typedef Agilent34970A ModelType;
protected: 
    ~DMMTraits() { /* */ }
public:
	
};


#endif // SPTS_DMMTraits

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
