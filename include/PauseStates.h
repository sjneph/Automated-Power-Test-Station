// Macro Guard
#ifndef SPTS_PAUSESTATES_H
#define SPTS_PAUSESTATES_H

// Files included
#include "NoCopy.h"
#include "ProgramTypes.h"
#include "SingletonType.h"
#include "SPTSFiles.h"
#include "StandardFiles.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============
   08/10/07,  mrb
   ==============
	Added another pause called InhibitPulsewidth.  This will be used to cycle the inhibit
	on the part multiple times to guarantee that the part survives Inhibit regularly.
   
   ==============
   11/14/05, sjn,
   ==============
     Added public method: SupplyCCModeChangePause().  This is related to pauses specific
       to particular power supplies that cannot be captured generically.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct PauseStates : private NoCopy {

    // Public enums
    enum PauseTypes {
        SAFEINHIBITON,                     // Safe Inhibit ON Pause
        SAFEINHIBITOFF,                    // Safe Inhibit OFF Pause
        VOUTDC,                            // Measure Vout DC Pause
        IOUTDC,                            // Measure Iout DC Pause
        MEASUREDMM,                        // Measure DMM Pause
        MEASURESCOPE,                      // Measure Scope Pause
        LLDO,                              // LLDO routine
        OPTIONALINITIALCONDITIONS,         // Optional Initial Conditions Pause
        MISCELLANEOUSINITIALCONDITIONS,    // Miscellaneous Initial Conditions Pause
        TRANSIENTTRIGGER,                  // Pause between Transient Triggers
        TOD,                               // Pause before releasing inhibit for TOD test
        TRIPPOINT,                         // Pause after load settings
		INHIBITPULSEWIDTH				   // Pause used for Life Cycle Test
    };
    enum StationPauses {
        POWERSUPPLYCHANGE = INHIBITPULSEWIDTH + 1, // Change Main PS Voltage
        RELAYSTATECHANGE,                  // Relay State Changes
        SYNCINPUT                          // Apply Sync Input Signal
    };

    /*
        It is worth noting that POWERSUPPLYCHANGE related to main PS voltage changes
          is for the converter's benefit.  The pause associated with the
          SupplyCCModeChangePause(MainSupplyTraits::Supply) is for power supply
          behaviors when going into CC mode, and are specific to the individual supply,
          independent of converter needs.
    */

    // Public Interface
    ProgramTypes::SetType GetPauseValue(PauseTypes type);    
    ProgramTypes::SetType GetPauseValue(StationPauses type);
    ProgramTypes::SetType SupplyCCModeChangePause(MainSupplyTraits::Supply whichSupply);

private:
    typedef std::map<PauseTypes, std::string> MapStringType;
    typedef std::map<PauseTypes, ProgramTypes::SetType> MapType;
    enum Types { DEFAULT, SPECIFIC };

private:
    friend class SingletonType<PauseStates>;
    PauseStates();
    void checkForNewDUT();
    MapStringType getString();
    void loadType(Types t);

private:    
    std::auto_ptr<FileTypes::PauseFileType> pf_, defaultFile_;
    std::auto_ptr<MapType> map_;
    std::string familyNumber_;
    std::string name_;
};

#endif // SPTS_PAUSESTATES_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
