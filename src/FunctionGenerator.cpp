// Files included
#include "Assertion.h"
#include "FunctionGenerator.h"
#include "GenericAlgorithms.h"
#include "InstrumentFile.h"
#include "SPTSException.h"
#include "StandardStationFiles.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============
   06/23/05, sjn,
   ==============
     Modified most functions due to moving away from static model to a dynamic one (to
       support multiple FGs simultaneously).  Added fg_ member variable - a auto_ptr
       to newly created FunctionGeneratorInterface.  This resulted in all previous
       Language::function() calls to fg_->function() calls.  Changed constructor to
       support runtime identification/loading of the appropriate function generator.
     Added #include "InstrumentFile.h"

   ==============
   05/23/05, sjn,
   ==============
     Modified bitprocess() overload --> made robust against differing size registers.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    typedef InstrumentFile IF;
    typedef FunctionGeneratorTraits::BusType BT;

    typedef StationExceptionTypes::BadArg          BadArg;
    typedef StationExceptionTypes::InstrumentError InstrumentError;
    typedef StationExceptionTypes::UnexpectedState UnexpectedState;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=============
// Constructor
//=============
FunctionGenerator::FunctionGenerator() 
  : ampl_(0), dc_(0), freq_(0), offset_(0), locked_(true), isOff_(false) {
                               
    IF* ptr = SingletonType<IF>::Instance();
    address_ = ptr->GetAddress(IF::FUNCTIONGENERATOR);

    typedef FunctionGeneratorTraits::FunctionGeneratorFactoryType FF;
    std::set<std::string> possibilities = FF::Instance()->GetAllRegistered();
    std::set<std::string>::iterator a = possibilities.begin();
    while ( a != possibilities.end() ) { // Create fg types until correct one found
        try {
            fg_.reset(FF::Instance()->CreateObject(*a));
            locked_ = false;
            std::string syntax = fg_->Identify();            
            std::string model;
            try {
                ptr->SetFunctionGeneratorType(query(syntax));
                model = Uppercase(ptr->GetModelType(IF::FUNCTIONGENERATOR));
                fg_.reset(FF::Instance()->CreateObject(model));
                break;
            } catch(FileError&) { // InstrumentFile does not recognize query() value
                locked_ = true;
                syntax = "";
                fg_.reset(0);
            }
        } catch(...) {
            locked_ = true;
            fg_.reset(0);
            throw;
        }
        ++a;
    } // while
    locked_ = true;
    Assert<UnexpectedState>(fg_.get() != 0, Name());
}

//==============
// Destructor()
//==============
FunctionGenerator::~FunctionGenerator()
{ /* */ }

//==============
// bitprocess() 
//==============
bool FunctionGenerator::bitprocess(const std::string& errorString,
                                   Instrument<BT>::Register toCheck) {
	// Convert eString to binary representation
    std::string eString = GetNumericInteger(errorString);
    Assert<BadArg>(! eString.empty(), Name());
	NumberBase conversion(eString, fg_->RegisterReturnType());
	eString = conversion.Value(NumberBase::BINARY);
    std::reverse(eString.begin(), eString.end()); // eString is backwards

	long size = static_cast<long>(eString.size());
	Assert<UnexpectedState>(size <= fg_->TotalRegisterBits(), Name());
	std::set<std::string> bitSet;
	char isset = '1';
	for ( long idx = 0; idx < size; ++idx ) {
		if ( eString.at(idx) == isset )
			bitSet.insert(convert<std::string>(idx));
	} // for-loop

    const char delim = fg_->BitDelimmitter();
    std::vector<std::string> eBits, oBits;
    std::set<std::string> eSet, oSet;
    std::vector<std::string> inter;
	switch(toCheck) {
		case ERROR: // ERROR --> if an error exists, return true
            eBits = SplitString(fg_->ErrorBits(), delim);
            eSet.insert(eBits.begin(), eBits.end());            
            std::set_intersection(eSet.begin(), eSet.end(),
                                  bitSet.begin(), bitSet.end(),
                                  std::back_inserter(inter));
			break;
		default:   // OPSCOMPLETE --> if done, then return true
            oBits = SplitString(fg_->OpCompleteBits(), delim);
            oSet.insert(oBits.begin(), oBits.end());
            std::set_intersection(oSet.begin(), oSet.end(),
                                  bitSet.begin(), bitSet.end(),
                                  std::back_inserter(inter));
	}; // Switch
	return(!inter.empty());
}

