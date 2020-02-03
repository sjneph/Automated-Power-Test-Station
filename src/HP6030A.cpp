// Files included
#include "Assertion.h"
#include "HP6030A.h"
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
        return(new HP6030A); 
    }

    struct Tmp {
        static bool func() {
            // register the HP6030A as a possible power supply
            bool reg = MainSupplyTraits::SupplyFactoryType::Instance()->Register(
                                                         HP6030A::Name(), Create);
            Assert<UnexpectedState>(reg, "Creation of HP6030A");
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
