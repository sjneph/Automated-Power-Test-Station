// Files included
#include "Agilent54624A.h"
#include "Assertion.h"
#include "OscilloscopeTraits.h"
#include "OScopeInterface.h"
#include "SPTSException.h"

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { 
   
    OScopeInterface* Create() { 
        return(new Agilent54624A); 
    }

    struct Tmp {
        static bool func() {
            // register the Agilent54624A as a possible oscilloscope
            bool reg = OscilloscopeTraits::ScopeFactoryType::Instance()->Register(
                                                    Agilent54624A::Name(), Create);
            Assert<StationExceptionTypes::UnexpectedState>(reg, Agilent54624A::Name());
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
