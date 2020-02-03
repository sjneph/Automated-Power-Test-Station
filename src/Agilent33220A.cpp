// Files included
#include "Agilent33220A.h"
#include "Assertion.h"
#include "FunctionGeneratorTraits.h"
#include "FunctionGeneratorInterface.h"
#include "SPTSException.h"

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { 
   
    FunctionGeneratorInterface* Create() { 
        return(new Agilent33220A); 
    }

    struct Tmp {
        static bool func() {
            // register the Agilent33220A as a possible oscilloscope
            typedef FunctionGeneratorTraits::FunctionGeneratorFactoryType FT;
            bool reg = FT::Instance()->Register(Agilent33220A::Name(), Create);
            Assert<StationExceptionTypes::UnexpectedState>(reg, Agilent33220A::Name());
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
