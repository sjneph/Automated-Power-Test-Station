// Files included
#include "Assertion.h"
#include "HPE4356A.h"
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
        return(new HPE4356A); 
    }

    struct Tmp {
        static bool func() {
            // register the HP6030A as a possible power supply
            bool reg = MainSupplyTraits::SupplyFactoryType::Instance()->Register(
                                                         HPE4356A::Name(), Create);
            Assert<UnexpectedState>(reg, "Creation of HPE4356A");
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
