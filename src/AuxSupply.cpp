// Files included
#include "Assertion.h"
#include "AuxSupply.h"
#include "SingletonType.h"
#include "SPTSException.h"
#include "StandardStationFiles.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { // unnamed
    typedef AuxSupplyTraits::ModelType::BusType BT;

    typedef StationExceptionTypes::BaseException   StationBaseException;

    typedef StationExceptionTypes::BadArg          BadArg;
    typedef StationExceptionTypes::InstrumentError InstrumentError;
    typedef StationExceptionTypes::OutOfRange      OutOfRange;
    typedef StationExceptionTypes::UnexpectedState UnexpectedState;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

//=============
// Constructor    
//=============
AuxSupply::AuxSupply() : locked_(true), address_(-1), lastError_(""), 
                         map_(new MapType), rangeMap_(new RangeMap), 
                         valueMap_(new ValueMap), name_(Name()) {

    InstrumentFile::Types type = InstrumentFile::APS;
    address_ = SingletonType<InstrumentFile>::Instance()->GetAddress(type);
    
    typedef SingletonType<InstrumentFile> IF;
    typedef ProgramTypes::SetType ST;  

    // Get volts/amps available 
    AuxSupplyTraits::Range rtype = AuxSupplyTraits::RANGE1;
    ST v1 = IF::Instance()->MaxVolts(AuxSupplyTraits::OUTPUT1, rtype);
    ST v2 = IF::Instance()->MaxVolts(AuxSupplyTraits::OUTPUT2, rtype);
    ST v3 = IF::Instance()->MaxVolts(AuxSupplyTraits::OUTPUT3, rtype);
    ST v4 = IF::Instance()->MaxVolts(AuxSupplyTraits::OUTPUT4, rtype);
    ST i1 = IF::Instance()->MaxAmps(AuxSupplyTraits::OUTPUT1, rtype);
    ST i2 = IF::Instance()->MaxAmps(AuxSupplyTraits::OUTPUT2, rtype);
    ST i3 = IF::Instance()->MaxAmps(AuxSupplyTraits::OUTPUT3, rtype);
    ST i4 = IF::Instance()->MaxAmps(AuxSupplyTraits::OUTPUT4, rtype);

    // Place range info in range_
    rangeMap_->insert(std::make_pair(AuxSupplyTraits::OUTPUT1, rtype));
    rangeMap_->insert(std::make_pair(AuxSupplyTraits::OUTPUT2, rtype));
    rangeMap_->insert(std::make_pair(AuxSupplyTraits::OUTPUT3, rtype));
    rangeMap_->insert(std::make_pair(AuxSupplyTraits::OUTPUT4, rtype));

    // Place current/voltage values in valueMap_
    valueMap_->insert(std::make_pair(AuxSupplyTraits::OUTPUT1, std::make_pair(-1, -1)));
    valueMap_->insert(std::make_pair(AuxSupplyTraits::OUTPUT2, std::make_pair(-1, -1)));
    valueMap_->insert(std::make_pair(AuxSupplyTraits::OUTPUT3, std::make_pair(-1, -1)));
    valueMap_->insert(std::make_pair(AuxSupplyTraits::OUTPUT4, std::make_pair(-1, -1)));

    // Place max Amp/Volt info in map_
    std::pair<MapType::iterator, bool> r;
    r = map_->insert(std::make_pair(AuxSupplyTraits::OUTPUT1, std::make_pair(v1, i1)));
    Assert<UnexpectedState>(r.second, name_);
    r = map_->insert(std::make_pair(AuxSupplyTraits::OUTPUT2, std::make_pair(v2, i2)));
    Assert<UnexpectedState>(r.second, name_);
    r = map_->insert(std::make_pair(AuxSupplyTraits::OUTPUT3, std::make_pair(v3, i3)));
    Assert<UnexpectedState>(r.second, name_);
    r = map_->insert(std::make_pair(AuxSupplyTraits::OUTPUT4, std::make_pair(v4, i4)));
    Assert<UnexpectedState>(r.second, name_);
}

