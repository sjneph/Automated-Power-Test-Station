// Files included
#include "Assertion.h"
#include "GenericAlgorithms.h"
#include "Oscilloscope.h"
#include "SingletonType.h"
#include "SPTSException.h"
#include "StandardStationFiles.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   05/23/05, sjn,
   ==============
     Modified bitprocess() overload --> made robust against differing size registers.

   ==============
   12/30/04, sjn,
   ==============
     Modified constructor to dynamically load correct oscilloscope.
     Modified SetHorizontalScale().  Added local var newScale, in which we set the
       precision.  Some scale arguments lose all significant digits when converted to
       a string if an explicit precision is not used.
     Removed Oscilloscope::SetGridNumber() and Oscilloscope::ScopeChannel::GridNumber
       typedef --> not generic to all scope vendors.
     Modified all Measure() overloaded member functions --> now make two calls to 
       overloaded IsError() 'oscope_' functions --> needed to account for the way
       different scope vendors deal with measurement errors.
     Re-arranged member functions to accommodate alphabetical ordering.
     Removed TriggerChannel nested class from Oscilloscope.  This modeled the external
       trigger of an oscilloscope, which we no longer use.  Removed all member funcs
       specific to external triggers.
     Added more error handling to SetTriggerX( ) member functions --> channel passed
       in must match the current trigSource_.  Some oscilloscope models require this,
       others do not.  We will enforce it from now on.  This means you must set the
       trigger source properly before calling any other SetTriggerX( ) function.
     Removed all Measure() member functions associated with direct delay measurements
       Replaced with implementation of Measurement() for TIME2LEVEL.
     Modified Initialize().  Because Reset() calls Initialize(), we must make sure that
       all ScopeChannels are fully reset too --> re-AddScopeChannel().
     Added implementations for GetHorzScale(), NumberHorizontalDivisions(), Stop()
       and Start().
     Removed trigCoupling_ and trigCouplingSet_ from nested class ScopeChannel.  Some
       oscilloscope models have trigger couplings which are specific to a given scope
       channel (the model we are using) while others have a single trigger coupling
       setting for the oscilloscope.  These two variables were used to speed up the
       instrument by keeping track of the trigger coupling.  Having these two different
       types of models breaks our code --> no more speed ups for trigger coupling.
     Modified SetVerticalScale() --> added assertions.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    typedef OscilloscopeTraits::BusType BT;
    typedef StationExceptionTypes::BaseException     StationBaseException;

    typedef StationExceptionTypes::BadArg            BadArg;
    typedef StationExceptionTypes::FileError         FileError;
    typedef StationExceptionTypes::InstrumentError   InstrumentError;
    typedef StationExceptionTypes::InstrumentSetup   InstrumentSetupError;
    typedef StationExceptionTypes::InstrumentTimeout InstrumentTimeout;
    typedef StationExceptionTypes::RescaleError      RescaleError;
    typedef StationExceptionTypes::ScopeMeasure      ScopeMeasureError;
    typedef StationExceptionTypes::UnexpectedState   UnexpectedState;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

//================================
// Start Nested ScopeChannel Type
//================================
struct Oscilloscope::ScopeChannel : public OScopeParameters {

    // Workaround for Microsoft 7.0 and 7.1 Compilers
    typedef Oscilloscope::Channel Channel;
    typedef Oscilloscope::MeasurementType MeasurementType;
    typedef Oscilloscope::CouplingType CouplingType;
    typedef Oscilloscope::MemorySetting MemorySetting;
    typedef Oscilloscope::TriggerCouplingType TriggerCouplingType;
    typedef Oscilloscope::TriggerMode TriggerMode;
    typedef Oscilloscope::SlopeType SlopeType;
    typedef Oscilloscope::ChannelMap ChannelMap;
    
    ScopeChannel(Oscilloscope* const oscope, OScopeChannels::Channel chan);  
    ProgramTypes::SetType GetMaxVertScale();
    ProgramTypes::SetType GetMinVertScale();
    ProgramTypes::SetType GetVertScale();
    std::string Name();
    void SetBandwidth(Switch state);
    void SetCoupling(CouplingType coupling);    
    void SetOffset(const ProgramTypes::SetType& offset);
    void SetTriggerCoupling(TriggerCouplingType type);
    void SetTriggerLevel(const ProgramTypes::SetType& level);
    void SetTriggerSlope(SlopeType type);
    void SetVertScale(const ProgramTypes::SetType& scale);
    void TurnOff();
    void TurnOn();

