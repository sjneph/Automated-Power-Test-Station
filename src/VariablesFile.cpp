// Files included
#include "Assertion.h"
#include "Converter.h"
#include "ConverterOutput.h"
#include "CustomTestHandler.h"
#include "Functions.h"
#include "GenericAlgorithms.h"
#include "OperatorInterface.h"
#include "ScaleUnits.h"
#include "SingletonType.h"
#include "SPTSException.h"
#include "StationFile.h"
#include "VariablesFile.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
	==============
	08/10/07, MRB,
	==============
		Added Variable InhibitLifeCycle;

	==============
	06/01/06, MRB,
	==============
		Added method SkipIinPard().	This method is designed to skip over IinPard measurement as
		a valid frequency measurement point.  If this variable is not found in the var file than
		it is automatically assumed that Iin Pard is a valid option for measuring Frequency.

	==============
    05/15/06, MRB,
	==============
		Added method UseLoadMeter().  Designed to tell system to measure current from Shunt
		or Load Meter.

   ==============  
   01/10/05, sjn,
   ==============
     Added #include CustomTestHandler.h.  Modified GetTemperature() to account for
         arbitrary test sequence name feature.
     Added local string UNDEFINED.  Now allow several Variables File variables to be
         declared as 'UNDEFINED' rather than just left blank.  This helps simplify the
         data entry system.
     Removed NewDashNumber() and NewFamilyNumber().
     Added Reload(): Assumes the jobs of NewDashNumber() and NewFamilyNumber().
                     More args when creating vf_ variables --> needed to support 
                     deviation and engineering tests.
     Added DoDUTDiagnostics() and DoDUTSanityChecks() implementations.
     Added StationFile.h.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    typedef MinorExceptionTypes::NoVariablesFound  NoVariablesFound;

    typedef StationExceptionTypes::BadArg          BadArg;
    typedef StationExceptionTypes::BadRtnValue     BadRtnValue;
    typedef StationExceptionTypes::FileError       FileError;
    typedef StationExceptionTypes::UnexpectedState UnexpectedState;

    static const std::string UNDEFINED = "UNDEFINED";
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    std::string name() {
        return("Variables File");
    }

    typedef ProgramTypes::SetType SetType;
    typedef ProgramTypes::MType   MType;

    template <typename ErrorType>
    bool isBoolean(const std::string& s) {
        Assert<ErrorType>(!s.empty(), name());
        std::string tmp = Uppercase(s);
        if ( tmp == "TRUE" )
            return(true);
        if ( tmp == "FALSE" )
            return(false);
        throw(ErrorType(name()));
    }
} // unnamed namespace

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=============
// Constructor
//=============
VariablesFile::VariablesFile() 
      : testTypesFile_(new TTF), vf_(0), locked_(true), dash_("")                            
{ /* */ }

//============
// Destructor
//============
VariablesFile::~VariablesFile() 
{ /* */ }

//====================
// DoDUTDiagnostics()
//====================
bool VariablesFile::DoDUTDiagnostics() {
    std::string diagnostics = get("Turn OFF All DUT Diagnostics");
    if ( diagnostics.empty() || (Uppercase(diagnostics) == UNDEFINED) )
        return(true);    
    return(!isBoolean<FileError>(diagnostics));
}

//=====================
// DoDUTSanityChecks()
//=====================
bool VariablesFile::DoDUTSanityChecks() {
    std::string ec = get("Turn OFF All DUT Sanity Checking");
    if ( ec.empty() || (Uppercase(ec) == UNDEFINED))
        return(true);    
    return(!isBoolean<FileError>(ec));
}

//===========
// Fixture()
//===========
std::string VariablesFile::Fixture() {
    Assert<UnexpectedState>(!locked_, name());
    std::string fixture = vf_->GetVariableValue("Fixture");
    Assert<BadRtnValue>(!fixture.empty(), name());
    return(fixture);
}

//=======
// get()
//=======
std::string VariablesFile::get(const std::string& str) {
    Assert<UnexpectedState>(!locked_, name());
    Assert<BadArg>(!str.empty(), name());
    return(vf_->GetVariableValue(str));    
}

