// Macro Guard
#ifndef SPTS_OScopeSetupParameters_H
#define SPTS_OScopeSetupParameters_H


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   09/22/04, sjn,
   ==============
    Removed OScopeParameters::GridNumber enumeration type.  
    Removed OScopeParameters::TriggerMode enumeration values - SINGLE and STOP.
    Removed ExtTrigger enumeration type.  Getting rid of external triggers.
    Removed OScopeChannels::FOUR --> use OScopeChannels::TRIGGER now.
    Removed DCFIFTY from CouplingType.  Not applicable with new layout.
    Added TIME2LEVEL to OScopeMeasurements enum type.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct OScopeParameters {

    // Channel coupling
    enum CouplingType {
		AC1M = 1,
		DC1M,
        GND
	};

    // Memory used during aquisition
	enum MemorySetting {
        MIN = GND + 1,
        SMALL,
        MEDIUM,
        BIG,
        MAX
    };

    // Trigger Coupling
    enum TriggerCouplingType {
		AC = MAX + 1, 
		DC,
        HFR,
        LFR
	};

    // Trigger Mode
    enum TriggerMode {
		AUTO = LFR + 1,
		NORMAL
	};

    // Trigger Slope
    enum SlopeType {
		POSITIVE = NORMAL + 1,
		NEGATIVE
	};

protected:
    ~OScopeParameters() { /* */ }
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct OScopeChannels {
	enum Channel {
		ONE = 1, 
		TWO,
		THREE,
        TRIGGER,
		ALL = -1
	};

protected:
	~OScopeChannels() { /* */ }
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct OScopeMeasurements {
	enum MeasurementType {
		DELAY = 1,
		FREQUENCY,
        HIGHVALUE,
        LOWVALUE,
		MAXIMUMVALUE, // MAXIMUMVALUE >= HIGHVALUE
		MINIMUMVALUE, // MINIMUMVALUE <= LOWVALUE
		PEAK2PEAK,
        TIME2LEVEL
	};

protected:
	~OScopeMeasurements() { /* */ }
};

#endif  // SPTS_OScopeSetupParameters_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