    friend struct Oscilloscope;

private:
    bool operator=(const ScopeChannel&);

    /* 
      default copy-semantics are being used for this struct because
      no deep-copy is necessary.  If you ever add a new pointer/reference
      member to this struct, then be wary...  Also using implicit destructor 
      since there is nothing to destroy
    */
    Oscilloscope* const oscope_;
    bool isOff_;
    Switch bw_;
    CouplingType coupling_;    
    SlopeType trigSlope_;
    ProgramTypes::SetType offset_;
    ProgramTypes::SetType vertScale_;
    ProgramTypes::SetType trigLevel_;
    bool couplingSet_;
    bool trigSlopeSet_;   
    bool bwSet_;
    bool offsetSet_;
    bool trigLevelSet_;
    bool chanOffOnSet_;
    OScopeChannels::Channel chan_;
    ProgramTypes::SetType maxOffset_, minOffset_;
    ProgramTypes::SetType maxVScale_, minVScale_;
    std::string name_;
};
//==============================
// End Nested ScopeChannel Type
//==============================

namespace { // unnamed
    typedef Oscilloscope::ScopeChannel SChannel;
    typedef SChannel::ChannelMap::iterator CMapIter; 
}

//====================================
// SChannel::ScopeChannel Constructor
//====================================
SChannel::ScopeChannel(Oscilloscope* const oscope, OScopeChannels::Channel chan) 
            : oscope_(oscope), isOff_(true), bw_(ON), coupling_(Oscilloscope::GND), 
              trigSlope_(NEGATIVE), offset_(0), 
              vertScale_(-1), trigLevel_(0), couplingSet_(false), 
              trigSlopeSet_(false), bwSet_(false), 
              offsetSet_(false), trigLevelSet_(false), chanOffOnSet_(false), 
              chan_(chan), name_(Name()) {

    // Grab scope limitations from instrument file
    typedef SingletonType<InstrumentFile> IF;
    typedef std::pair<InstrumentFile::MinType, InstrumentFile::MaxType> range;         
    range offset    = IF::Instance()->ScopeOffsetRange();     
    range vertScale = IF::Instance()->ScopeVertScaleRange();

    // Set up known limits
    maxOffset_    = offset.second;
    minOffset_    = offset.first;
    maxVScale_    = vertScale.second; 
    minVScale_    = vertScale.first;
}   

//=============================
// SChannel::GetMaxVertScale()
//=============================
ProgramTypes::SetType SChannel::GetMaxVertScale() {
    return(maxVScale_);
}

//===========================
// SChannel::GetMinVertScale
//===========================
ProgramTypes::SetType SChannel::GetMinVertScale() {
    return(minVScale_);
}

//==========================
// SChannel::GetVertScale()
//==========================
ProgramTypes::SetType SChannel::GetVertScale() {
    Assert<UnexpectedState>(vertScale_ > static_cast<ProgramTypes::SetType>(0), 
                            name_);
    return(vertScale_); 
}

//==================
// SChannel::Name()
//==================
std::string SChannel::Name() {
   static std::string name = oscope_->Name() + " Chan " + convert<std::string>(chan_);
   return(name);
}

//==========================
// SChannel::SetBandwidth()
//==========================
void SChannel::SetBandwidth(Switch state) { 
    if ( bwSet_ && (bw_ == state) ) // already there
        return; 
    
    oscope_->syntax_ = oscope_->scope_->SetBandwidth(chan_, state);    
    Assert<InstrumentError>(oscope_->command(), name_);
    bw_    = state;
    bwSet_ = true;
}

//=========================
// SChannel::SetCoupling()
//=========================
void SChannel::SetCoupling(CouplingType coupling) {
    if ( couplingSet_ && (coupling_ == coupling) ) // already there
        return; 
    oscope_->syntax_ = oscope_->scope_->SetCoupling(chan_, coupling);    
    Assert<InstrumentError>(oscope_->command(), name_);
    coupling_    = coupling;
    couplingSet_ = true;
}