//===================
// GetAllLoadsUsed()
//===================
std::vector<LoadTraits::Channels> 
                 VariablesFile::GetAllLoadsUsed(long numOuts) {
    Assert<UnexpectedState>(!locked_, name());
    Assert<BadArg>(numOuts > 0, name());
  
    // Grab all load channels to be used by this converter, including parallel loads
    std::vector<LoadTraits::Channels> toRtn;
    for ( long i = 0; i < numOuts; ) {
        std::string out = convert<std::string>(++i);
        std::string used = vf_->GetVariableValue("Load Channel Vout" + out);
        Assert<FileError>(IsInteger(used), name());
        toRtn.push_back(static_cast<LoadTraits::Channels>(convert<long>(used)));

        std::pair< bool, std::vector<LoadTraits::Channels> > p;
        p = GetParallelLoads(static_cast<ConverterOutput::Output>(i));
        if ( p.first ) // Parallelling loads
           std::copy(p.second.begin(), p.second.end(), std::back_inserter(toRtn));             
    }
    std::sort(toRtn.begin(), toRtn.end());
    toRtn.erase(std::unique(toRtn.begin(), toRtn.end()), toRtn.end());    
 
    Assert<FileError>(!toRtn.empty(), name());
    return(toRtn);
}

//=================
// getHighestIin()
//=================
ProgramTypes::SetType VariablesFile::getHighestIin() {
    std::string highest = get("Worst Case Iin");    
    Assert<FileError>(!highest.empty(), name());
    std::pair<SetType, ScaleUnits<SetType>::Units> p;
    SetType toRtn;
    try {            
        p = ScaleUnits<SetType>::GetUnits(highest);
        toRtn = ScaleUnits<SetType>::ScaleUp(p.first, p.second);       
    } catch(...) {
        throw(FileError(name()));
    }   
    return(toRtn);
}

//=================
// getHighestVin()
//=================
ProgramTypes::SetType VariablesFile::getHighestVin() {
    std::string highest = get("Highest Vin");    
    Assert<FileError>(!highest.empty(), name());
    std::pair<SetType, ScaleUnits<SetType>::Units> p;
    SetType toRtn;
    try {            
        p = ScaleUnits<SetType>::GetUnits(highest);
        toRtn = ScaleUnits<SetType>::ScaleUp(p.first, p.second);       
    } catch(...) {
        throw(FileError(name()));
    }   
    return(toRtn);
}

//===============
// getHighLine()
//===============
ProgramTypes::SetType VariablesFile::getHighLine() {
    std::string highest = get("High Line"); 
    Assert<FileError>(!highest.empty(), name());
    std::pair<SetType, ScaleUnits<SetType>::Units> p;
    SetType toRtn;
    try {            
        p = ScaleUnits<SetType>::GetUnits(highest);
        toRtn = ScaleUnits<SetType>::ScaleUp(p.first, p.second);       
    } catch(...) {
        throw(FileError(name()));
    }   
    return(toRtn);    
}

//============
// getIouts()
//============
ProgramTypes::MTypeContainer 
                         VariablesFile::getIouts(long numOuts) {
    Assert<UnexpectedState>(!locked_, name());
    Assert<BadArg>(numOuts > 0, name());
  
    // Grab all full load Iout values
    ProgramTypes::MTypeContainer toRtn;
    for ( long i = 0; i < numOuts; ) {
        std::string out = convert<std::string>(++i);
        std::string value = vf_->GetVariableValue("Full Load" + out);    
        Assert<FileError>(!value.empty(), name());

        std::pair<MType, ScaleUnits<MType>::Units> p;
        try {            
            p = ScaleUnits<MType>::GetUnits(value);
            MType value = ScaleUnits<MType>::ScaleUp(p.first, p.second);
            Assert<FileError>(value > MType(0), name());
            toRtn.push_back(value);
        } catch(...) {
            throw(FileError(name()));
        }                 
    }
    Assert<FileError>(!toRtn.empty(), name());
    return(toRtn);
}

//=====================
// GetJumperPullIout()
//=====================
VariablesFile::JumperPullTable 
               VariablesFile::GetJumperPullIout(ConverterOutput::Output out) const {
    Assert<UnexpectedState>(!locked_, name());
    return(vf_->GetJumperPullIout(convert<std::string>(out)));
}

