// Macro Guard
#ifndef SPTSLoadTraits_H
#define SPTSLoadTraits_H

// Files included
#include "AgilentN3300A.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct LoadTraits {
    typedef AgilentN3300A ModelType;
	enum Types       { ELECTRONIC, PASSIVE };
	enum Modes       { CR, CC }; 
	enum Channels    { ONE = 1, TWO, THREE, FOUR, FIVE };
    enum AllChannels { ALL };
	enum             { MAXCHANNELS = FIVE };
protected:
	~LoadTraits() {}
};
   
#endif  // SPTSLoadTraits_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