//=======================
// SChannel::SetOffset()
//=======================
void SChannel::SetOffset(const ProgramTypes::SetType& offset) {
    if ( offsetSet_ && (offset == offset_) ) // already there
        return;
    Assert<BadArg>((offset >= minOffset_) && (offset <= maxOffset_), name_);
    oscope_->syntax_ = oscope_->scope_->SetOffset(chan_, offset);
    Assert<InstrumentError>(oscope_->command(), name_);
    offset_    = offset;
    offsetSet_ = true;
}

//================================
// SChannel::SetTriggerCoupling()
//================================
void SChannel::SetTriggerCoupling(TriggerCouplingType type) {
    oscope_->syntax_ = oscope_->scope_->SetTriggerCoupling(chan_, type);
    Assert<InstrumentError>(oscope_->command(), name_);
}

//=============================
// SChannel::SetTriggerLevel()
//=============================
void SChannel::SetTriggerLevel(const ProgramTypes::SetType& level) {
    if ( trigLevelSet_ && (trigLevel_ == level) ) // already there
        return;
    oscope_->syntax_ = oscope_->scope_->SetTriggerLevel(chan_, level);
    Assert<InstrumentError>(oscope_->command(), name_);
    trigLevel_    = level;
    trigLevelSet_ = true;
}

//=============================
// SChannel::SetTriggerSlope()
//=============================
void SChannel::SetTriggerSlope(SlopeType type) {
    if ( trigSlopeSet_ && (trigSlope_ == type) ) // already there
        return;
    oscope_->syntax_ = oscope_->scope_->SetTriggerSlope(chan_, type);
    Assert<InstrumentError>(oscope_->command(), name_);
    trigSlope_    = type;
    trigSlopeSet_ = true;
}

//==========================
// SChannel::SetVertScale()
//==========================
void SChannel::SetVertScale(const ProgramTypes::SetType& scale) {
    if ( vertScale_ == scale ) // already there
        return;
    Assert<BadArg>((scale >= minVScale_) && (scale <= maxVScale_), name_);
    oscope_->syntax_ = oscope_->scope_->SetVertScale(chan_, scale);
    Assert<InstrumentError>(oscope_->command(), name_);
    vertScale_ = scale;
}

//=====================
// SChannel::TurnOff()
//=====================
void SChannel::TurnOff() {
    if ( chanOffOnSet_ && isOff_ ) 
        return;
    oscope_->syntax_ = oscope_->scope_->ChannelState(chan_, OFF);
    Assert<InstrumentError>(oscope_->command(), name_);
    isOff_ = true;
    chanOffOnSet_ = true;
}