//=====================
// GetJumperPullVout()
//=====================
VariablesFile::JumperPullTable 
               VariablesFile::GetJumperPullVout(ConverterOutput::Output out) const {
    Assert<UnexpectedState>(!locked_, name());
    return(vf_->GetJumperPullVout(convert<std::string>(out)));
}

//===============
// GetLoadsMap()
//===============
std::vector< std::pair<ConverterOutput::Output, LoadTraits::Channels> > 
                                      VariablesFile::GetLoadsMap(long numOuts) {
    std::vector< std::pair<ConverterOutput::Output, LoadTraits::Channels> > toRtn;
    for ( long i = 0; i < numOuts; ) {
        std::string out = convert<std::string>(++i);
        std::string used = vf_->GetVariableValue("Load Channel Vout" + out);    
        Assert<FileError>(IsInteger(used), name());
        toRtn.push_back(std::make_pair(
                          static_cast<ConverterOutput::Output>(i),
                          static_cast<LoadTraits::Channels>(convert<long>(used)))
                       );        
    }
    return(toRtn);
}

//==============
// getLowLine()
//==============
ProgramTypes::SetType VariablesFile::getLowLine() {
    std::string low = get("Low Line"); 
    Assert<FileError>(!low.empty(), name());
    std::pair<SetType, ScaleUnits<SetType>::Units> p;
    SetType toRtn;
    try {            
        p = ScaleUnits<SetType>::GetUnits(low);
        toRtn = ScaleUnits<SetType>::ScaleUp(p.first, p.second);       
    } catch(...) {
        throw(FileError(name()));
    }   
    return(toRtn);     
}

//===================
// getMaxNoLoadIin()
//===================
ProgramTypes::SetType VariablesFile::getMaxNoLoadIin() {
    std::string value = get("No Load Max Iin"); 
    Assert<FileError>(!value.empty(), name());
    std::pair<SetType, ScaleUnits<SetType>::Units> p;
    SetType toRtn;
    try {            
        p = ScaleUnits<SetType>::GetUnits(value);
        toRtn = ScaleUnits<SetType>::ScaleUp(p.first, p.second);       
    } catch(...) {
        throw(FileError(name()));
    }   
    return(toRtn);     
}

//==================
// getNominalLine()
//==================
ProgramTypes::SetType VariablesFile::getNominalLine() {
    std::string value = get("Nominal Line"); 
    Assert<FileError>(!value.empty(), name());
    std::pair<SetType, ScaleUnits<SetType>::Units> p;
    SetType toRtn;
    try {            
        p = ScaleUnits<SetType>::GetUnits(value);
        toRtn = ScaleUnits<SetType>::ScaleUp(p.first, p.second);       
    } catch(...) {
        throw(FileError(name()));
    }   
    return(toRtn); 
}
   
//====================
// getInhibitLifecycle()
//====================
long VariablesFile::getInhibitLifecycles() {    
    std::string value = get("Inhibit lifecycle");
	try{
		Assert<FileError>(!value.empty(), name());
	}catch(...){
		value = "0";
	}
    // std::string --> long --> ConverterOutput::Output
	return(convert<long>(value));
}

//====================
// getNumberOutputs()
//====================
ConverterOutput::Output VariablesFile::getNumberOutputs() {    
    std::string value = get("Total Outputs"); 
    Assert<FileError>(!value.empty(), name());
    // std::string --> long --> ConverterOutput::Output
    return(static_cast<ConverterOutput::Output>(convert<long>(value)));
}

//======================
// GetOrientationOhms()
//======================
std::pair<bool, ProgramTypes::MType> VariablesFile::GetOrientationOhms() {
    std::string ohms = get("Orientation Ohms");
    if ( ohms.empty() || (Uppercase(ohms) == UNDEFINED))
        return(std::make_pair(false, -1));
    std::pair<MType, ScaleUnits<MType>::Units> p;
    MType toRtn;
    try {            
        p = ScaleUnits<MType>::GetUnits(ohms);
        toRtn = ScaleUnits<MType>::ScaleUp(p.first, p.second);       
    } catch(...) {
        throw(FileError(name()));
    }
    return(std::make_pair(true, toRtn));
}

