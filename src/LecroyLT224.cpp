// Files included
#include "Assertion.h"
#include "LecroyLT224.h"
#include "OscilloscopeTraits.h"
#include "OScopeInterface.h"
#include "SPTSException.h"

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { 
   
    OScopeInterface* Create() { 
        return(new LecroyLT224); 
    }

    struct Tmp {
        static bool func() {
            // register the LT224 as a possible oscilloscope
            bool reg = OscilloscopeTraits::ScopeFactoryType::Instance()->Register(
                                                      LecroyLT224::Name(), Create);
            Assert<StationExceptionTypes::UnexpectedState>(reg, LecroyLT224::Name());
            return(reg);
        }
    };
    static bool reg = Tmp::func();

} // end unnamed namespace

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
