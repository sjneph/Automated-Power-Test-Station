// Files included
#include "Assertion.h"
#include "ElectronicLoad.h"
#include "GenericAlgorithms.h"
#include "NumberBase.h"
#include "StandardStationFiles.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
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
    typedef StationExceptionTypes::BaseException   StationBaseException;

    typedef StationExceptionTypes::BadArg          BadArg;
    typedef StationExceptionTypes::BadCommand      BadCommand;
    typedef StationExceptionTypes::ContainerState  ContainerState;
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

//=========================================
// Start Nested ElectronicLoadChannel Type
//=========================================
class ElectronicLoad::ElectronicLoadChannel {

	// Constructors
	ElectronicLoadChannel(LoadTraits::Channels, const SetType&, const SetType&, 
                          const SetType&, const SetType&, ElectronicLoad* const);

public:
    // Microsoft 7.0 compiler workaround
    typedef ElectronicLoad::MType    MType;
    typedef ElectronicLoad::SetType  SetType;
    typedef ElectronicLoad::LoadType LoadType;
    typedef ElectronicLoad::Language Language;
    typedef ElectronicLoad::Switch   Switch;
    typedef ElectronicLoad::Channels Channels;

    friend struct ElectronicLoad;

	// Public Interface
	bool Initialize();
	bool InputOn();
	bool InputOff();
	bool IsOn() const;
	SetType LoadValue() const;
	SetType MaxAmps() const;
    SetType MaxOhms() const;
	SetType MaxVolts() const;
    MType MeasureCurrent();
    MType MeasureVoltage();
    SetType MinOhms() const;
	ElectronicLoad::Mode Mode() const;
	std::string Name() const;
	void ResetParallelLoads();
	void ResetXSTStates();
	void SetLoad(const SetType&);
	bool SetMode(LoadTraits::Modes);
	bool SetParallelLoad(const ElectronicLoadChannel&);
	void SetXSTStates(const SetType&, const SetType&, const std::string&);
private:
	void setPercentages();
	void operator=(const ElectronicLoadChannel& elc); // undefined
private:
	typedef std::map<Channels, SetType> PercentMap;
	typedef std::map<Channels, SetType> AmpsMap;
    typedef std::map< Channels, std::pair<SetType, SetType> > OhmsMap;
    typedef PercentMap VoltsMap;
private:
    /* 
      default copy-semantics are being used for this struct because
      no deep-copy is necessary.  If you ever add a new pointer/reference
      to this struct, then be wary...
    */
	LoadTraits::Channels chan_;
	const SetType maxVolts_;
	const SetType maxAmps_;
    const SetType maxOhms_;
    const SetType minOhms_;
	SetType currentValue_;
	ElectronicLoad::Mode mode_;
	bool isOn_;
	bool isParalleled_;
	bool isTransient_;
    bool modeChanged_;
	PercentMap percentages_;
	AmpsMap amps_;
    OhmsMap ohms_;
    VoltsMap volts_;
	SetType to_;
	SetType from_;
	ElectronicLoad* const loadPtr_;
    std::string name_;
}; 
//=======================================
// End Nested ElectronicLoadChannel Type
//=======================================

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { // unnamed
    typedef ElectronicLoad::ElectronicLoadChannel ELC;
}

//====================
// ELC::Constructor()
//====================
ELC::ElectronicLoadChannel(LoadTraits::Channels chan, const SetType& maxVolts, 
						   const SetType& maxAmps, const SetType& minOhms,
                           const SetType& maxOhms, ElectronicLoad* const el) 
	: chan_(chan), maxVolts_(maxVolts), maxAmps_(maxAmps), minOhms_(minOhms),
      maxOhms_(maxOhms), currentValue_(0), mode_(LoadTraits::CC), isOn_(false), 
      isParalleled_(false), isTransient_(false), percentages_(), modeChanged_(false), 
      amps_(), ohms_(), volts_(), to_(0), from_(0), loadPtr_(el), name_(Name())
	  { /* */ }

