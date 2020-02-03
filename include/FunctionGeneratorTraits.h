// Macro Guard
#ifndef SPTSFGTraits_H
#define SPTSFGTraits_H

// Files included
#include "GPIB.h"
#include "Factory.h"
#include "SingletonType.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============
   06/23/05, sjn,
   ==============
     Modified everything --> changed model from static to virtual.  Moving away from
       a static model to a dynamic one so multiple FGs can be swapped in/out of the
       station.  Added support for Factory<> registration.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

class FunctionGeneratorTraits {
    typedef Factory<FunctionGeneratorInterface, std::string> FactoryType;
public:
    typedef SingletonType<FactoryType> FunctionGeneratorFactoryType;
    typedef SPTSInstrument::GPIB BusType;
protected:
    ~FunctionGeneratorTraits() { /* */ }
};
   
#endif  // SPTSFGTraits_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