//====================
// GetParallelLoads()
//====================
std::pair< bool, std::vector<LoadTraits::Channels> >
            VariablesFile::GetParallelLoads(ConverterOutput::Output output) {
    Assert<UnexpectedState>(!locked_, name());
    std::string out = convert<std::string>(output);
    std::string used = vf_->GetVariableValue("Loads Paralleled" + out);
    if ( used.empty() || (Uppercase(used) == UNDEFINED)) // nada
        return(std::make_pair(false, std::vector<LoadTraits::Channels>()));

    // Local variables and typedefs
    std::vector<std::string> split = SplitString(used, ',');
    typedef std::vector<LoadTraits::Channels> LoadVec;
    typedef LoadVec::iterator iterator;
    typedef std::pair<iterator, iterator> PairLoadVec;
    LoadVec toRtn, toCheck;
    PairLoadVec findMismatch;

    // Convert string information to LoadTraits information
    std::vector<std::string>::iterator i = split.begin(), j = split.end();
    while ( i != j ) { 
        Assert<FileError>(IsInteger(*i), name());
        toRtn.push_back(static_cast<LoadTraits::Channels>(convert<long>(*i)));
        ++i;
    }

    // Ensure info gathered is valid (loads must be in numerical order)
    Assert<FileError>(toRtn.size() > static_cast<std::size_t>(1), name());
    toCheck = toRtn;
    std::sort(toCheck.begin(), toCheck.end());
    findMismatch = std::mismatch(toRtn.begin(), toRtn.end(), toCheck.begin());
    Assert<FileError>(findMismatch.first == toRtn.end(), name());
    return(std::make_pair(true, toRtn));
}

//====================
// getSyncAmplitude()
//====================
ProgramTypes::SetType VariablesFile::getSyncAmplitude() {
    std::string value = get("Sync Amplitude"); 
    // must be defined if this member function is called
    Assert<FileError>(!value.empty() && (Uppercase(value) != UNDEFINED), name());
    std::pair<SetType, ScaleUnits<SetType>::Units> p;
    SetType toRtn;
    try {            
        p = ScaleUnits<SetType>::GetUnits(value);
        toRtn = ScaleUnits<SetType>::ScaleUp(p.first, p.second);       
    } catch(...) {
        throw(FileError(name()));
    }   
    return(toRtn);    
}

//====================
// getSyncDutyCycle()
//====================
ProgramTypes::PercentType VariablesFile::getSyncDutyCycle() {
    std::string value = get("Sync Duty Cycle");
    // must be defined if this member function is called
    Assert<FileError>(!value.empty() && (Uppercase(value) != UNDEFINED), name());
    std::pair<SetType, ScaleUnits<SetType>::Units> p;
    SetType toRtn;
    try {            
        p = ScaleUnits<SetType>::GetUnits(value);
        toRtn = ScaleUnits<SetType>::ScaleUp(p.first, p.second);       
    } catch(...) {
        throw(FileError(name()));
    }
    // PercentType is range-checked to prevent errors
    return(static_cast<ProgramTypes::PercentType::ValueType>(toRtn.Value()));
}

//=================
// getSyncOffset()
//=================
ProgramTypes::SetType VariablesFile::getSyncOffset() {
    std::string value = get("Sync Offset");
    // must be defined if this member function is called
    Assert<FileError>(!value.empty() && (Uppercase(value) != UNDEFINED), name());
    std::pair<SetType, ScaleUnits<SetType>::Units> p;
    SetType toRtn;
    try {
        p = ScaleUnits<SetType>::GetUnits(value);
        toRtn = ScaleUnits<SetType>::ScaleUp(p.first, p.second);
    } catch(...) {
        throw(FileError(name()));
    }
    return(toRtn);
}

//==================
// GetTemperature()
//==================
ProgramTypes::SetType VariablesFile::GetTemperature() {
    OperatorInterface* oi = SingletonType<OperatorInterface>::Instance();
    if ( isRoomTemperature() )
        return(GetRoomTemperature().Value());

    std::string testType = "";
    CustomTestHandler cth(oi->GetTestType());
    if ( cth.IsArbitraryTestName() )
        testType = cth.GetCustomTestName();
    else
        testType = cth.GetFullTestName();

    std::string value = cth.FindTemperature(get(testType));

    // must be defined if this member function is called
    Assert<FileError>(!value.empty() && (Uppercase(value) != UNDEFINED), name());
    std::pair<SetType, ScaleUnits<SetType>::Units> p;
    SetType toRtn;
    try {
        p = ScaleUnits<SetType>::GetUnits(value);
        toRtn = ScaleUnits<SetType>::ScaleUp(p.first, p.second);
    } catch(...) {
        throw(FileError(name()));
    }
    return(toRtn);
}