//===================
// ELC::Initialize()
//===================
bool ELC::Initialize() {
	currentValue_ = 0;
	mode_ = LoadTraits::CC;
    if ( isTransient_ ) 
        ResetXSTStates();
    modeChanged_ = false;
    isOn_ = false;

	if ( ! isParalleled_ ) 
	    loadPtr_->syntax_ = Language::InitializeChannel(chan_);
	else {  // Parallel Loads
	    PercentMap::iterator it = percentages_.begin();
		Assert<ContainerState>(it != percentages_.end(), name_);
		Channels c = it->first;
		loadPtr_->syntax_ = Language::InitializeChannel(c);
		while ( ++it != percentages_.end() ) {
			c = it->first;
			loadPtr_->syntax_ += Language::Concatenate();
			loadPtr_->syntax_ += Language::InitializeChannel(c);
		}
	}
	return(loadPtr_->command());
}

//=================
// ELC::InputOff()
//=================
bool ELC::InputOff() {
	Assert<BadCommand>(!isTransient_, name_);
	if ( isOn_ ) {
		if ( ! isParalleled_ )
		    loadPtr_->syntax_ = Language::InputOff(chan_);
		else {  // Parallel Loads
			PercentMap::iterator it = percentages_.begin();
			Assert<ContainerState>(it != percentages_.end(), name_);
			loadPtr_->syntax_ = Language::InputOff(it->first);
			while ( ++it != percentages_.end() ) {
				loadPtr_->syntax_ += Language::Concatenate();
				loadPtr_->syntax_ += Language::InputOff(it->first);
			}
		}
		Assert<UnexpectedState>(loadPtr_->command(), name_);
		isOn_ = false;
	}
	return(! isOn_);
}

//================
// ELC::InputOn()
//================
bool ELC::InputOn() {	
	Assert<BadCommand>(!isTransient_, name_);
	if ( ! isOn_ ) {
		if ( ! isParalleled_ )
		    loadPtr_->syntax_ = Language::InputOn(chan_);
		else {  // Parallel Loads
			PercentMap::iterator it = percentages_.begin();
			Assert<ContainerState>(it != percentages_.end(), name_);
			loadPtr_->syntax_ = Language::InputOn(it->first);
			while ( ++it != percentages_.end() ) {
				loadPtr_->syntax_ += Language::Concatenate();
				loadPtr_->syntax_ += Language::InputOn(it->first);
			}
		}
		Assert<UnexpectedState>(loadPtr_->command(), name_);
		isOn_ = true;
	}
	return(isOn_);
}

//=============
// ELC::IsOn()
//=============
bool ELC::IsOn() const {
	return(isOn_);
}

//==================
// ELC::LoadValue()
//==================
ELC::SetType ELC::LoadValue() const {
	Assert<BadCommand>(!(isTransient_ || modeChanged_), name_);
	return(currentValue_);
}

//================
// ELC::MaxAmps()
//================
ELC::SetType ELC::MaxAmps() const {
    if ( ! isParalleled_ )
		return(maxAmps_);

	SetType total = maxAmps_;
	AmpsMap::const_iterator start = amps_.begin();
	while ( start != amps_.end() ) {
		total += start->second;
		++start;
	}
	return(total);
}

//================
// ELC::MaxOhms()
//================
ELC::SetType ELC::MaxOhms() const {
    if ( ! isParalleled_ )
		return(maxOhms_);

	SetType total = maxOhms_;
	OhmsMap::const_iterator start = ohms_.begin();
	while ( start != ohms_.end() ) {
		total += start->second.second;
		++start;
	}
	return(total);
}

//=================
// ELC::MaxVolts()
//=================
ELC::SetType ELC::MaxVolts() const {
    if ( ! isParalleled_ )
		return(maxVolts_);

	SetType min = maxVolts_;
	VoltsMap::const_iterator start = volts_.begin();
	while ( ++start != volts_.end() ) {
		if ( start->second < min )
			min = start->second;
		++start;
	}
	return(min);
}

