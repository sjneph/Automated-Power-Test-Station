// Macro Guard
#ifndef SPTS_TEMPERATURECONTROLLER_H
#define SPTS_TEMPERATURECONTROLLER_H

// Files included
#include "Instrument.h"
#include "NoCopy.h"
#include "ProgramTypes.h"
#include "StandardFiles.h"
#include "TemperatureControllerTraits.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
  ==============
  07/16/03, sjn,
  ==============
      Added GetTemperatureSetting() and private variables setting_, setTemp_.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

struct TemperatureController 
               : private NoCopy,
                 private Instrument<TemperatureControllerTraits::ModelType::BusType> {
     ~TemperatureController();

    //========================
    // Start Public Interface
    //========================	
    TemperatureController();
    ProgramTypes::MType CurrentTemperature();
    ProgramTypes::SetType GetTemperatureSetting() const;
	bool Initialize();
	bool IsError();
    bool IsOn() const;
    std::string Name() const;
    bool OpsComplete();
    bool Reset();
    void SetTemperature(const ProgramTypes::SetType& temperature);
    void SetTemperatureLimits(const ProgramTypes::SetType& lowLimit,
                              const ProgramTypes::SetType& hiLimit);
    void TurnOff();
    std::string WhatError();
    //======================
    // End Public Interface
    //======================

private:
    typedef TemperatureControllerTraits::ModelType::BusType BT;
    bool bitprocess(const std::string& eString, Instrument<BT>::Register toCheck);
    bool command(const std::string& cmd);    
    std::string query(const std::string& q);

private:
    typedef TemperatureControllerTraits::ModelType ModelType;
    typedef ModelType::Language Language;
    long    address_;
    bool    isOff_;
    bool    locked_;
    bool    limitsSpecified_;
    bool    setTemp_;
    ProgramTypes::SetType min_;
    ProgramTypes::SetType max_;
    ProgramTypes::SetType setting_;
};

} // namespace SPTSInstrument

#endif // SPTS_TEMPERATURECONTROLLER_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