//===========
// command()
//===========
bool FunctionGenerator::command(const std::string& cmd) {
    Assert<UnexpectedState>(!locked_, Name());
    return(commandInstr(address_, cmd));
}

//==============
// Initialize()
//==============
bool FunctionGenerator::Initialize() {
    ampl_   = static_cast<SetType>(0);
    dc_     = static_cast<PercentType>(0);
    freq_   = static_cast<SetType>(0);
    offset_ = static_cast<SetType>(0);
    locked_ = false;
    return(command(fg_->Initialize()));  
}

//===========
// IsError()
//===========
bool FunctionGenerator::IsError() {
	std::string syntax = fg_->IsError();
	return(bitprocess(query(syntax), Instrument<BT>::ERROR)); 
}

//========
// Name()
//========
std::string FunctionGenerator::Name() const {
    return("Function Generator");
}

//===============
// OpsComplete()
//===============
bool FunctionGenerator::OpsComplete() {
    Assert<InstrumentError>(command(fg_->SetOpsComplete()), Name());
	std::string syntax = fg_->IsDone();
	return(bitprocess(query(syntax), Instrument<BT>::OPSCOMPLETE)); 
}

//=============
// OutputOff()
//=============
void FunctionGenerator::OutputOff() {
    if ( isOff_ )
        return;
    Assert<UnexpectedState>(command(fg_->OutputOff()), Name());
    ampl_   = static_cast<SetType>(0);
    dc_     = static_cast<PercentType>(0);
    freq_   = static_cast<SetType>(0);
    offset_ = static_cast<SetType>(0);
    isOff_  = true;
}

//=========
// query()
//=========
std::string FunctionGenerator::query(const std::string& q) {
    Assert<UnexpectedState>(!locked_, Name());
	return(Instrument<BT>::queryInstr(address_, q));
}

//=========
// Reset()
//=========
bool FunctionGenerator::Reset() {
	return(Initialize());
}

//================
// SetAmplitude()
//================
void FunctionGenerator::SetAmplitude(const ProgramTypes::SetType& value) {
    if ( ampl_ == value ) return; // already there
    IF* iptr = SingletonType<IF>::Instance();
    Assert<BadArg>(value <= static_cast<SetType>(iptr->MaxAmplitude()), Name());
    typedef UnexpectedState US;
    Assert<US>(command(fg_->SetAmplitude(convert<std::string>(value))), Name());
    ampl_ = value;
    isOff_ = false;
}

//================
// SetDutyCycle()
//================
void FunctionGenerator::SetDutyCycle(const ProgramTypes::PercentType& percent) {
    if ( dc_ == percent ) return; // already there
    IF* iptr = SingletonType<IF>::Instance();
    PercentType min = static_cast<PercentType>(iptr->MinDutyCycle());
    PercentType max = static_cast<PercentType>(iptr->MaxDutyCycle());
    Assert<BadArg>((percent <= max) && (percent >= min), Name());    
    typedef UnexpectedState US;
    Assert<US>(command(fg_->SetDutyCycle(convert<std::string>(percent))), Name());
    dc_ = percent;
    isOff_ = false;
}

//================
// SetFrequency()
//================
void FunctionGenerator::SetFrequency(const ProgramTypes::SetType& value) {
    if ( freq_ == value ) return; // already there
    IF* iptr = SingletonType<IF>::Instance();
    Assert<BadArg>(value <= static_cast<SetType>(iptr->MaxFrequency()), Name());
    typedef UnexpectedState US;
    std::string cmd = fg_->SetSquareWave();
    cmd += fg_->Concatenate();
    cmd += fg_->SetFrequency(convert<std::string>(value));
    Assert<US>(command(cmd), Name());
    freq_ = value;
    isOff_ = false;
}

//=============
// SetOffset()
//=============
void FunctionGenerator::SetOffset(const ProgramTypes::SetType& value) {
    if ( offset_ == value ) return; // already there
    IF* iptr = SingletonType<IF>::Instance();
    Assert<BadArg>(value <= static_cast<SetType>(iptr->MaxOffset()), Name());
    typedef UnexpectedState US;
    Assert<US>(command(fg_->SetOffset(convert<std::string>(value))), Name()); 
    offset_ = value;
    isOff_ = false;
}

//=============
// WhatError()
//=============
std::string FunctionGenerator::WhatError() {
    return(query(fg_->WhatError()));
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
