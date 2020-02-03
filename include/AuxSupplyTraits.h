// Macro Guard
#ifndef SPTSAuxSupplyTraits_H
#define SPTSAuxSupplyTraits_H

// Files included
#include "HP6624A.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct AuxSupplyTraits {
    typedef HP6624A ModelType;
    enum Channels { OUTPUT1 = 1, OUTPUT2 = 2, OUTPUT3 = 3, OUTPUT4 = 4 };   
    enum Range { RANGE1 = 1, RANGE2 };
protected:
    ~AuxSupplyTraits() { /* */ }
};
   
#endif  // SPTSAuxSupplyTraits_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
