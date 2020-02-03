// Files included
#include "Assertion.h"
#include "Factory.h"
#include "GenericAlgorithms.h"
#include "Measurement.h"
#include "MeasurementFunctions.h"
#include "SPTSException.h"

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { // unnamed
    using namespace SPTSMeasurement;

    typedef StationExceptionTypes::UnexpectedState UnexpectedState;
    typedef Measurement::MFactory MFactory;

    template <typename R>
    Measurement* Create() { 
        return(new R); 
    }

    template <typename R>
    void registerFunc() {
        Assert<UnexpectedState>(MFactory::Instance()->Register(Uppercase(R::Name()), 
                                                               Create<R>));
    }    

    bool forwardFunc() {
        registerFunc<CaseTemperature>();
        registerFunc<CrossRegulation>();
        registerFunc<CrossRegulationXX>();
        registerFunc<Efficiency>();    
        registerFunc<Frequency>();
        registerFunc<GenericVoltsMeasureDC>();
        registerFunc<IinDC>();
        registerFunc<IinPARD>();
		registerFunc<InhibitCycleTest>();
        registerFunc<IoutDC>();
        registerFunc<IoutTripPoint>();
        registerFunc<IoutTripPretrim>();
        registerFunc<LineRegulation>();
        registerFunc<LoadRegulation>();    
        registerFunc<LoadTransientRecovery>();
        registerFunc<LoadTransientResponse>();
        registerFunc<LowLineDropout>();
        registerFunc<PowerDissipation>();
        registerFunc<SCReleaseDelay>();
        registerFunc<SCReleaseOvershoot>();        
        registerFunc<TurnOnDelay>();
        registerFunc<TurnOnOvershoot>();
        registerFunc<VinDC>();
        registerFunc<VinRampDelay>();
        registerFunc<VinRampOvershoot>();
        registerFunc<VoutDC>();
        registerFunc<VoutDCPretrim>();
        registerFunc<VoutPARD>();
        return(true);
    }
    
    static bool dummy = forwardFunc();
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
