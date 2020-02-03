// Files included
#include "Assertion.h"
#include "AgilentN5772A.h"
#include "MainSupplyTraits.h"
#include "SPTSException.h"
#include "SupplyInterface.h"

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { 
    typedef StationExceptionTypes::UnexpectedState UnexpectedState;

    SupplyInterface* Create() { 
        return(new AgilentN5772A); 
    }

    struct Tmp {
        static bool func() {
            // register the AgilentN5772A as a possible power supply
            bool reg = MainSupplyTraits::SupplyFactoryType::Instance()->Register(
                                                       AgilentN5772A::Name(), Create);
            Assert<UnexpectedState>(reg, "Creation of AgilentN5772A");
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
