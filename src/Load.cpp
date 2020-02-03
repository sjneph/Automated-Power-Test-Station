
// Files included
#include "Assertion.h"
#include "Load.h"
#include "SPTSException.h"
#include "StandardStationFiles.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { // unnamed 
    // Exceptions
    struct ELoadOnlyTag {
        static std::string Name() {
            return("Bad Command - Applies to Electronic Load Only");
        }
    };    
    typedef ExceptionTypes::SomeException<ExceptionTypes::NoErrorNumber, 
                                          StationExceptionTypes::BaseException, 
                                          ELoadOnlyTag> ELoadOnly;

    // getAllRLLs()
    SPTSInstrument::ResistiveLoad::RLLContainer getAllRLLs() {
        SPTSInstrument::ResistiveLoad::RLLContainer rl;
        rl.insert(ControlMatrixTraits::RelayTypes::RLL1);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL2);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL3);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL4);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL5);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL6);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL7);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL8);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL9);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL10);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL11);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL12);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL13);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL14);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL15);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL16);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL17);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL18);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL19);
        rl.insert(ControlMatrixTraits::RelayTypes::RLL20);
        return(rl);
    }    
} // unnamed

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

//=============
// Constructor
//=============
Load::Load() : rl_(0), el_(0) { 
    rl_.reset(new ResistiveLoad());
    el_.reset(new ElectronicLoad());
} 

//============
// Destructor
//============
Load::~Load() 
{ /* */ } 

//==================
// AddLoadChannel()
//==================
bool Load::AddLoadChannel(LoadTraits::Channels chan) {
	if ( isEL() ) {
		ProgramTypes::SetType a, b, c, d;
		a = SingletonType<InstrumentFile>::Instance()->MaxAmps(chan);
		b = SingletonType<InstrumentFile>::Instance()->MaxVolts(chan);
        c = SingletonType<InstrumentFile>::Instance()->MinOhms(chan);
        d = SingletonType<InstrumentFile>::Instance()->MaxOhms(chan);
		return(el_->AddLoadChannel(chan, a, b, c, d));
	}
	return(true);  // rl_ takes care of this on its own
}

//===============
// Concatenate()
//===============
void Load::Concatenate(Switch state) { 
	isEL() ? el_->Concatenate(state) : rl_->Concatenate(state); 
}

//================
// GetLoadValue()
//================
std::pair<Switch, ProgramTypes::SetType> 
                          Load::GetLoadValue(LoadTraits::Channels channel) {
    std::pair<Switch, ProgramTypes::SetType> p;
    if ( isEL() )
        p = std::make_pair(LoadState(channel), el_->GetLoadValue(channel));
    else // rl_
        p = std::make_pair(LoadState(channel), rl_->GetLoadValue(channel));
    return(p);
}

//===========
// GetMode()
//===========
LoadTraits::Modes Load::GetMode(LoadTraits::Channels chan) const { 
	return(isEL() ? el_->GetMode(chan) : LoadTraits::CR); 
}

//=================
// ImmediateMode()
//=================
void Load::ImmediateMode() {
    isEL() ? el_->ImmediateMode() : rl_->ImmediateMode();
}

//==============
// Initialize()
//==============
bool Load::Initialize() {
    // Make sure el_ and rl_ are initialized, then delete the unused one    
    rl_->InitializeAllRLLs(getAllRLLs());
    if ( SingletonType<VariablesFile>::Instance()->LoadType() == LoadTraits::PASSIVE )
        el_.reset(0);
    else
        rl_.reset(0);
   return(isEL() ? el_->Initialize() : rl_->Initialize());
}

//========
// isEL()
//========
bool Load::isEL() const {
    return(0 != el_.get());
}

//===========
// IsError()
//===========
bool Load::IsError() const { 
	return(isEL() ? el_->IsError() : rl_->IsError()); 
}

//=============
// LoadState()
//=============
Switch Load::LoadState(LoadTraits::Channels channel) const { 
	return(isEL() ? el_->CurrentState(channel) : rl_->CurrentState(channel)); 
}

//============
// LoadType()
//============
LoadTraits::Types Load::LoadType() const { 
	return(isEL() ? LoadTraits::ELECTRONIC : LoadTraits::PASSIVE); 
}

//==============
// MaxCurrent()
//==============
ProgramTypes::SetType Load::MaxCurrent(LoadTraits::Channels chan) const { 
	return(isEL() ? el_->MaxAmps(chan) : rl_->MaxAmps(chan));
}