//========================
// GetTemperatureOffset()
//========================
ProgramTypes::SetType 
      VariablesFile::GetTemperatureOffset(TemperatureOffsetType type) {
    Assert<UnexpectedState>(!locked_, name());
    std::string offset;
    switch(type) {
        case COLDOFFSET:
            offset = "Cold Plate Offset";
            break;
        case HOTOFFSET:
            offset = "Hot Plate Offset";
            break;
        default: // ROOMOFFSET:
            offset = "Room Plate Offset";
    };
    offset = vf_->GetVariableValue(offset);
    Assert<FileError>(!offset.empty() && (Uppercase(offset) != UNDEFINED), name());
    return(convert<ProgramTypes::SetType>(offset));
}

//============
// getVouts()
//============
ProgramTypes::MTypeContainer VariablesFile::getVouts(long numOuts) {
    Assert<UnexpectedState>(!locked_, name());
    Assert<BadArg>(numOuts > 0, name());
  
    // Grab all full load Iout values
    ProgramTypes::MTypeContainer toRtn;
    for ( long i = 0; i < numOuts; ) {
        std::string out = convert<std::string>(++i);
        std::string value = vf_->GetVariableValue("Vout" + out);    
        Assert<FileError>(! value.empty(), name());

        std::pair<MType, ScaleUnits<MType>::Units> p;
        try {            
            p = ScaleUnits<MType>::GetUnits(value);
            toRtn.push_back(ScaleUnits<MType>::ScaleUp(p.first, p.second));
        } catch(...) {
            throw(FileError(name()));
        }                 
    }
    Assert<FileError>(!toRtn.empty(), name());
    return(toRtn);
}

//===================
// IsDeviationTest()
//===================
bool VariablesFile::IsDeviationTest() const {
    return(vf_->IsDeviationTest());
}

//=======================
// isFrequencySplitter()
//=======================
bool VariablesFile::isFrequencySplitter() {
    Assert<UnexpectedState>(!locked_, name());
    return(isBoolean<FileError>(get("Frequency Splitter"))); 
}

//==================
// isGroundedSync()
//==================
bool VariablesFile::isGroundedSync() {
    Assert<UnexpectedState>(!locked_, name());
    return(isBoolean<FileError>(get("Grounded Sync")));  
}

//====================
// IsJumperPullIout()
//====================
bool VariablesFile::IsJumperPullIout() const {
    Assert<UnexpectedState>(!locked_, name());
    std::string toRtn = vf_->GetVariableValue("Jumper Pull - Iout");
    return(isBoolean<FileError>(toRtn));    
}

//====================
// IsJumperPullVout()
//====================
bool VariablesFile::IsJumperPullVout() const {
    Assert<UnexpectedState>(!locked_, name());
    std::string toRtn = vf_->GetVariableValue("Jumper Pull - Vout");
    return(isBoolean<FileError>(toRtn));  
}

//====================
// isPrimaryInhibit()
//====================
bool VariablesFile::isPrimaryInhibit() {
    Assert<UnexpectedState>(!locked_, name());
    return(isBoolean<FileError>(get("Primary Inhibit Pin"))); 
}

//=====================
// isRoomTemperature()
//=====================
bool VariablesFile::isRoomTemperature() {
    typedef SingletonType<OperatorInterface> OI;
    if ( OI::Instance()->IsGoldStandardTest() )
        return(true);

    TTF::TestTypes roomTypes = testTypesFile_->GetRoomTempTestTypes();
    std::sort(roomTypes.begin(), roomTypes.end());
    std::transform(roomTypes.begin(), roomTypes.end(), roomTypes.begin(), Uppercase);
    TTF::TestTypes::iterator start = roomTypes.begin(), end = roomTypes.end();
    std::string currentTest = Uppercase(OI::Instance()->GetTestType());
    return(std::find(start, end, currentTest) != end);
} 