//=======================
// ELC::MeasureCurrent()
//=======================
ELC::MType ELC::MeasureCurrent() {
	Assert<BadCommand>(!isTransient_, name_);

    loadPtr_->syntax_ = Language::MeasureCurrent(chan_);
    ELC::MType toRtn = convert<ELC::MType>(loadPtr_->query());
    if ( ! isParalleled_ )        
        return(toRtn);

    PercentMap::iterator i = percentages_.begin(), j = percentages_.end();
    while ( ++i != j ) {
        loadPtr_->syntax_ = Language::MeasureCurrent(i->first);
        toRtn += convert<ELC::MType>(loadPtr_->query());    
    }
    return(toRtn);
}

//=======================
// ELC::MeasureVoltage()
//=======================
ELC::MType ELC::MeasureVoltage() {
    Assert<BadCommand>(!isTransient_, name_);
    
    // Parallelling has minimal effect on this measurement
    loadPtr_->syntax_ = Language::MeasureVoltage(chan_);
    ELC::MType toRtn = convert<ELC::MType>(loadPtr_->query());
    return(toRtn);
}

//================
// ELC::MinOhms()
//================
ELC::SetType ELC::MinOhms() const {
    if ( ! isParalleled_ )
		return(minOhms_);

	SetType total = minOhms_;    
	OhmsMap::const_iterator start = ohms_.begin();
	while ( start != ohms_.end() ) {
		total = (total * start->second.first) / 
                (total + start->second.first);
        ++start;
	}
	return(total);
}

//=============
// ELC::Mode()
//=============
ElectronicLoad::Mode ELC::Mode() const {
	return(mode_);
}

//=============
// ELC::Name()
//=============
std::string ELC::Name() const {
    std::string toRtn = "ElectronicLoad::ElectronicLoadChannel";  
    toRtn += convert<std::string>(chan_);
	return(toRtn);
}

//===========================
// ELC::ResetParallelLoads()
//===========================
void ELC::ResetParallelLoads() {
	percentages_.erase(percentages_.begin(), percentages_.end());
	amps_.erase(amps_.begin(), amps_.end());
    ohms_.erase(ohms_.begin(), ohms_.end());
    volts_.erase(volts_.begin(), volts_.end());    
	isParalleled_ = false;
}

//=======================
// ELC::ResetXSTStates()
//=======================
void ELC::ResetXSTStates() {
    if ( ! isTransient_ ) // nothing to reset
        return;

	if ( ! isParalleled_ )
        loadPtr_->syntax_ = Language::TransientOff(chan_);
	else {  // Parallel Loads
		PercentMap::iterator it = percentages_.begin();
		Assert<ContainerState>(it != percentages_.end(), name_);
        loadPtr_->syntax_ = Language::TransientOff(it->first);
		while ( ++it != percentages_.end() ) {
			loadPtr_->syntax_ += Language::Concatenate();
			loadPtr_->syntax_ += Language::TransientOff(it->first);
		}
	}
	Assert<UnexpectedState>(loadPtr_->command(), name_);
	isTransient_ = false;
	currentValue_ = from_;
	to_   = 0;
	from_ = 0;
}