//====================
// SChannel::TurnOn()
//====================
void SChannel::TurnOn() {
    if ( (!isOff_) && chanOffOnSet_ ) 
        return;
    oscope_->syntax_ = oscope_->scope_->ChannelState(chan_, ON);
    Assert<InstrumentError>(oscope_->command(), name_);
    isOff_ = false;
    chanOffOnSet_ = true;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//=============
// Constructor
//=============
Oscilloscope::Oscilloscope() : locked_(true), concatenate_(false), clipping_(false),
                               channelMap_(new ChannelMap), syntax_(""), name_(Name()),
                               totalSyntax_(""), trigMode_(AUTO), 
                               trigSource_(OScopeChannels::ALL), horzScale_(-1),
                               trigModeSet_(false), stopped_(false) {
                               
    InstrumentFile* ptr = SingletonType<InstrumentFile>::Instance();
    address_ = ptr->GetAddress(InstrumentFile::OSCOPE);

    typedef OscilloscopeTraits::ScopeFactoryType SF;
    std::set<std::string> possibilities = SF::Instance()->GetAllRegistered();
    std::set<std::string>::iterator a = possibilities.begin();
    while ( a != possibilities.end() ) { // Create scope types until correct one found
        try {
            scope_.reset(SF::Instance()->CreateObject(*a));
            locked_ = false;
            syntax_ = scope_->Identify();            
            std::string model;
            try {
                ptr->SetScopeType(query());
                model = Uppercase(ptr->GetModelType(InstrumentFile::OSCOPE));
                scope_.reset(SF::Instance()->CreateObject(model));
                break;
            } catch(FileError&) { // InstrumentFile does not recognize query() value
                locked_ = true;
                syntax_ = "";
                scope_.reset(0);
            }
        } catch(...) {
            locked_ = true;
            syntax_ = "";
            scope_.reset(0);
            throw;
        }
        ++a;
    } // while
    locked_ = true;
    syntax_ = "";
    Assert<UnexpectedState>(scope_.get() != 0, name_);
}

//============
// Destructor
//============
Oscilloscope::~Oscilloscope() 
{ /* */ }

//===================
// AddScopeChannel()
//===================
void Oscilloscope::AddScopeChannel(Channel chan) {
    Assert<UnexpectedState>(locked_, name_);
    Assert<BadArg>(validChannel(chan), name_);
    Assert<BadArg>(channelMap_->find(chan) == channelMap_->end(), name_);
    bool OK = channelMap_->insert(std::make_pair(chan, ScopeChannel(this, chan))).second;
    Assert<InstrumentError>(OK, name_);
}

//==============
// bitprocess()
//==============
bool Oscilloscope::bitprocess(const std::string& errorString,
                              Instrument<BT>::Register toCheck) {
	// Convert eString to binary representation
    typedef NumberBase::Base NB;
    std::string eString = GetNumericInteger(errorString);
    Assert<BadArg>(!eString.empty(), name_);
	NumberBase conversion(eString, scope_->RegisterReturnType());
	eString = conversion.Value(NumberBase::BINARY);
    std::reverse(eString.begin(), eString.end()); // eString is backwards

	long size = static_cast<long>(eString.size());
	Assert<UnexpectedState>(size <= scope_->TotalRegisterBits(), name_);
    std::set<std::string> bitSet;
	char isset = '1';
	for ( long idx = 0; idx < size; ++idx ) {
		if ( eString.at(idx) == isset )
			bitSet.insert(convert<std::string>(idx));
	} // for-loop

    const char delim = scope_->BitDelimmitter();
    std::vector<std::string> eBits, oBits;
    std::set<std::string> eSet, oSet;
    std::vector<std::string> inter;
	switch(toCheck) {
		case ERROR: // ERROR --> if an error exists, return true
            eBits = SplitString(scope_->ErrorBits(), delim);
            eSet.insert(eBits.begin(), eBits.end());            
            std::set_intersection(eSet.begin(), eSet.end(),
                                  bitSet.begin(), bitSet.end(),
                                  std::back_inserter(inter));
			break;
		default:   // OPSCOMPLETE --> if done, then return true
            oBits = SplitString(scope_->OpCompleteBits(), delim);
            oSet.insert(oBits.begin(), oBits.end());
            std::set_intersection(oSet.begin(), oSet.end(),
                                  bitSet.begin(), bitSet.end(),
                                  std::back_inserter(inter));
	}; // Switch
	return(!inter.empty());
}

//================
// channelOnOff()
//================
void Oscilloscope::channelOnOff(Channel chan, Switch state) {
    Assert<BadArg>(validChannel(chan), name_);
    CMapIter f = channelMap_->find(chan);
    Assert<BadArg>(f != channelMap_->end(), name_);     
    state == ON ? f->second.TurnOn() : f->second.TurnOff();
}

//==============
// ChannelOff()
//==============
void Oscilloscope::ChannelOff(Channel chan) {
    channelOnOff(chan, OFF);
}

//=============
// ChannelOn()
//=============
void Oscilloscope::ChannelOn(Channel chan) {
    channelOnOff(chan, ON);
}

//===========
// command()
//===========
bool Oscilloscope::command() {
	Assert<UnexpectedState>(!locked_, name_);
	if ( ! syntax_.empty() ) {
		if ( ! totalSyntax_.empty() ) {
			totalSyntax_ += scope_->Concatenate();
			totalSyntax_ += syntax_;
		}
		else
			totalSyntax_ = syntax_;
	}

	bool result = false;
	if ( concatenate_ ) // Store syntax for later use
		result = true;
	else { // Talk to instrument
		result = (totalSyntax_.size()) ? 
                          Instrument<BT>::commandInstr(address_, totalSyntax_) : true;
		totalSyntax_ = "";
	}
	syntax_ = "";
	return(result);
}
      
//==============
// Concatenate()
//==============
void Oscilloscope::Concatenate(Switch state) {
    state == ON ? concatenate_ = true : concatenate_ = false;
    if ( ! concatenate_ )
        Assert<InstrumentError>(command(), name_);
}

//================
// GetHorzScale()
//================
ProgramTypes::SetType Oscilloscope::GetHorzScale() const {
    return(horzScale_);
}

//================
// GetVertRange()
//================
std::pair<ProgramTypes::SetType, ProgramTypes::SetType> 
                                         Oscilloscope::GetVertRange(Channel chan) {
    CMapIter f = channelMap_->find(chan);
    Assert<BadArg>(f != channelMap_->end(), name_);  
    return(std::make_pair(f->second.GetMinVertScale(), f->second.GetMaxVertScale()));
}

//================
// GetVertScale()
//================
ProgramTypes::SetType Oscilloscope::GetVertScale(Channel chan) {
    CMapIter f = channelMap_->find(chan);
    Assert<BadArg>(f != channelMap_->end(), name_);
    return(f->second.GetVertScale());
}

//=================
// ImmediateMode()
//=================
void Oscilloscope::ImmediateMode() {
    concatenate_ = false;
    syntax_      = "";
    totalSyntax_ = "";
}

//==============
// Initialize() 
//==============
bool Oscilloscope::Initialize() {
    concatenate_ = false;    
    trigModeSet_ = false;
    trigSource_  = OScopeChannels::ALL;
    totalSyntax_ = "";
    try {
        // First re-add all known channels
        locked_ = true;
        std::set<OScopeChannels::Channel> toAdd;
        ChannelMap::iterator i = channelMap_->begin();
        while ( i != channelMap_->end() ) {
            toAdd.insert(i->first);
            ++i;
        } // while

        channelMap_.reset(new ChannelMap());
        std::set<OScopeChannels::Channel>::iterator j = toAdd.begin();
        while ( j != toAdd.end() ) {
            AddScopeChannel(*j);
            ++j;
        } // while

        locked_ = false;
        syntax_ = scope_->Initialize();
        Assert<InstrumentError>(command(), name_);
        SetTriggerMode(AUTO); // workaround to set trigModeSet_

    } catch(StationBaseException& error) {
        locked_ = true; 
        throw(error);
    }
    return(true);
}

//==============
// IsClipping()
//==============
bool Oscilloscope::IsClipping() {
    if ( clipping_ ) {
        clipping_ = false;
        return(true);
    }        
    return(false);
}

//===========
// IsError()
//===========
bool Oscilloscope::IsError() {
    Assert<UnexpectedState>(!concatenate_, name_);
	syntax_ = scope_->IsError();
	return(bitprocess(query(), Instrument<BT>::ERROR)); 
}

//=====================
// Measure() Overload1
//=====================
ProgramTypes::MType Oscilloscope::Measure(MeasurementType type, Channel chan) {    
    Assert<BadArg>(validChannel(chan), Name());
    waitOnScope();    
    clipping_ = false;
    syntax_ = scope_->Measure(chan, type);
    std::string value = query();

    std::string errorStr = "";
    if ( scope_->IsError(value) )
        errorStr = value;
    else if ( IsError() )
        errorStr = WhatError();

    if ( !errorStr.empty() ) {
        if ( scope_->IsClipping(errorStr) )
            clipping_ = true;
        else
            throw(ScopeMeasureError(Name() + ": " + errorStr));
    }
    return(convert<ProgramTypes::MType>(scope_->CleanMeasure(value)));
}

//=====================
// Measure() Overload2
//=====================
ProgramTypes::MType Oscilloscope::Measure(MeasurementType type, Channel chan,
                                          ProgramTypes::SetType refLevel,
                                          SlopeType slope) {
    Assert<BadArg>(type == OScopeMeasurements::TIME2LEVEL, "Oscilloscope::Measure()");
    Assert<BadArg>(validChannel(chan), Name());
    waitOnScope();
    clipping_ = false;
    syntax_ = scope_->Measure(chan, refLevel, type, slope);
    std::string value = query();

    std::string errorStr = "";
    if ( IsError() )
        errorStr = WhatError();
    else if ( scope_->IsError(value) )
        errorStr = value;

    if ( !errorStr.empty() ) {
        if ( scope_->IsClipping(errorStr) )
            clipping_ = true;
        else
            throw(ScopeMeasureError(Name() + ": " + errorStr));
    }
    return(convert<ProgramTypes::MType>(scope_->CleanMeasure(value)));
}

//========
// Name()
//========
std::string Oscilloscope::Name() {
    static std::string name = InstrumentFile::GetName(InstrumentFile::OSCOPE);
	return(name);
}

//=============================
// NumberHorizontalDivisions()
//=============================
long Oscilloscope::NumberHorizontalDivisions() const {
    return(scope_->NumberHorizontalDivisions());
}

//===========================
// NumberVerticalDivisions()
//===========================
long Oscilloscope::NumberVerticalDivisions() const {
    return(scope_->NumberVerticalDivisions());
}

//=====================
// OperationComplete()
//=====================
bool Oscilloscope::OperationComplete() {
    Assert<UnexpectedState>(!concatenate_, Name());
    syntax_ = scope_->SetOpsComplete();
    Assert<InstrumentError>(command(), Name());
    syntax_ = scope_->IsDone();
    return(bitprocess(query(), Instrument<BT>::OPSCOMPLETE));
}

//=========
// query()
//=========
std::string Oscilloscope::query() {
    Assert<InstrumentError>(!(syntax_.empty() || concatenate_ || locked_), Name());
    std::string toQuery = syntax_;
    syntax_ = "";
	return(Instrument<BT>::queryInstr(address_, toQuery));   
}

//=============
// RescaleUp()
//=============
void Oscilloscope::RescaleUp(Channel chan) {
    typedef ProgramTypes::SetType ST;
    typedef std::vector<ST> VST;
    InstrumentFile* ifile = SingletonType<InstrumentFile>::Instance();
    VST tmp = ifile->ScopeVerticalScales();
    std::sort(tmp.begin(), tmp.end());  
    CMapIter f = channelMap_->find(chan);
    Assert<BadArg>(f != channelMap_->end(), Name());
    VST::iterator i = std::find_if(
                                   tmp.begin(), 
                                   tmp.end(), 
                                   std::bind2nd(std::greater<ST>(), 
                                   f->second.GetVertScale())
                                   );
    Assert<RescaleError>(i != tmp.end(), Name());
    SetVerticalScale(chan, *i);
}

//=========
// Reset()
//=========
bool Oscilloscope::Reset() {
    return(Initialize());
}

//=================
// SetBandwidtch()
//=================
void Oscilloscope::SetBandwidth(Channel chan, Switch state) {
    CMapIter f = channelMap_->find(chan);
    Assert<BadArg>(f != channelMap_->end(), Name());
    f->second.SetBandwidth(state);
}

//===============
// SetCoupling()
//===============
void Oscilloscope::SetCoupling(Channel chan, CouplingType type) {
    CMapIter f = channelMap_->find(chan);
    Assert<BadArg>(f != channelMap_->end(), Name());
    f->second.SetCoupling(type);
}

//=======================
// SetHorizontalMemory()
//=======================
void Oscilloscope::SetHorizontalMemory(MemorySetting setting) {
    syntax_ = scope_->SetMemory(setting);
    Assert<InstrumentError>(command(), Name());
}

//======================
// SetHorizontalScale()
//======================
void Oscilloscope::SetHorizontalScale(const ProgramTypes::SetType& scale) {
    typedef std::pair<ProgramTypes::SetType, ProgramTypes::SetType> RangeType;
    typedef SingletonType<InstrumentFile> IF;
    static const RangeType horzScale = IF::Instance()->ScopeHorzScaleRange();
    
    Assert<BadArg>((scale >= horzScale.first) && (scale <= horzScale.second), Name());    
    if ( horzScale_ == scale ) // already there
        return;
    ProgramTypes::SetType newScale(scale);
    newScale.SetPrecision(10); // arbitrary, but sufficient and necessary
    syntax_ = scope_->SetHorzScale(newScale);
    Assert<InstrumentError>(command(), Name());
    horzScale_ = scale;
}

//=============
// SetOffset()
//=============
void Oscilloscope::SetOffset(Channel chan, const ProgramTypes::SetType& offset) {
    CMapIter f = channelMap_->find(chan);
    Assert<BadArg>(f != channelMap_->end(), Name());
    f->second.SetOffset(offset);
}

//======================
// SetTriggerCoupling()
//======================
void Oscilloscope::SetTriggerCoupling(Channel chan, TriggerCouplingType type) {
    Assert<BadArg>(trigSource_ == chan, Name());
    CMapIter f = channelMap_->find(chan);
    Assert<BadArg>(f != channelMap_->end(), Name());
    f->second.SetTriggerCoupling(type);
}

//===================
// SetTriggerLevel()
//===================
void Oscilloscope::SetTriggerLevel(Channel chan, 
                                   const ProgramTypes::SetType& level) {
    Assert<BadArg>(trigSource_ == chan, Name());
    CMapIter f = channelMap_->find(chan);
    Assert<BadArg>(f != channelMap_->end(), Name());
    f->second.SetTriggerLevel(level);    
}

//==================
// SetTriggerMode()
//==================
void Oscilloscope::SetTriggerMode(TriggerMode mode) {
    if ( trigModeSet_ && (trigMode_ == mode) )
        return;
    syntax_ = scope_->SetTriggerMode(mode);
    Assert<InstrumentError>(command(), Name());
    trigMode_ = mode;
    trigModeSet_ = true;
}
    
//===================
// SetTriggerSlope()
//===================
void Oscilloscope::SetTriggerSlope(Channel chan, SlopeType type) {
    Assert<BadArg>(trigSource_ == chan, Name());
    CMapIter f = channelMap_->find(chan);
    Assert<BadArg>(f != channelMap_->end(), Name());
    f->second.SetTriggerSlope(type);
}

//====================
// SetTriggerSource()
//====================
void Oscilloscope::SetTriggerSource(Channel chan) {
    Assert<BadArg>(validChannel(chan), Name());
    if ( trigSource_ == chan )
        return; // already there
    syntax_ = scope_->SetTriggerSource(chan);
    Assert<InstrumentError>(command(), Name());
    trigSource_ = chan;
}

//====================
// SetVerticalScale()
//====================
void Oscilloscope::SetVerticalScale(Channel chan, 
                                    const ProgramTypes::SetType& scale) {
    Assert<BadArg>(validChannel(chan), Name());
    CMapIter f = channelMap_->find(chan);
    Assert<BadArg>(f != channelMap_->end(), Name());
    f->second.SetVertScale(scale);    
}

//=========
// Start()
//=========
void Oscilloscope::Start() {
    // warning: must use current trigger mode when calling for Start() syntax
    Assert<BadArg>(trigModeSet_, Name());
    syntax_ = scope_->Start(trigMode_);
    bool isStopped = stopped_;
    try {
        stopped_ = false;
        Assert<InstrumentError>(command(), Name());
    } catch(...) {
        stopped_ = isStopped;
        throw;
    }
}

//========
// Stop()
//========
void Oscilloscope::Stop() {
    Assert<UnexpectedState>(!stopped_, "Stopping a stopped scope?");
    syntax_ = scope_->Stop();
    Assert<InstrumentError>(command(), Name());
    stopped_ = true;
}

//================
// validChannel()
//================
bool Oscilloscope::validChannel(Channel chan) {
    return(chan != OScopeChannels::ALL);
}

//===============
// waitOnScope()
//===============
void Oscilloscope::waitOnScope() {
    long cntr = 0, maxLoop = 100; // arbitrary
    Assert<UnexpectedState>(!concatenate_, Name());
    syntax_ = scope_->SetOpsComplete();
    Assert<InstrumentError>(command(), Name());
    while ( ! OperationComplete() ) // wait
        Assert<InstrumentTimeout>(++cntr < maxLoop, name_);
}

//=============
// WhatError()
//=============
std::string Oscilloscope::WhatError() { 
    syntax_ = scope_->WhatError();
    return(query());    
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