//================
// isSyncOutPin()
//================
bool VariablesFile::isSyncOutPin() {
    Assert<UnexpectedState>(!locked_, name());
    return(isBoolean<FileError>(get("Sync Out Pin")));
}

//===============================
// LoadTransientTransitionTime()
//===============================
std::pair<bool, ProgramTypes::SetType> VariablesFile::LoadTransientTransitionTime() {
    Assert<UnexpectedState>(!locked_, name());
    std::string value = vf_->GetVariableValue("Transient Transition Time");
    if ( value.empty() || (value == UNDEFINED) )
        return(std::make_pair(false, -1));

    std::pair<SetType, ScaleUnits<SetType>::Units> p;
    try {
        p = ScaleUnits<SetType>::GetUnits(value);                
    } catch(...) {
        throw(FileError(name()));
    }
    return(std::make_pair(true, ScaleUnits<SetType>::ScaleUp(p.first, p.second)));
}

//============
// LoadType()
//============
LoadTraits::Types VariablesFile::LoadType() {
    Assert<UnexpectedState>(!locked_, name());
    std::string type = vf_->GetVariableValue("Load Type");
    Assert<FileError>(! type.empty(), name());
    type = Uppercase(type);
    if ( type == "ELECTRONIC" )
        return(LoadTraits::ELECTRONIC);
    else if ( type == "PASSIVE" )
        return(LoadTraits::PASSIVE); 
    
    throw(FileError(name()));
}

//=============
// MiscLines()
//=============
std::vector<std::string> VariablesFile::MiscLines() {
    Assert<UnexpectedState>(!locked_, name());
    std::string misc = vf_->GetVariableValue("Miscellaneous Lines");
    if ( misc.empty() || (Uppercase(misc) == UNDEFINED) ) // nothing
        return(std::vector<std::string>());
    std::vector<std::string> tmp = SplitString(misc, ',');
    std::sort(tmp.begin(), tmp.end());
    std::vector<std::string>::iterator i = std::unique(tmp.begin(), tmp.end());
    if ( i != tmp.end() )
        tmp.erase(i);
    return(tmp);
}

//==================
// NoWirebondPull()
//==================
std::string VariablesFile::NoWirebondPull() const {
    return("NONE");
}

//====================
// PrimaryAuxSupply()
//====================
std::pair<bool, ProgramTypes::SetType> VariablesFile::PrimaryAuxSupply() {
    Assert<UnexpectedState>(!locked_, name());
    std::string pAux = vf_->GetVariableValue("Primary APS");
    if ( pAux.empty() || (Uppercase(pAux) == UNDEFINED) )
        return(std::make_pair(false, 0));
    
    typedef ProgramTypes::SetType SetType;
    SetType toRtn;
    std::pair<SetType, ScaleUnits<SetType>::Units> p;
    try {            
        p = ScaleUnits<SetType>::GetUnits(pAux);        
        toRtn = ScaleUnits<SetType>::ScaleUp(p.first, p.second);
    } catch(...) {
        throw(FileError(name()));
    }
    if ( toRtn == static_cast<SetType>(0) )
        return(std::make_pair(false, toRtn));
    return(std::make_pair(true, toRtn));
}


//==========
// Reload()
//==========
void VariablesFile::Reload() {
    std::string famNumber = SingletonType<Converter>::Instance()->FamilyNumber();
    std::string dash = SingletonType<Converter>::Instance()->DashNumberNoAlpha();
    std::string alphas = SingletonType<Converter>::Instance()->DashNumber();
    std::string loc = SingletonType<StationFile>::Instance()->StationLocation();
    OperatorInterface* oi = SingletonType<OperatorInterface>::Instance();
    bool engTest = oi->IsEngineeringTest();
    bool teTest = oi->IsTestEngineeringTest();
    teTest = teTest ? teTest : oi->IsStationDebugMode();

    // Reset vf_ to point to variable file for famNumber
    try {
        std::string wo = oi->GetWorkOrder();
        std::string id = oi->GetOperatorID();
        
        vf_.reset(new FileTypes::VariablesFileType(famNumber, wo, alphas,
                                                   id, loc, teTest, engTest));
    } catch(StationExceptionTypes::FileFormatError& ffe) {
        throw(MinorExceptionTypes::NoVariablesFound(ffe.GetExceptionInfo()));
    } catch(...) {
        vf_.reset(0);
        throw(NoVariablesFound());
    }

    // Get variables info
    try {
//        vf_->GetVariables(dash);
        vf_->GetVariables(alphas);  // now looking for entire part number
    } catch(...) {
        throw(NoVariablesFound());
    }
    dash_ = dash;
    locked_ = false;
}

