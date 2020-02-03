// Files included
#include "Assertion.h"
#include "ConfigureRelays.h"
#include "Converter.h"
#include "GenericAlgorithms.h"
#include "OperatorInterface.h"
#include "ProgramTypes.h"
#include "SingletonType.h"
#include "SPTSException.h"
#include "StandardStationFiles.h"
#include "VariablesFile.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
	==============
	08/10/07, mrb
	==============
		Added InhibitLifeCycle_ variable
	
	==============
	05/30/06, mrb
	==============
		Added SkipIinPard_ variable
	
	==============
	05/16/06, mrb
	==============
		Added useloadmeter_ variable

   ==============  
   07/21/04, sjn,
   ==============
     Changed GetInfo() to Initialize()
     Added assertion: pimpl_->dashSet_ is set in DashNumber() and DashNumberNoAlpha().
     Added assertion: pimpl_->familySet_ is set in FamilyNumber().
     Modified setInfo(): reload variable file each time setInfo() is called.
     Removed vf_ and operatorInterface_ from Converter::Pimpl --> not needed.
     Added BadCommand typedef to unnamed namespace.
     Included VariablesFile.h which was always needed.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {  
    typedef ConverterOutput::Output        Out;
    typedef ProgramTypes::MType            MType;
    typedef ProgramTypes::MTypeContainer   MContainer;
    typedef ProgramTypes::PercentType      PercentType;     
    typedef ProgramTypes::SetType          SetType;
    typedef ProgramTypes::SetTypeContainer SContainer;

    // Exception Types
    typedef MinorExceptionTypes::NoVariablesFound NoVarFile;

    typedef StationExceptionTypes::BadArg     BadArg;
    typedef StationExceptionTypes::BadCommand BadCommand;
    typedef StationExceptionTypes::FileError  FileError;
} // unnamed namespace

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

class Converter::Pimpl {
    friend struct Converter;

    Pimpl() : familySet_(false), dashSet_(false)
    { /* */ }

