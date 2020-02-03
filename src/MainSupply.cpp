// Files included
#include "GenericAlgorithms.h"
#include "MainSupply.h"
#include "SingletonType.h"
#include "StandardStationFiles.h"

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

   ==============
   05/23/05, sjn,
   ==============
     Modified bitprocess() overload1&2 --> made robust against different size registers.

   ==============  
   12/15/04, sjn,
   ==============
     Modified Initialize() --> switched the order of "clear errors" and "output off",
       helps ensure the condition is cleared before trying to clear any problems.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

namespace {
    typedef MainSupplyTraits::BusType BT;

    typedef StationExceptionTypes::BaseException   StationBaseException;

    typedef StationExceptionTypes::BadArg          BadArg;
    typedef StationExceptionTypes::BadCommand      BadCommand;
    typedef StationExceptionTypes::FileError       FileError;
    typedef StationExceptionTypes::InstrumentError InstrumentError;
    typedef StationExceptionTypes::UnexpectedState UnexpectedState;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=============
// Constructor
//=============
MainSupply::MainSupply(const std::pair<
                                       const ProgramTypes::SetType, 
                                       const ProgramTypes::SetType
                                      >& values)
          : isOff_(true), locked_(true), oc_(false), iin_(0), vin_(0), name_(Name()), 
            hasChanged_(true) {


    typedef SingletonType<InstrumentFile> IF;
    typedef ProgramTypes::SetType ST;

    // Get Power Supply model types available, voltage and current capabilities
    //   try-catch blocks used in case some supplies are not in the instrument rack.
    std::string model1, model2, model3;
    ST v1, v2, v3, i1, i2, i3;
    long add1, add2, add3;
    try {
        model1 = Uppercase(IF::Instance()->GetModelType(InstrumentFile::PS1));
        v1     = IF::Instance()->MaxVolts(MainSupplyTraits::PS1);
        i1     = IF::Instance()->MaxAmps(MainSupplyTraits::PS1);
        add1   = Instrument<BT>::getAddress(InstrumentFile::PS1);
    } catch(...) {
        model1 = "";
        v1     = -1;
        i1     = -1;
        add1   = -1;       
    }

    try {
        model2 = Uppercase(IF::Instance()->GetModelType(InstrumentFile::PS2));
        v2     = IF::Instance()->MaxVolts(MainSupplyTraits::PS2);
        i2     = IF::Instance()->MaxAmps(MainSupplyTraits::PS2);
        add2   = Instrument<BT>::getAddress(InstrumentFile::PS2);
    } catch(...) {
        model2 = "";
        v2     = -1;
        i2     = -1;
        add2   = -1;
    }

    try {
        model3 = Uppercase(IF::Instance()->GetModelType(InstrumentFile::PS3));  
        v3     = IF::Instance()->MaxVolts(MainSupplyTraits::PS3);
        i3     = IF::Instance()->MaxAmps(MainSupplyTraits::PS3);
        add3   = Instrument<BT>::getAddress(InstrumentFile::PS3);
    } catch(...) {
        model3 = "";
        v3     = -1;
        i3     = -1;
        add3   = -1;
    }

    // Ensure that we have at least one defined supply in the rack.  Ensure
    //   used supplies (PS1->PS3) are identified from Low Volts -> High Volts.
    ProgramTypes::SetType zero = 0;
    if ( v1 > zero ) {
        if ( v2 > zero ) {
            Assert<FileError>(v1 <= v2, name_);            
            if ( v3 > zero )
                Assert<FileError>(v2 <= v3, name_);        
        }
        else if ( v3 > zero )
            Assert<FileError>(v1 <= v3, name_);
    }
    else if ( v2 > zero ) {
        if ( v3 > zero )
            Assert<FileError>(v2 <= v3, name_);
    }
    else
        Assert<FileError>(v3 > zero, name_);
    

    // Find supply that fits the job with preference from PS1->PS3
    //  store unnused supplies so they can be initialized for safety
    typedef MainSupplyTraits::SupplyFactoryType SF;
    std::string thisModel;
    ProgramTypes::SetType maxVoltsNeeded = values.first;
    ProgramTypes::SetType maxAmpsNeeded  = values.second;
    if ( (v1 > maxVoltsNeeded) && (i1 > maxAmpsNeeded) ) {
        thisModel   = Uppercase(model1);
        address_    = Instrument<BT>::getAddress(InstrumentFile::PS1);
        supplyType_ = MainSupplyTraits::PS1;
        if ( v2 > zero )
            otherSupplies_.push_back(std::make_pair(
                                     SF::Instance()->CreateObject(Uppercase(model2)),
                                     add2)
                                    );
        
        if ( v3 > zero )
            otherSupplies_.push_back(std::make_pair(
                                     SF::Instance()->CreateObject(Uppercase(model3)),
                                     add3)
                                    );
    }
    else if ( (v2 > maxVoltsNeeded) && (i2 > maxAmpsNeeded) ) {
        thisModel   = Uppercase(model2);
        address_    = Instrument<BT>::getAddress(InstrumentFile::PS2); 
        supplyType_ = MainSupplyTraits::PS2;
        if ( v1 > zero )
            otherSupplies_.push_back(std::make_pair(
                                     SF::Instance()->CreateObject(Uppercase(model1)),
                                     add1)
                                    );

        if ( v3 > zero )
            otherSupplies_.push_back(std::make_pair(
                                     SF::Instance()->CreateObject(Uppercase(model3)),
                                     add3)
                                    );
    }
    else if ( (v3 > maxVoltsNeeded) && (i3 > maxAmpsNeeded) ) {
        thisModel   = Uppercase(model3);
        address_    = Instrument<BT>::getAddress(InstrumentFile::PS3);
        supplyType_ = MainSupplyTraits::PS3;
        if ( v1 > zero ) 
            otherSupplies_.push_back(std::make_pair(
                                     SF::Instance()->CreateObject(Uppercase(model1)),
                                     add1)
                                    );

        if ( v2 > zero ) 
            otherSupplies_.push_back(std::make_pair(
                                     SF::Instance()->CreateObject(Uppercase(model2)),
                                     add2)
                                    );
    }
    else 
        throw(BadCommand(name_, "No P/S available can meet DUT requirements"));
        

    // Create supply to be used
    supply_.reset(SF::Instance()->CreateObject(thisModel));
    Assert<UnexpectedState>(supply_.get() != 0, name_);
}

//============
// Destructor
//============
MainSupply::~MainSupply() 
{ /* */ }

//========================
// bitprocess() overload1
//========================
bool MainSupply::bitprocess(std::string& toProcess) {
	// Convert toProcess to binary representation
    toProcess = GetNumericInteger(toProcess);
    Assert<BadArg>(! toProcess.empty(), name_);
	NumberBase conversion(toProcess, 
                          static_cast<NumberBase::Base>(supply_->RegisterReturnType()));
	toProcess = conversion.Value(NumberBase::BINARY);
    std::reverse(toProcess.begin(), toProcess.end()); // toProcess is backwards

	long size = static_cast<long>(toProcess.size());
    std::set<std::string> bitSet;
	char isset = '1';
	for ( long idx = 0; idx < size; ++idx ) {
		if ( toProcess.at(idx) == isset )
			bitSet.insert(convert<std::string>(idx));
	} // for-loop

    const char delim = supply_->BitDelimmitter();
    std::vector<std::string> oBits = SplitString(supply_->OverCurrentCheckBits(), delim);
    std::set<std::string> oSet(oBits.begin(), oBits.end());
    std::vector<std::string> inter;
    std::set_intersection(oSet.begin(), oSet.end(),
                          bitSet.begin(), bitSet.end(),
                          std::back_inserter(inter));
    oc_ = false;
    oc_ = (!inter.empty());
	return(oc_);
}

//========================
// bitprocess() overload2
//========================
bool MainSupply::bitprocess(std::string& eString, Instrument<BT>::Register toCheck) {
	// Convert eString to binary representation
    eString = GetNumericInteger(eString);
    Assert<BadArg>(!eString.empty(), name_);
	NumberBase conversion(eString, 
                          static_cast<NumberBase::Base>(supply_->RegisterReturnType()));
	eString = conversion.Value(NumberBase::BINARY);
    std::reverse(eString.begin(), eString.end()); // eString is backwards

	long size = static_cast<long>(eString.size());
    std::set<std::string> bitSet;
	char isset = '1';
	for ( long idx = 0; idx < size; ++idx ) {
		if ( eString.at(idx) == isset )
			bitSet.insert(convert<std::string>(idx));
	} // for-loop

    const char delim = supply_->BitDelimmitter();
    std::vector<std::string> eBits, oBits;
    std::set<std::string> eSet, oSet;
    std::vector<std::string> inter;	
	switch(toCheck) {
		case ERROR: // ERROR --> if an error exists, return true
            eBits = SplitString(supply_->ErrorBits(), delim);
            eSet.insert(eBits.begin(), eBits.end());            
            std::set_intersection(eSet.begin(), eSet.end(),
                                  bitSet.begin(), bitSet.end(),
                                  std::back_inserter(inter));
			break;
		default:   // OPSCOMPLETE --> if done, then return true
            oBits = SplitString(supply_->OpCompleteBits(), delim);
            oSet.insert(oBits.begin(), oBits.end());
            std::set_intersection(oSet.begin(), oSet.end(),
                                  bitSet.begin(), bitSet.end(),
                                  std::back_inserter(inter));
	}; // Switch
	return(!inter.empty());
}

//========================
// CanTrustVoltsMeasure()
//========================
bool MainSupply::CanTrustVoltsMeasure() const {
    typedef SingletonType<InstrumentFile> IF;
    return(!IF::Instance()->SetMainSupplyVoltsWithDMM(supplyType_));
}

//=====================
// command() overload1
//=====================
bool MainSupply::command(const std::string& cmd) {
    Assert<InstrumentError>(!(locked_ || cmd.empty()), name_);
    return(Instrument<BT>::commandInstr(address_, cmd)); 
}

//=====================
// command() overload2
//=====================
bool MainSupply::command(const std::string& cmd, long address) {
    Assert<InstrumentError>(!(locked_ || cmd.empty()), name_);
    return(Instrument<BT>::commandInstr(address, cmd)); 
}

//===============
// GetAccuracy()
//===============
ProgramTypes::MType MainSupply::GetAccuracy() const {
    typedef SingletonType<InstrumentFile> IF;
    return(IF::Instance()->VoltageAccuracy(supplyType_).Value());
}

//===========
// GetAmps()
//===========
ProgramTypes::SetType MainSupply::GetAmps() {
    return(iin_);
}

//============
// GetVolts()
//============
ProgramTypes::SetType MainSupply::GetVolts() {
    return(vin_);
}

//==============
// Initialize()
//==============
bool MainSupply::Initialize() {
    isOff_  = true;
    locked_ = false;
    hasChanged_ = true;
    try {
        Assert<InstrumentError>(command(supply_->Initialize()), name_);
        Assert<InstrumentError>(OutputOff(), name_);
        Assert<InstrumentError>(command(supply_->ClearErrors()), name_);

        // Initialize unused supplies to ensure they are off, then delete
        for ( std::size_t idx = 0; idx < otherSupplies_.size(); ++idx ) {
            Assert<InstrumentError>(
                 command(otherSupplies_[idx].first->Initialize(), 
                         otherSupplies_[idx].second), name_
            );
            Assert<InstrumentError>(
                 command(otherSupplies_[idx].first->OutputOff(),
                         otherSupplies_[idx].second), name_
            );
            Assert<InstrumentError>(
                 command(otherSupplies_[idx].first->ClearErrors(),
                         otherSupplies_[idx].second), name_
            );
            delete(otherSupplies_.at(idx).first);
            otherSupplies_.at(idx).first = 0;
        }      
        otherSupplies_.erase(otherSupplies_.begin(), otherSupplies_.end());
    } catch(...) {
        locked_ = true;
        throw;
    }
    return(true);
}

//===========
// IsError()
//===========
bool MainSupply::IsError() {
    std::string reg = query(supply_->IsError());
    bool toRtn = bitprocess(reg, Instrument<BT>::ERROR);
    reg = query(supply_->OverCurrentCheck());
    return(bitprocess(reg) || toRtn);
}

//========
// IsOn()
//========
bool MainSupply::IsOn() {
    return(!isOff_);
}

//================
// MeasureVolts()
//================
ProgramTypes::MType MainSupply::MeasureVolts() {
    if ( !hasChanged_ ) 
      return(vin_.Value());
    try {
        hasChanged_ = false;    
        std::string result = query(supply_->MeasureVolts());
        return(convert<ProgramTypes::MType>(result));
    } catch(...) {
        hasChanged_ = true;
        throw;
    }
}

//========
// Name()
//========
std::string MainSupply::Name() const {
    return("Main Supply");
}

//=====================
// OperationComplete()
//=====================
bool MainSupply::OperationComplete() {
    std::string ensure = supply_->SetOpsComplete();
    if ( ! ensure.empty() ) // not supported on this supply model
        Assert<InstrumentError>(command(ensure), name_);
    std::string toQuery = supply_->IsDone();
    if ( toQuery.empty() ) // not supported on this supply model
        return(true);
    std::string done = query(toQuery);
    return(bitprocess(done, Instrument<BT>::OPSCOMPLETE));
}

//=============
// OutputOff()
//=============
bool MainSupply::OutputOff() {    
    Assert<InstrumentError>(command(supply_->OutputOff()));
    isOff_ = true;
    hasChanged_ = true;
    return(isOff_);
}

//============
// OutputOn()
//============
bool MainSupply::OutputOn() {
    Assert<InstrumentError>(command(supply_->OutputOn()));
    isOff_ = false;
    hasChanged_ = true;
    return(!isOff_);
}

//=========
// query()
//=========
std::string MainSupply::query(const std::string& q) {
    Assert<InstrumentError>(! (locked_ || q.empty()), name_);
    return(Instrument<BT>::queryInstr(address_, q)); 
}

//=======
// Reset
//=======
bool MainSupply::Reset() {
    return(Initialize());
}

//==============
// SetCurrent()
//==============
bool MainSupply::SetCurrent(const ProgramTypes::SetType& limit) {
    Assert<BadArg>(limit >= ProgramTypes::SetType(0), name_);
    if ( iin_ == limit )
        return(true); // already there
    if ( command(supply_->SetAmps(limit)) ) {
        iin_ = limit;
        hasChanged_ = true;
        return(true);
    }   
    hasChanged_ = true;     
    return(false);
}

//========================
// SetCurrentProtection()
//========================
bool MainSupply::SetCurrentProtection(Switch state) {
    return(command(supply_->SetCurrentProtection(state)));
}

//============
// SetVolts()
//============
bool MainSupply::SetVolts(const ProgramTypes::SetType& value) {   
    Assert<BadArg>(value >= ProgramTypes::SetType(0), name_);
    if ( vin_ == value ) // already there
        return(true); 
    if ( command(supply_->SetVolts(value)) ) {
        vin_ = value;
        hasChanged_ = true;
        return(true);
    }
    hasChanged_ = true;
    return(false);
}

//=============
// WhatError()
//=============
std::string MainSupply::WhatError() {
    std::string toRtn = query(supply_->WhatError());
    if ( oc_ )
        toRtn += " : Overcurrent Condition Detected";
    oc_ = false;
    return(toRtn);
}

//===============
// WhichSupply()
//===============
MainSupplyTraits::Supply MainSupply::WhichSupply() const { 
    return(supplyType_);
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