//============
// MaxVolts()
//============
ProgramTypes::SetType Load::MaxVolts(LoadTraits::Channels chan) const { 
	return(isEL() ? el_->MaxVolts(chan) : static_cast<SetType>(1000));  
	/* no such ResistiveLoad specification */
}

//===============
// MeasureAmps()
//===============
ProgramTypes::MType Load::MeasureAmps(LoadTraits::Channels chan) {
    if ( isEL() )
        return(el_->MeasureCurrent(chan));
    throw(ELoadOnly(Name()));
}

//================
// MeasureVolts()
//================
ProgramTypes::MType Load::MeasureVolts(LoadTraits::Channels chan) {
    if ( isEL() )
        return(el_->MeasureVoltage(chan));
    throw(ELoadOnly(Name()));
}

//==============
// MinMaxOhms()
//==============
std::pair<ProgramTypes::SetType, ProgramTypes::SetType> 
                    Load::MinMaxOhms(LoadTraits::Channels chan) const {
    if ( isEL() )
        return(el_->MinMaxOhms(chan));
    throw(ELoadOnly(Name()));
}

//========
// name()
//========
std::string Load::name() const { 
	return("Load"); 
}

//========
// Name()
//========
std::string Load::Name() const {
    return(isEL() ? el_->Name() : rl_->Name());
}

//=======
// Off() 
//=======
void Load::Off(LoadTraits::Channels channel) { 
	isEL() ? el_->LoadOff(channel) : rl_->LoadOff(channel); 
}

//======
// On()
//======
void Load::On(LoadTraits::Channels channel) { 
	isEL() ? el_->LoadOn(channel) : rl_->LoadOn(channel); 
}

//=====================
// OperationComplete()
//=====================
bool Load::OperationComplete() const { 
	return(isEL() ? el_->OpsComplete() : rl_->OpsComplete()); 
}

//=================
// ParallelLoads()
//=================
LoadTraits::Channels Load::ParallelLoads(LoadTraits::Channels ChA, 
										 LoadTraits::Channels ChB) { 
    if ( isEL() ) 
		return(el_->SetParallelLoads(ChA, ChB)); 
	throw(ELoadOnly(name())); // Not for resistiveLoad --> parallel
                              //  what you want directly in hardware
}

//=========
// Reset()
//=========
bool Load::Reset() { 
	return(isEL() ? el_->Reset() : rl_->Reset()); 
}

//======================
// ResetParallelLoads()
//======================
void Load::ResetParallelLoads() { 
	if ( isEL() ) 
		el_->ResetParallelLoads(); 
    else 
	    throw(ELoadOnly(name())); // Not for ResistiveLoad
}

//================
// SetLoadValue()
//================
void Load::SetLoadValue(LoadTraits::Channels channel, 
		                const ProgramTypes::SetType& value)  { 
     isEL() ? el_->SetLoad(channel, value) : rl_->SetLoad(channel, value); 
}

//===========
// SetMode()
//===========
bool Load::SetMode(LoadTraits::Channels chan, LoadTraits::Modes mode) { 
	return(isEL() ? el_->SetMode(chan, mode) : true); 
}

//==========================
// SetTransient() overload1
//==========================
void Load::SetTransient(LoadTraits::Channels chan, 
		                const ProgramTypes::SetType& A,
		                const ProgramTypes::SetType& B, 
					    const ProgramTypes::SetType& Rate) {
    isEL() ? 
		el_->SetXSTStates(chan, A, B, Rate) : 
	    rl_->SetXSTStates(chan, A, B); 
}

//==========================
// SetTransient() overload2
//==========================
void Load::SetTransient(LoadTraits::Channels chan,
                  const ProgramTypes::SetType& A,
                  const ProgramTypes::SetType& B) {
    // use for maximum slew rate
    isEL() ?
        el_->SetXSTStates(chan, A, B, ElectronicLoad::MAXSLEW) :
        rl_->SetXSTStates(chan, A, B);
}

//================
// TransientOff()
//================
void Load::TransientOff(LoadTraits::Channels chan) { 
	isEL() ? 
		el_->ResetXSTStates(chan) : 
	    rl_->ResetXSTStates(chan); 
}

//===============
// TransientOn()
//===============
void Load::TransientOn() { 
	isEL() ? 
		el_->TriggerXSTEvent() : 
	    rl_->TriggerXSTEvent(); 
}

//=============
// WhatError()
//=============
std::string Load::WhatError() const { 
	return(isEL() ? el_->WhatError() : rl_->WhatError()); 
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