    std::string dash_;
    bool        dashSet_;
    std::string family_;
    bool        familySet_;
    bool        groundedSync_;
    SetType     highestIin_;
    SetType     highestVin_;  
    SetType     highLine_;
    SetType     hotOffset_;
    SetType     hotTemp_;
	long		InhibitLifeCycle_;
    MContainer  iouts_;
    bool        ioutTrim_;
	SetType     lowLine_;
	SetType     nominalLine_;
    SetType     noLoadIin_;    
    Out         outputs_;
    bool        priInhibit_;
	std::string serial_;
	bool		skipIinPard_;
	bool        splitter_;
    SetType     syncAmpl_;
    PercentType syncDC_;
    SetType     syncOff_;
	bool        syncOut_;
    SetType     temperature_;
	bool		useLoadMeter_;
    MContainer  vouts_;
    bool        voutTrim_;
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=============
// Constructor
//=============
Converter::Converter() : pimpl_(new Pimpl) { setInfo(); }  

//============
// Destructor
//============
Converter::~Converter()
{ /* */ }

//==============
// DashNumber()
//==============
std::string Converter::DashNumber() const {
    Assert<BadCommand>(pimpl_->dashSet_, name());
    return(pimpl_->dash_);
}

//=====================
// DashNumberNoAlpha()
//=====================
std::string Converter::DashNumberNoAlpha() const {
    Assert<BadCommand>(pimpl_->dashSet_, name());
    char dash = '-';
    std::string toRtn = pimpl_->dash_;
    Assert<BadArg>(toRtn.size() > std::string::size_type(0), name());
    if ( toRtn.at(0) == dash )
        toRtn = toRtn.substr(1);
    std::string::size_type found = toRtn.find(dash);
    toRtn = toRtn.substr(0, found);
    Assert<BadArg>(toRtn.find(dash) == std::string::npos, name());
    return(toRtn);
}

//======================
// EnabledInputRelays()
//======================
std::vector<ControlMatrixTraits::RelayTypes::InputRelay> 
                                                    Converter::EnabledInputRelays() {
    std::vector<ControlMatrixTraits::RelayTypes::InputRelay> toRtn;
    if ( !pimpl_->groundedSync_ ) // sync is grounded by default
        toRtn.push_back(ControlMatrixTraits::RelayTypes::SYNCIN);
    return(toRtn);
}

//=====================
// EnabledMiscRelays()
//=====================
std::vector<ControlMatrixTraits::RelayTypes::MiscRelay> Converter::EnabledMiscRelays() {
    std::vector<ControlMatrixTraits::RelayTypes::MiscRelay> toRtn;
    std::vector<std::string> tmp = 
                        SingletonType<VariablesFile>::Instance()->MiscLines();
    for ( std::string::size_type i = 0; i < tmp.size(); ++i ) 
        toRtn.push_back(ConvertToMisc(tmp[i]));
    return(toRtn);
}

//=======================
// EnabledOutputRelays()
//=======================
std::vector<ControlMatrixTraits::RelayTypes::OutputRelay> 
                                                   Converter::EnabledOutputRelays() {
    // None right now
    return(std::vector<ControlMatrixTraits::RelayTypes::OutputRelay>());
}

//================
// FamilyNumber()
//================
std::string Converter::FamilyNumber() const {
    Assert<BadCommand>(pimpl_->familySet_, name());
    return(pimpl_->family_);
}

//=====================
// FrequencySplitter()
//=====================
bool Converter::FrequencySplitter() const { 
    return(pimpl_->splitter_);
}

//================
// GroundedSync()
//================
bool Converter::GroundedSync() const { 
    return(pimpl_->groundedSync_);
}

//==============
// InhibitLifeCycle()
//==============
long Converter::InhibitLifeCycle() const {
	return(pimpl_->InhibitLifeCycle_);
}

//==============
// HasInhibit()
//==============
bool Converter::HasInhibit() const { 
    return(pimpl_->priInhibit_);
}

//==============
// HasSyncOut()
//==============
bool Converter::HasSyncOut() const { 
    return(pimpl_->syncOut_);
}

//==================
// HighestIinSeen()
//==================
const SetType& Converter::HighestIinSeen() const { 
    return(pimpl_->highestIin_);
}

//==================
// HighestVinSeen()
//==================
const SetType& Converter::HighestVinSeen() const { 
    return(pimpl_->highestVin_);
}

//============
// HighLine()
//============
const SetType& Converter::HighLine() const { 
    return(pimpl_->highLine_);
}

//==============
// Initialize()
//==============
void Converter::Initialize() {
    setInfo();
}

//========
// Iout()
//========
const ProgramTypes::SetType Converter::Iout(ConverterOutput::Output channel) const {
    ProgramTypes::MType toRtn;
    switch(channel) {
        case ConverterOutput::ONE:   toRtn = (pimpl_->iouts_)[0]; break;
        case ConverterOutput::TWO:   toRtn = (pimpl_->iouts_)[1]; break;
        case ConverterOutput::THREE: toRtn = (pimpl_->iouts_)[2]; break;
        case ConverterOutput::FOUR:  toRtn = (pimpl_->iouts_)[3]; break;
        case ConverterOutput::FIVE:  toRtn = (pimpl_->iouts_)[4]; break; 
        default:
            throw(BadArg(name()));
    };
    return(toRtn.Value());  
}

//=========
// Iouts()
//=========
const MContainer& Converter::Iouts() const { 
    return(pimpl_->iouts_);
}

//==================
// JumperPullIout()
//==================
bool Converter::JumperPullIout() const { 
    return(pimpl_->ioutTrim_);
}

//==================
// JumperPullVout()
//==================
bool Converter::JumperPullVout() const { 
    return(pimpl_->voutTrim_);
}

//===========
// LowLine()
//===========
const SetType& Converter::LowLine() const { 
    return(pimpl_->lowLine_);
}

//================
// MaxIinNoLoad()
//================
const SetType& Converter::MaxIinNoLoad() const { 
    return(pimpl_->noLoadIin_);
}

//========
// name()
//========
std::string Converter::name() const {
    return("Converter Class");
}

//===============
// NominalLine()
//===============
const SetType& Converter::NominalLine() const { 
    return(pimpl_->nominalLine_);
}

//=================
// NumberOutputs() 
//=================
ConverterOutput::Output Converter::NumberOutputs() const { 
    return(pimpl_->outputs_);
}

//===========
// Outputs()
//===========
std::vector<ConverterOutput::Output> Converter::Outputs() const {
    std::vector<ConverterOutput::Output> toRtn;
    for ( long idx = 1; idx <= pimpl_->outputs_; ++idx )
        toRtn.push_back(static_cast<ConverterOutput::Output>(idx));
    Assert<FileError>(!toRtn.empty(), name());
    return(toRtn);
}

//================
// SerialNumber()
//================
std::string Converter::SerialNumber() const { 
    return(pimpl_->serial_);
}

//===========
// setInfo()
//===========
void Converter::setInfo() {
    VariablesFile* vf = SingletonType<VariablesFile>::Instance();
    OperatorInterface* oi = SingletonType<OperatorInterface>::Instance();

    // Reload variable file information
    try {
    /*
    pimpl_->family_ and pimpl_->dash_ *must* be set to their correct
    values before vf->Reload() is called, otherwise you will get very 
    subtle behavioral problems as the VariablesFile needs a correct
    family# and dash# to load its correct values from file.
    */
        pimpl_->family_ = oi->getFamily();
        pimpl_->dash_ = oi->getDash();
        pimpl_->familySet_ = true;
        pimpl_->dashSet_ = true;
        vf->Reload();
    } catch(NoVarFile& nvf) {
        throw(nvf);
    } catch(...) {
        pimpl_->familySet_ = false;
        pimpl_->dashSet_ = false;
        throw(NoVarFile());
    }

    // Re-initialize items related to the converter
    pimpl_->groundedSync_ = vf->isGroundedSync();
    pimpl_->highestIin_   = vf->getHighestIin();
    pimpl_->highestVin_   = vf->getHighestVin();  
    pimpl_->highLine_     = vf->getHighLine();
    pimpl_->outputs_      = vf->getNumberOutputs();
    pimpl_->iouts_        = vf->getIouts(pimpl_->outputs_);
    pimpl_->ioutTrim_     = vf->IsJumperPullIout();
	pimpl_->lowLine_      = vf->getLowLine();
	pimpl_->nominalLine_  = vf->getNominalLine();
    pimpl_->noLoadIin_    = vf->getMaxNoLoadIin();    
    pimpl_->priInhibit_   = vf->isPrimaryInhibit();    
	pimpl_->serial_       = oi->getSerial();
	pimpl_->skipIinPard_  = vf->skipIinPard();
	pimpl_->splitter_     = vf->isFrequencySplitter();
    pimpl_->syncAmpl_     = vf->getSyncAmplitude();
    pimpl_->syncDC_       = vf->getSyncDutyCycle();
    pimpl_->syncOff_      = vf->getSyncOffset();
	pimpl_->syncOut_      = vf->isSyncOutPin();    
    pimpl_->vouts_        = vf->getVouts(pimpl_->outputs_);
    pimpl_->voutTrim_     = vf->IsJumperPullVout();
	pimpl_->useLoadMeter_ = vf->UseLoadMeter();
	pimpl_->InhibitLifeCycle_ = vf->getInhibitLifecycles();
}
//=====================
// SkipIinPard()
//=====================
bool Converter::SkipIinPard() const { 
    return(pimpl_->skipIinPard_);
}

//=================
// SyncAmplitude()
//=================
const ProgramTypes::SetType& Converter::SyncAmplitude() const {
    return(pimpl_->syncAmpl_);
}

//=================
// SyncDutyCycle()
//=================
const ProgramTypes::PercentType& Converter::SyncDutyCycle() const {
    return(pimpl_->syncDC_);
}

//==============
// SyncOffset()
//==============
const ProgramTypes::SetType& Converter::SyncOffset() const {
    return(pimpl_->syncOff_);
}

//=====================
// UseLoadMeter()
//=====================
bool Converter::UseLoadMeter() const { 
    return(pimpl_->useLoadMeter_);
}

//========
// Vout()
//========
const ProgramTypes::SetType Converter::Vout(ConverterOutput::Output channel) const {
    ProgramTypes::MType toRtn;
    switch(channel) {
        case ConverterOutput::ONE:   toRtn = (pimpl_->vouts_)[0]; break;
        case ConverterOutput::TWO:   toRtn = (pimpl_->vouts_)[1]; break;
        case ConverterOutput::THREE: toRtn = (pimpl_->vouts_)[2]; break;
        case ConverterOutput::FOUR:  toRtn = (pimpl_->vouts_)[3]; break;
        case ConverterOutput::FIVE:  toRtn = (pimpl_->vouts_)[4]; break; 
        default:
            throw(BadArg(name()));
    };
    return(toRtn.Value());  
}

//=========
// Vouts() 
//=========
const MContainer& Converter::Vouts() const { 
    return(pimpl_->vouts_);
}
	
/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
