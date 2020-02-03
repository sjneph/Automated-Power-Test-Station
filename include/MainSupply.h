// Macro Guard
#ifndef SPTS_MAINSUPPLY_H
#define SPTS_MAINSUPPLY_H

// Files included
#include "Instrument.h"
#include "MainSupplyTraits.h"
#include "NoCopy.h"
#include "ProgramTypes.h"
#include "StandardFiles.h"
#include "SupplyInterface.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   11/14/05, sjn,
   ==============
     Added SetCurrentProtection() to the public interface.

   ==============
   10/07/05, sjn,
   ==============
     Added CanTrustVoltsMeasure() to the public interface.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

struct MainSupply : private Instrument<MainSupplyTraits::BusType>, private NoCopy {

    //========================
    // Start Public Interface
    //========================
    explicit MainSupply(const std::pair<const ProgramTypes::SetType, 
                                        const ProgramTypes::SetType>& values);
    ~MainSupply();
    bool CanTrustVoltsMeasure() const;
    ProgramTypes::MType GetAccuracy() const;
    ProgramTypes::SetType GetAmps();
    ProgramTypes::SetType GetVolts();  
    bool Initialize();
    bool IsError();
    bool IsOn();
    ProgramTypes::MType MeasureVolts();
    std::string Name() const;
    bool OperationComplete();
    bool OutputOff();
    bool OutputOn();
    bool Reset();
    bool SetCurrent(const ProgramTypes::SetType& limit);
    bool SetCurrentProtection(Switch state);
    bool SetVolts(const ProgramTypes::SetType& value);
    std::string WhatError();
    MainSupplyTraits::Supply WhichSupply() const;
    //======================
    // End Public Interface
    //======================

private:
    bool bitprocess(std::string& toProcess);
    bool bitprocess(std::string& eString, 
                    Instrument<MainSupplyTraits::BusType>::Register toCheck);
    bool command(const std::string& cmd);
    bool command(const std::string& cmd, long address);
    std::string query(const std::string& q);

private:
    std::auto_ptr<SupplyInterface> supply_;
    std::vector< std::pair<SupplyInterface*, long> > otherSupplies_;
    bool isOff_;
    bool locked_;
    bool oc_;
    long address_;
    bool hasChanged_;
    ProgramTypes::SetType iin_;
    ProgramTypes::SetType vin_;
    MainSupplyTraits::Supply supplyType_;
    std::string name_;
};

} // namespace SPTSInstrument

#endif // SPTS_MAINSUPPLY_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
