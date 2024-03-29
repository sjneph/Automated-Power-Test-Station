// Macro Guard
#ifndef SPTS_Agilent6034A_H
#define SPTS_Agilent6034A_H

// Files included
#include "Agilent6034ALanguage.h"
#include "NumberBase.h"

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct Agilent6034A : public Agilent6034ALanguage {  
    static std::string Name() 
        { return("AGILENT6034A"); }
	virtual ~Agilent6034A() { /* */ }
};


#endif  // SPTS_Agilent6034A_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
