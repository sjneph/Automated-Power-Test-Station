// Files included
#include "HP33120A.h"
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
        return(new HP33120A); 
    }

    struct Tmp {
        static bool func() {
            // register the HP33120A as a possible oscilloscope
            typedef FunctionGeneratorTraits::FunctionGeneratorFactoryType FT;
            bool reg = FT::Instance()->Register(HP33120A::Name(), Create);
            Assert<StationExceptionTypes::UnexpectedState>(reg, HP33120A::Name());
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