//================
// ELC::SetLoad()
//================
void ELC::SetLoad(const SetType& value) {
	static const SetType zero = static_cast<SetType>(0);
	Assert<BadCommand>(!isTransient_, name_);
    Assert<BadArg>(!(value < zero), name_);
    if ( mode_ == LoadTraits::CC )
	    Assert<BadArg>(value <= maxAmps_, name_);

    // If you're already there, then return
	if ( (currentValue_ == value) && isOn_ && (!modeChanged_) )
		return;
	else if ( mode_ == LoadTraits::CC && (!modeChanged_) ) {
		if ( (value == currentValue_) && (value == zero) && (! isOn_) )
		    return;
	}

    if ( (value == zero) && (mode_ == LoadTraits::CC) )
        InputOff();
	else if ( ! isOn_ )
		Assert<UnexpectedState>(InputOn(), name_);

	switch(mode_) {
		case LoadTraits::CR: // CR Mode
			Assert<BadArg>(value != zero, name_); // no zero-ohm values
			if ( ! isParalleled_ ) 
			    loadPtr_->syntax_ = Language::SetOhms(chan_, value);
			else {   // parallel loads
                PercentMap::iterator it = percentages_.begin();
				Assert<ContainerState>(it != percentages_.end(), name_);

                SetType percentX = 1, finalX = 0, val;
                while ( it != percentages_.end() ) {
                    val = ((1 - it->second.Value())); // subtract from 100%
                    percentX *= val;
                    ++it;
                }

                // finalX = total resistance to multiply percentages by
                //           to achieve final targe value
                finalX = value / percentX; 
                it = percentages_.begin();
                OhmsMap::iterator jt = ohms_.begin();
                Assert<ContainerState>((percentages_.size()-1) == ohms_.size(), name_);
                val = ((1 - it->second.Value()) * finalX.Value()); // subtract from 100%
                if ( val > maxOhms_ )
                    val = maxOhms_; // best we can do
                loadPtr_->syntax_ = Language::SetOhms(it->first, val);
                while ( ++it != percentages_.end() ) {              
                    val = ((1 - it->second.Value()) * finalX.Value());
                    if ( val > MaxOhms() )
                        val = jt->second.second; // best we can do
                    ++jt;
                    loadPtr_->syntax_ += Language::Concatenate();
				    loadPtr_->syntax_ += Language::SetOhms(it->first, val);
                }
			}
			break;
		default: // LoadTraits::CC --> CC Mode
			if ( ! isParalleled_ )
			    loadPtr_->syntax_ = Language::SetAmps(chan_, value);
			else {   // parallel loads
                PercentMap::iterator it = percentages_.begin();
				Assert<ContainerState>(it != percentages_.end(), name_);
				SetType val = (it->second * value);
				loadPtr_->syntax_ = Language::SetAmps(it->first, val);
				while ( ++it != percentages_.end() ) {
					val = (it->second * value);
                    loadPtr_->syntax_ += Language::Concatenate();
				    loadPtr_->syntax_ += Language::SetAmps(it->first, val);
				}
			}
	}; // switch

	Assert<UnexpectedState>(loadPtr_->command(), name_);
	currentValue_ = value; // Store new load value

    modeChanged_ = false; // Ensure currentValue_ is not for wrong mode_
}

//================
// ELC::SetMode()
//================
bool ELC::SetMode(LoadTraits::Modes mode) {
	if ( mode == mode_ ) 
		return(true);

	switch(mode) {
		case LoadTraits::CR:
			if ( ! isParalleled_ )
			    loadPtr_->syntax_ = Language::SetCRMode(chan_);
			else {  // Loads Paralleled
                PercentMap::iterator it = percentages_.begin();
				Assert<ContainerState>(it != percentages_.end(), name_);
                loadPtr_->syntax_ = Language::SetCRMode(it->first);
				while ( ++it != percentages_.end() ) {
					loadPtr_->syntax_ += Language::Concatenate();
					loadPtr_->syntax_ += Language::SetCRMode(it->first);
				}
			}
			break;
		default: // LoadTraits::CC:
			if ( ! isParalleled_ )
			    loadPtr_->syntax_ = Language::SetCCMode(chan_);
			else {  // Loads Paralleled
                PercentMap::iterator it = percentages_.begin();
				Assert<ContainerState>(it != percentages_.end(), name_);
				loadPtr_->syntax_ = Language::SetCCMode(it->first);
				while ( ++it != percentages_.end() ) {
                    loadPtr_->syntax_ += Language::Concatenate();
					loadPtr_->syntax_ += Language::SetCCMode(it->first);
				}
			}
	}; // switch

	if ( loadPtr_->command() ) {
		mode_ = mode;
        modeChanged_ = true; // Need to call SetLoad()
		return(true);
	}
    return(false);
}