//==========
// RLoads()
//==========
VariablesFile::InnerMap VariablesFile::RLoads(ConverterOutput::Output output) {
    // Grab all RLL info for output variable and return as an InnerMap    
    Assert<UnexpectedState>(!locked_, name());
    typedef ProgramTypes::SetType SetType;
    InnerMap toRtn;
    FileTypes::VariablesFileType::RLLTable r;
    r = vf_->GetRLLTable(convert<std::string>(output));
    FileTypes::VariablesFileType::RLLTable::iterator i = r.begin(), j = r.end();
    while ( i != j ) {
        std::pair<SetType, ScaleUnits<SetType>::Units> p;
        SetType first;
        try {
            p = ScaleUnits<SetType>::GetUnits(i->first);
            first = ScaleUnits<SetType>::ScaleUp(p.first, p.second);
        } catch(...) {
            throw(FileError(name()));
        }
        std::vector<std::string> vec = SplitString(i->second, ',');
        RLLSet tmp;
        std::copy(vec.begin(), vec.end(), std::inserter(tmp, tmp.begin()));        
        toRtn.insert(std::make_pair(first, tmp));
        ++i;
    }
    return(toRtn);
}

//======================
// SecondaryAuxSupply()
//======================
std::pair<bool, ProgramTypes::SetType> VariablesFile::SecondaryAuxSupply() {
    Assert<UnexpectedState>(!locked_, name());
    std::string sAux = vf_->GetVariableValue("Secondary APS");
    if ( sAux.empty() || (Uppercase(sAux) == UNDEFINED) )
        return(std::make_pair(false, 0));
    
    typedef ProgramTypes::SetType SetType;
    SetType toRtn;
    std::pair<SetType, ScaleUnits<SetType>::Units> p;
    try {            
        p = ScaleUnits<SetType>::GetUnits(sAux);        
        toRtn = ScaleUnits<SetType>::ScaleUp(p.first, p.second);
    } catch(...) {
        throw(FileError(name()));
    }
    if ( toRtn == static_cast<SetType>(0) )
        return(std::make_pair(false, toRtn));
    return(std::make_pair(true, toRtn));
}

//=======================
// skipIinPard()
//=======================
bool VariablesFile::skipIinPard() {
    Assert<UnexpectedState>(!locked_, name());
	bool ValidState;
	try{
		ValidState = isBoolean<FileError>(get("Skip Iin Pard"));
	}catch(...){
		ValidState = false;
	}
    return(ValidState); 
}

//=================
// TODPercentage()
//=================
std::pair<bool, ProgramTypes::PercentType> VariablesFile::TODPercentage() {
    Assert<UnexpectedState>(!locked_, name());
    std::string perc = vf_->GetVariableValue("TOD Percentage");
    if ( perc.empty() || (Uppercase(perc) == UNDEFINED) )
        return(std::make_pair(false, 0));

    typedef ProgramTypes::SetType SetType;
    ProgramTypes::PercentType toRtn;
    std::pair<SetType, ScaleUnits<SetType>::Units> p;
    try {
        p = ScaleUnits<SetType>::GetUnits(perc);
        toRtn = static_cast<ProgramTypes::PercentType::ValueType>(p.first.Value());
        Assert<FileError>((p.second == ScaleUnits<SetType>::Percent) || 
                          (p.second == ScaleUnits<SetType>::NONE), name());
    } catch(...) {
        throw(FileError(name()));
    }
    return(std::make_pair(true, toRtn));
}

//=======================
// UseLoadMeter()
//=======================
bool VariablesFile::UseLoadMeter() {
    Assert<UnexpectedState>(!locked_, name());
    return(isBoolean<FileError>(get("Use Load Meter"))); 
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