//============
// Destructor
//============
AuxSupply::~AuxSupply() 
{ /* */ }

//===========
// command()
//===========
bool AuxSupply::command(const std::string& cmd) {
    Assert<BadArg>(!(cmd.empty() || locked_), name_);
    return(Instrument<BT>::commandInstr(address_, cmd));
}

//================
// FindSetRange()
//================
bool AuxSupply::FindSetRange(AuxSupplyTraits::Channels chan, 
                             const ProgramTypes::SetType& volts, 
                             const ProgramTypes::SetType& amps) {

    // Make sure chan exists
    typedef InstrumentError IE;
    MapType::iterator  i = map_->find(chan); 
    RangeMap::iterator j = rangeMap_->find(chan); 
    Assert<BadArg>(i != map_->end(), name_);
    Assert<BadArg>(j != rangeMap_->end(), name_);

    // See if we can do what is needed within current range
    if ( (i->second.first >= volts) && (i->second.second >= amps) ) {        
        Assert<IE>(SetCurrent(chan, amps) && SetVolts(chan, volts), name_);
        return(true);
    }

    // See if we actually support more than 1 range
    bool doSupport = AuxSupplyTraits::ModelType::DualRanges;
    if ( !doSupport )
        return(false);

    AuxSupplyTraits::Range next;
    switch(j->second) { // Current range        
        case AuxSupplyTraits::RANGE1:
            next = AuxSupplyTraits::RANGE2;
            break;
        case AuxSupplyTraits::RANGE2:
            next = AuxSupplyTraits::RANGE1;
            break;
        default: // ?
            throw(UnexpectedState(name_));
    };

    // Get alternate range information
    typedef ProgramTypes::SetType ST;
    typedef SingletonType<InstrumentFile> IF;
    ST v1 = IF::Instance()->MaxVolts(chan, next);
    ST i1 = IF::Instance()->MaxAmps(chan, next);
   
    // Go to next range if it works, else return false
    if ( (v1 >= volts) && (i1 >= amps) ) {
        j->second = next;
        ST tempV = i->second.first;
        ST tempI = i->second.second;
        try {     
            i->second.first  = v1;
            i->second.second = i1;   
            Assert<IE>
                (
                    SetVolts(chan, volts) &&
                    SetCurrent(chan, amps),
                    name_              
                );
        } catch(StationBaseException& error) {
            j->second = next;
            i->second.first = tempV;
            i->second.second = tempI;
            throw(error);
        }
        return(true);
    }
    return(false);
}

//===========
// GetAmps()
//===========
ProgramTypes::SetType AuxSupply::GetAmps(AuxSupplyTraits::Channels chan) {
    ValueMap::iterator found = valueMap_->find(chan);
    Assert<BadArg>(found != valueMap_->end(), name_);
    return(found->second.second);
}

//==============
// GetMaxAmps()
//==============
ProgramTypes::SetType AuxSupply::GetMaxAmps(AuxSupplyTraits::Channels chan) {
    MapType::iterator i = map_->find(chan);
    Assert<BadArg>(i != map_->end(), name_);
    return(i->second.second);
}

//===============
// GetMaxVolts()
//===============
ProgramTypes::SetType AuxSupply::GetMaxVolts(AuxSupplyTraits::Channels chan) {
    MapType::iterator i = map_->find(chan);
    Assert<BadArg>(i != map_->end(), name_);
    return(i->second.first);
}

//============
// GetVolts()
//============
ProgramTypes::SetType AuxSupply::GetVolts(AuxSupplyTraits::Channels chan) {
    ValueMap::iterator found = valueMap_->find(chan);
    Assert<BadArg>(found != valueMap_->end(), name_);
    return(found->second.first);
}