//========================
// ELC::SetParallelLoad()
//========================
bool ELC::SetParallelLoad(const ElectronicLoadChannel& el) {
	Assert<UnexpectedState>(el.chan_ != chan_, name_);
	Assert<UnexpectedState>(percentages_.find(el.chan_) == percentages_.end(), name_);

	std::pair<AmpsMap::iterator, bool> check;
    LoadTraits::Channels c = el.chan_;
    SetType s = el.maxAmps_;
	check = amps_.insert(std::make_pair(c, el.maxAmps_));    
	Assert<UnexpectedState>(check.second, name_);
    volts_.insert(std::make_pair(c, el.maxVolts_));
    ohms_.insert(std::make_pair(c, std::make_pair(el.minOhms_, el.maxOhms_)));

	setPercentages();
	isParalleled_ = true;
    return(isParalleled_);
}

//=======================
// ELC::setPercentages()
//=======================
void ELC::setPercentages() {
	Assert<UnexpectedState>(amps_.size() > 0, name_);

	percentages_.clear();
	AmpsMap::iterator start = amps_.begin(), stop = amps_.end();

	SetType total = maxAmps_;
	while ( start != stop ) {
		total += start->second;
		++start;
	}
	start = amps_.begin(); // reset

	PercentMap tmp;
	typedef PercentMap::iterator tmpIterator;
	std::pair<AmpsMap::iterator, bool> check;

	SetType diff = static_cast<SetType>(1);
	SetType one  = diff;
	SetType perc = (maxAmps_ / total);
	diff -= perc;
	while ( start != stop ) {  // Calculate approx. load percentages
		perc = start->second / total;
		diff -= perc;
		check = tmp.insert(std::make_pair(start->first, perc));
		Assert<UnexpectedState>(check.second, name_);
		++start;
	}

	// store final calculated values : diff used for roundoff error above
    check = percentages_.insert(std::make_pair(chan_, (maxAmps_ / total + diff)));
	Assert<ContainerState>(check.second, name_);
	tmpIterator tstart = tmp.begin();
	while ( tstart != tmp.end() ) {
		check = percentages_.insert(*tstart);
		Assert<ContainerState>(check.second, name_);
        ++tstart;
	}
}

