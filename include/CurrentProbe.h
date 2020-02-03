// Macro Guard
#ifndef SPTS_CURRENTPROBE_H
#define SPTS_CURRENTPROBE_H

// Files included
#include "DateTime.h"
#include "CurrentProbeTraits.h"
#include "Instrument.h"
#include "ProgramTypes.h"
#include "NoCopy.h"
#include "StandardFiles.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   03/02/05, sjn,
   ==============
   Redesigned station to work with TekTCPA300 rather than old TekAM5003.  This requires
     a lot of changes as the new probe is even more primitive (from S/W point of view)
     than the old one.  The station is being re-layed out as only a single current
     probe will be needed with the new amplifier system (the old one had two).  We
     must also take control of explicit timing in between talk/queries or else the new
     probe will crash!

   Removed Probe enum type
   nameLC() and nameHC_ changed to name()
   Removed MinScale(), MaxScale(), SetScale(), WhichProbe(), RescaleUp() and
     SelectProbe().   
   Removed activeProbe_, addressLC_, addressHC_, probeSelected_ and checkDegauss_.

   Modified MaxAmpsAllowed(): Removed CurrentRange argument ; made const
   
   Added void wait();
   Added double waitCalculate();
   Added setAppropriateRangeAndCoupling();
   Added timer_
   Added lastCommand_
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

class CurrentProbe : private Instrument<CurrentProbeTraits::ModelType::BusType>, 
                     private NoCopy {
    typedef CurrentProbeTraits::ModelType ModelType;
    typedef ModelType::Language Language;

public:
     ~CurrentProbe();  

    //========================
    // Start Public Interface
    //========================	
    CurrentProbe();
    ProgramTypes::SetType CurrentScale();
    ProgramTypes::SetType MaxAmpsAllowed() const;
	bool Initialize();
	bool IsError();
    std::string Name() const;
    bool NeedsDegauss();
    bool Reset();
    void setAppropriateRangeAndCoupling();
    bool SystemDegauss();
    std::string WhatError();
    //======================
    // End Public Interface
    //======================

private:    
    typedef CurrentProbeTraits::ModelType::BusType BT;
    void checkDegaussState();
    bool command(const std::string& cmd);
    std::string query(const std::string& q);
    void wait();
    double waitCalculate();

private:
    long         address_;
    bool         locked_;   
    std::string  lastError_;
    std::string  name_;
    bool         needDegauss_;
    Clock        timer_;
    std::string  lastCommand_;
};

} // namespace SPTSInstrument

#endif // SPTS_CURRENTPROBE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