//==============
// Initialize()
//==============
bool AuxSupply::Initialize() {
    locked_ = false;
    lastError_ = "";
    try {
        Assert<InstrumentError>(command(Language::Initialize()));
        Assert<InstrumentError>(SetCurrent(AuxSupplyTraits::OUTPUT1, 0));
        Assert<InstrumentError>(SetVolts(AuxSupplyTraits::OUTPUT1, 0));
        Assert<InstrumentError>(SetCurrent(AuxSupplyTraits::OUTPUT2, 0));
        Assert<InstrumentError>(SetVolts(AuxSupplyTraits::OUTPUT2, 0));
        Assert<InstrumentError>(SetCurrent(AuxSupplyTraits::OUTPUT3, 0));
        Assert<InstrumentError>(SetVolts(AuxSupplyTraits::OUTPUT3, 0));
        Assert<InstrumentError>(SetCurrent(AuxSupplyTraits::OUTPUT4, 0));
        Assert<InstrumentError>(SetVolts(AuxSupplyTraits::OUTPUT4, 0));
    } catch(StationBaseException& error) {
        locked_ = true;
        throw(error);
    }
    return(true);
}

//===========
// IsError()
//===========
bool AuxSupply::IsError() {
    std::string result = query(Language::IsError());   
    Language::Clean(result);    
    if ( convert<SetType>(result) != convert<SetType>(Language::NoErrorCode()) ) 
        lastError_ = result;
    return(lastError_.empty() ? false : true);
}

//========
// Name()
//========
std::string AuxSupply::Name() const {
    static std::string name = InstrumentFile::GetName(InstrumentFile::APS);
    return(name);
}

//=====================
// OperationComplete()
//=====================
bool AuxSupply::OperationComplete() {
    return(true); // not crucial to our application
}

//=============
// OutputOff()
//=============
bool AuxSupply::OutputOff(Channel chan) {
    Assert<InstrumentError>(command(Language::OutputOff(chan)));    
    return(true);
}

//============
// OutputOn()
//============
bool AuxSupply::OutputOn(Channel chan) {
    Assert<InstrumentError>(command(Language::OutputOn(chan)));    
    return(true);
}

//=========
// query()
//=========
std::string AuxSupply::query(const std::string& q) {
    Assert<BadArg>(!(q.empty() || locked_), name_);
    return(Instrument<BT>::queryInstr(address_, q));
}

//=========
// Reset()
//=========
bool AuxSupply::Reset() {
    return(Initialize());
}

//==============
// SetCurrent()
//==============
bool AuxSupply::SetCurrent(Channel chan, const ProgramTypes::SetType& limit) {
    static std::string name = name_;
    MapType::iterator  i = map_->find(chan);
    ValueMap::iterator j = valueMap_->find(chan);
    Assert<BadArg>(i != map_->end(), name);
    Assert<BadArg>(j != valueMap_->end(), name);
    if ( limit == j->second.second ) // already there
        return(true);
    Assert<OutOfRange>(i->second.second >= limit, name);
    Assert<InstrumentError>(command(Language::SetIin(chan, limit)), name);
    j->second.second = limit; // update valueMap_
    return(true);
}

//============
// SetVolts()
//============
bool AuxSupply::SetVolts(Channel chan, const ProgramTypes::SetType& value) {    
    static std::string name = name_;
    MapType::iterator  i = map_->find(chan);
    ValueMap::iterator j = valueMap_->find(chan);
    Assert<BadArg>(i != map_->end(), name);
    Assert<BadArg>(j != valueMap_->end(), name);
    if ( value == j->second.first ) // already there
        return(true);
    Assert<OutOfRange>(i->second.first >= value, name);
    Assert<OutOfRange>(value >= 0, name);
    Assert<InstrumentError>(command(Language::SetVolts(chan, value)), name);
    j->second.first = value; // update valueMap_
    return(true);
}

//=============
// WhatError()
//=============
std::string AuxSupply::WhatError() {
    std::string toRtn = lastError_;
    lastError_ = "";
    return(toRtn);
}        

} // namespace SPTSInstrument

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