//=====================
// ELC::SetXSTStates()
//=====================
void ELC::SetXSTStates(const SetType& from, const SetType& to, const std::string& slew) {
    Assert<UnexpectedState>(mode_ == LoadTraits::CC, name_);
    Assert<UnexpectedState>(InputOn(), name_);
	if ( ! isParalleled_ )
	    loadPtr_->syntax_ = Language::SetTransient(chan_, from, to, slew);
	else {  // Loads Paralleled
        PercentMap::iterator it = percentages_.begin();
		Assert<ContainerState>(it != percentages_.end(), name_);
		SetType fromVal = it->second * from;
		SetType toVal   = it->second * to;
		Channels chan   = it->first;
		loadPtr_->syntax_ = Language::SetTransient(chan, fromVal, toVal, slew);
		while ( ++it != percentages_.end() ) {
            loadPtr_->syntax_ += Language::Concatenate();
			fromVal = (it->second * from);
			toVal   = (it->second * to);
			chan    = it->first;
			loadPtr_->syntax_ += Language::SetTransient(chan, fromVal, toVal, slew);
		}
	}
	Assert<UnexpectedState>(loadPtr_->command(), name_);
	to_ = to;
	from_ = from;
	isTransient_ = true;
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//===============
// Constructor()
//===============
ElectronicLoad::ElectronicLoad() 
	: loadMap_(new LoadMap),
	  concatenate_(false),
	  locked_(true),
	  syntax_(""),
	  totalSyntax_(""),
	  lastError_(""),
	  address_(-1),
      name_(Name()) { 

    address_ = Instrument<BT>::getAddress(InstrumentFile::ELECTRONICLOAD);
}

//==============
// Destructor()
//==============
ElectronicLoad::~ElectronicLoad()
{ /* */ }

//==================
// AddLoadChannel()
//==================
bool ElectronicLoad::AddLoadChannel(LoadTraits::Channels chan, const SetType& maxAmps, 
                                    const SetType& maxVolts, const SetType& minOhms, 
                                    const SetType& maxOhms) {

	SetType zero = static_cast<SetType>(0);
	Assert<BadArg>(loadMap_->find(chan) == loadMap_->end(), name_);
	Assert<BadArg>((maxAmps > zero) && (maxVolts > zero), name_);
	return(loadMap_->insert(std::make_pair(chan, 
		ElectronicLoadChannel(chan, maxVolts, maxAmps, minOhms, maxOhms, this))).second);
}

//==============
// bitprocess()
//==============
bool ElectronicLoad::bitprocess(const std::string& errorString,
                                Instrument<BT>::Register toCheck) {
	// Convert eString to binary representation
    std::string eString = GetNumericInteger(errorString);
    Assert<BadArg>(!eString.empty(), name_);
	NumberBase conversion(eString, 
                          static_cast<NumberBase::Base>(LoadType::BitReturnTypes));
	eString = conversion.Value(NumberBase::BINARY);
    std::reverse(eString.begin(), eString.end()); // eString is backwards

	long size = static_cast<long>(eString.size());
	Assert<UnexpectedState>(size <= LoadType::TotalRegisterBits, name_);
    std::set<std::string> bitSet;
	char isset = '1';
	for ( long idx = 0; idx < size; ++idx ) {
		if ( eString.at(idx) == isset )
			bitSet.insert(convert<std::string>(idx));
	} // for-loop

    const char delim = Language::BITDELIMMITTER;
    std::vector<std::string> eBits, oBits;
    std::set<std::string> eSet, oSet;
    std::vector<std::string> inter;
	switch(toCheck) {
		case ERROR: // ERROR --> if an error exists, return true
            eBits = SplitString(Language::ErrorBits(), delim);
            eSet.insert(eBits.begin(), eBits.end());            
            std::set_intersection(eSet.begin(), eSet.end(),
                                  bitSet.begin(), bitSet.end(),
                                  std::back_inserter(inter));
			break;
		default:   // OPSCOMPLETE --> if done, then return true
            oBits = SplitString(Language::OpCompleteBits(), delim);
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
bool ElectronicLoad::command() {
	Assert<UnexpectedState>(!locked_, name_);
	if ( ! syntax_.empty() ) {
		if ( ! totalSyntax_.empty() ) {
			totalSyntax_ += Language::Concatenate();
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

//===============
// Concatenate()
//===============
void ElectronicLoad::Concatenate(Switch state) {
	switch(state) {
		case OFF:
			concatenate_ = false;
			Assert<UnexpectedState>(command(), name_);
			break;
		default: // ON
			concatenate_ = true;
	}; // Switch
}

//================
// CurrentState()
//================
Switch ElectronicLoad::CurrentState(LoadTraits::Channels channel) {
	LoadMapIterator found = loadMap_->find(channel);
	Assert<BadArg>(found != loadMap_->end(), name_);
	return(found->second.IsOn() ? ON : OFF);
}

//================
// GetLoadValue()
//================
ProgramTypes::SetType ElectronicLoad::GetLoadValue(LoadTraits::Channels channel) {
    LoadMapIterator found = loadMap_->find(channel);
	Assert<BadArg>(found != loadMap_->end(), name_);
	return(found->second.LoadValue());
}

//===========
// GetMode()
//===========
LoadTraits::Modes ElectronicLoad::GetMode(LoadTraits::Channels chan) {
	ConstLoadMapIterator found = loadMap_->find(chan);
	Assert<BadArg>(found != loadMap_->end(), name_);
	return(found->second.Mode());
}

//=================
// ImmediateMode()
//=================
void ElectronicLoad::ImmediateMode() {
    concatenate_ = false;
    syntax_      = "";
    totalSyntax_ = "";
}

//==============
// Initialize()
//==============
bool ElectronicLoad::Initialize() {
	Assert<UnexpectedState>(locked_, name_); 
	lastError_ = "";
	locked_ = false;
	concatenate_ = false;
	syntax_ = "";
	totalSyntax_ = "";

    try {
	    Concatenate(ON);
	    syntax_ = Language::Initialize();
	    Assert<InstrumentError>(command(), name_);
	    LoadMapIterator start = loadMap_->begin(), stop = loadMap_->end();
	    Assert<ContainerState>(start != stop, name_);
	    while ( start != stop ) {
		    start->second.Initialize();
		    ++start;
	    }
	    Concatenate(OFF);
    } catch(...) {
        locked_ = true;
        throw;
    }
    return(true);
}

//===========
// IsError()
//===========
bool ElectronicLoad::IsError() {
	Assert<UnexpectedState>(! concatenate_, name_);
	syntax_ = Language::IsError();
	return(bitprocess(query(), Instrument<BT>::ERROR)); 
}

//===========
// LoadOff()
//===========
bool ElectronicLoad::LoadOff(LoadTraits::Channels channel) {
	LoadMapIterator found = loadMap_->find(channel);
	Assert<BadArg>(found != loadMap_->end(), name_);
	return(found->second.InputOff());
}

//==========
// LoadOn()
//==========
bool ElectronicLoad::LoadOn(LoadTraits::Channels channel) {
	LoadMapIterator found = loadMap_->find(channel);
	Assert<BadArg>(found != loadMap_->end(), name_);
	return(found->second.InputOn());
}

//===========
// MaxAmps()
//===========
ElectronicLoad::SetType ElectronicLoad::MaxAmps(LoadTraits::Channels channel) const {
    LoadMapIterator found = loadMap_->find(channel);
    Assert<BadArg>(found != loadMap_->end());
	return(found->second.MaxAmps());
}

//============
// MaxVolts()
//============
ElectronicLoad::SetType ElectronicLoad::MaxVolts(LoadTraits::Channels channel) const {
    LoadMapIterator found = loadMap_->find(channel);
    Assert<BadArg>(found != loadMap_->end());
	return(found->second.MaxVolts());
}

//==================
// MeasureCurrent()
//==================
ElectronicLoad::MType ElectronicLoad::MeasureCurrent(LoadTraits::Channels channel) {
    LoadMapIterator found = loadMap_->find(channel);
    Assert<BadArg>(found != loadMap_->end());
    return(found->second.MeasureCurrent());
}

//==================
// MeasureVoltage()
//==================
ElectronicLoad::MType ElectronicLoad::MeasureVoltage(LoadTraits::Channels channel) {
    LoadMapIterator found = loadMap_->find(channel);
    Assert<BadArg>(found != loadMap_->end());
    return(found->second.MeasureVoltage());
}

//==============
// MinMaxOhms()
//==============
std::pair<ElectronicLoad::SetType, ElectronicLoad::SetType> 
                     ElectronicLoad::MinMaxOhms(LoadTraits::Channels channel) const {
    LoadMapIterator found = loadMap_->find(channel);
    Assert<BadArg>(found != loadMap_->end());
    return(std::make_pair(
                         (found->second).MinOhms(),
                         (found->second).MaxOhms()
                         )
          );
}

//========
// Name()
//========
std::string ElectronicLoad::Name() {    
    static std::string name = InstrumentFile::GetName(InstrumentFile::ELECTRONICLOAD);
	return(name);
}

//===============
// OpsComplete()
//===============
bool ElectronicLoad::OpsComplete() {
	Assert<UnexpectedState>(! concatenate_, name_);
    syntax_ = Language::SetOpsComplete();
    Assert<InstrumentError>(command(), name_);
	syntax_ = Language::IsDone();
	return(bitprocess(query(), Instrument<BT>::OPSCOMPLETE)); 
}

//=========
// query()
//=========
std::string ElectronicLoad::query() {
    Assert<UnexpectedState>(!(syntax_.empty() || concatenate_ || locked_), name_);
    std::string toRtn = Instrument<BT>::queryInstr(address_, syntax_);
    syntax_ = "";
    return(toRtn);
}

//=========
// Reset()
//=========
bool ElectronicLoad::Reset() {
	locked_ = true;
	return(Initialize());
}

//======================
// ResetParallelLoads()
//======================
void ElectronicLoad::ResetParallelLoads() {	
	LoadMapIterator start = loadMap_->begin(), stop = loadMap_->end();
    while ( start != stop ) {
		start->second.ResetParallelLoads();
		++start;
    }
	locked_ = true; // Can call Initialize(), Reset() and SetParallelLoads() only
}

//==================
// ResetXSTStates()
//==================
void ElectronicLoad::ResetXSTStates(LoadTraits::Channels channel) {
    LoadMapIterator found = loadMap_->find(channel);
	Assert<BadArg>(found != loadMap_->end(), name_);
	found->second.ResetXSTStates();
}

//===========
// SetLoad()
//===========
void ElectronicLoad::SetLoad(LoadTraits::Channels channel, const SetType& value) {
    LoadMapIterator found = loadMap_->find(channel);
	Assert<BadArg>(found != loadMap_->end(), name_);
	found->second.SetLoad(value);
}

//===========
// SetMode()
//===========
bool ElectronicLoad::SetMode(LoadTraits::Channels channel, LoadTraits::Modes mode) {
	LoadMapIterator found = loadMap_->find(channel);
	Assert<BadArg>(found != loadMap_->end(), name_);
	return(found->second.SetMode(mode));
}

//====================
// SetParallelLoads()
//====================
LoadTraits::Channels ElectronicLoad::SetParallelLoads(LoadTraits::Channels chA,  
												      LoadTraits::Channels chB) {
    Assert<UnexpectedState>(locked_, name_); // parallel before initialize only
    LoadMapIterator foundA = loadMap_->find(chA);
    Assert<UnexpectedState>(foundA != loadMap_->end(), name_);
	LoadMapIterator foundB = loadMap_->find(chB);
	Assert<UnexpectedState>(foundB != loadMap_->end(), name_);

	Assert<UnexpectedState>(foundA->second.SetParallelLoad(foundB->second), name_);
	loadMap_->erase(foundB);
    return(chA);
}

//================
// SetXSTStates()
//================
void ElectronicLoad::SetXSTStates(LoadTraits::Channels chan, const SetType& from, 
								  const SetType& to, const SetType& rate) {

	std::string slew = (rate == ElectronicLoad::MAXSLEW) ? Language::MaxSlew() 
		                                                 : convert<std::string>(rate);
    LoadMapIterator found = loadMap_->find(chan);
	found->second.SetXSTStates(from, to, slew);
}

//===================
// TriggerXSTEvent()
//===================
void ElectronicLoad::TriggerXSTEvent() {
	syntax_ = Language::TransientOn();
	Assert<InstrumentError>(command(), name_);
}

//=============
// WhatError()
//=============
std::string ElectronicLoad::WhatError() {
	if ( ! lastError_.empty() ) {
        std::string toRtn = lastError_;
        lastError_  = "";
		return(toRtn);
    }
	Assert<UnexpectedState>(!concatenate_, name_);
	syntax_ = Language::WhatError();
	lastError_ = query();
	return(lastError_);
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
