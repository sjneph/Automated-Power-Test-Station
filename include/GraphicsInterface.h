// Macro Guard
#ifndef SPTS_GUI_INTERFACE
#define SPTS_GUI_INTERFACE

// Files included
#include "Functions.h"
#include "StandardFiles.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============  
   03/26/05, sjn,
   ==============
     Added void SoakingAtTemperature(bool isSoaking) to support GEN-029 changes.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct GraphicsInterface {
 
    //=======
    // enums
    //=======
    enum TestInfoParameters {
        DASH = 0,
        DEBUGMODE,
        ENGTEST,
        GOLDTEST,
        LIMITSREV,
        OPID,
        PN,
        RMTEST,
        SN,
        STOPFIRSTFAIL,
        TESTENGTEST,
        TESTYPE,
        WO
    };

    //===========
    // Constants
    //===========
    static const char Delimiter = ';';

    //=========================
    // Static member functions
    //=========================
    static long NumberParameters() { return(WO + 1); }

    //==================
    // Public Interface
    //==================
    virtual void AtTemperature(Temperature temp) = 0;
    virtual void Close() = 0;
    virtual void DisplayDialogInteractive(const std::string& info) = 0;  
    virtual void DisplayDialogMessage(const std::string& info) = 0;
    virtual void DisplayDialogWarning(const std::string& warning) = 0;
    virtual void DisplayResults(bool result, const std::string& info) = 0;
    virtual std::string GetTestInfo() = 0;
    virtual void Initialize(const std::vector<std::string>& stationInfo) = 0;
    virtual bool IsDonePrinting() = 0;
    virtual bool IsError() = 0;
    virtual void Print(const std::string& toPrint) = 0;
    virtual void RampingToTemperature(Temperature temp) = 0;
    virtual void Reset() = 0;
    virtual void SetSequenceResult(bool result) = 0;
    virtual void ShowTestInfo(const std::vector<std::string>& info)  = 0;
    virtual void SoakingAtTemperature(bool isSoaking) = 0;
    virtual bool UserRequestAbort() = 0;
    virtual bool UserRequestClosed() = 0;
    virtual bool UserRequestTesting() = 0;
    virtual std::string WhatError() = 0;
    virtual ~GraphicsInterface() { /* */ }
};

#endif // SPTS_GUI_INTERFACE

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
