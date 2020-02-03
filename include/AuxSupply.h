// Macro Guard
#ifndef SPTS_AUXSUPPLY_H
#define SPTS_AUXSUPPLY_H

// Files included
#include "AuxSupplyTraits.h"
#include "Instrument.h"
#include "NoCopy.h"
#include "ProgramTypes.h"
#include "StandardFiles.h"
#include "Switch.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

struct AuxSupply : private Instrument<AuxSupplyTraits::ModelType::BusType>, 
                   private NoCopy {
    // Public Typedefs
    typedef AuxSupplyTraits::Channels Channel;

    //========================
    // Start Public Interface
    //========================
    AuxSupply();
    ~AuxSupply();
    bool FindSetRange(AuxSupplyTraits::Channels chan, 
                      const ProgramTypes::SetType& volts, 
                      const ProgramTypes::SetType& amps);
    ProgramTypes::SetType GetAmps(AuxSupplyTraits::Channels chan);
    ProgramTypes::SetType GetMaxAmps(AuxSupplyTraits::Channels chan);
    ProgramTypes::SetType GetMaxVolts(AuxSupplyTraits::Channels chan);
    ProgramTypes::SetType GetVolts(AuxSupplyTraits::Channels chan);
    bool Initialize();
    bool IsError();
    std::string Name() const;
    bool OperationComplete();
    bool OutputOff(Channel chan);
    bool OutputOn(Channel chan);
    bool Reset();
    bool SetCurrent(Channel chan, const ProgramTypes::SetType& limit);
    bool SetVolts(Channel chan, const ProgramTypes::SetType& value);
    std::string WhatError();
    //======================
    // End Public Interface
    //======================

private:    
    bool command(const std::string& cmd);
    std::string query(const std::string& q);

private:
    typedef AuxSupplyTraits::ModelType ModelType;
    typedef ModelType::Language Language;
    typedef ProgramTypes::SetType SetType;
    typedef SetType MaxVout;
    typedef SetType Vset;
    typedef SetType MaxIout;
    typedef SetType Iset;
    typedef std::map< Channel, std::pair<MaxVout, MaxIout> > MapType;
    typedef std::map< Channel, std::pair<Vset, Iset> > ValueMap;
    typedef std::map<Channel, AuxSupplyTraits::Range> RangeMap;
    bool locked_;
    long address_;
    std::string lastError_;
    std::auto_ptr<MapType> map_;
    std::auto_ptr<RangeMap> rangeMap_;
    std::auto_ptr<ValueMap> valueMap_;
    std::string name_;
};

} // namespace SPTSInstrument

#endif // SPTS_AUXSUPPLY_H


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
