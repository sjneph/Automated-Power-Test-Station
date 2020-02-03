// Files included
#include "Assertion.h"
#include "GenericAlgorithms.h"
#include "ScaleUnits.h"
#include "SPTSException.h"
#include "TestFixtureFile.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   11/15/05, sjn,
   ==============
     Added implementation of GetPowerConnectionMiscLine().
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    typedef StationExceptionTypes::BadArg          BadArg;
    typedef StationExceptionTypes::FileError       FileError;
    typedef StationExceptionTypes::UnexpectedState UnexpectedState;


    std::string name() {
        return("Test Fixture File");
    }

    template <typename ErrorType>
    bool isBoolean(const std::string& s) {
        Assert<ErrorType>(! s.empty(), name());
        std::string tmp = Uppercase(s);
        if ( tmp == "TRUE" )
            return(true);
        if ( tmp == "FALSE" )
            return(false);
        throw(ErrorType(name()));
    }

    static const std::string UNDEFINED = "UNDEFINED";
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//==============
// DownLeaded()
//==============
bool TestFixtureFile::DownLeaded() {
    Assert<UnexpectedState>(!fixture_.empty(), name());    
    std::string val = tf_.GetVariableValue(fixture_, "DUT Type");
    Assert<FileError>(!val.empty(), name());

    val = Uppercase(val);
    if ( "DOWNLEADED" == val )
        return(true);
    else if ("FLATLEADED" == val )
        return(false);
    throw(FileError(name()));    
}

//================
// GetLoadsWired()
//================
std::vector<LoadTraits::Channels> TestFixtureFile::GetLoadsWired() {    
    Assert<UnexpectedState>(!fixture_.empty(), name());
    std::vector<LoadTraits::Channels> toRtn; 
    std::string val = tf_.GetVariableValue(fixture_, "Loads Wired");
    std::vector<std::string> results = SplitString(val, ',');
    Assert<FileError>(!results.empty(), name());

    for ( std::size_t i = 0; i < results.size(); ++i ) {        
        Assert<FileError>(IsInteger(results[i]), Name());
        toRtn.push_back(static_cast<LoadTraits::Channels>(convert<long>(results[i])));
    }
    return(toRtn);
}

//==============================
// GetPowerConnectionMiscLine()
//==============================
std::pair<bool, std::string>
                 TestFixtureFile::GetPowerConnectionMiscLine() {  
    std::string powerCheckMisc = tf_.GetVariableValue(fixture_, "Power Check Miscellaneous");
    if ( powerCheckMisc.empty() || Uppercase(powerCheckMisc) == UNDEFINED )
        return(std::make_pair(false, ""));
    return(std::make_pair(true, powerCheckMisc));
}

//==============
// IDResistor()
//==============
ProgramTypes::MType TestFixtureFile::IDResistor() {
    Assert<UnexpectedState>(!fixture_.empty(), name());
    std::string value = tf_.GetVariableValue(fixture_, "ID Resistor");
    Assert<FileError>(IsFloating(value), name());
    return(convert<ProgramTypes::MType>(value));
}

//=======================
// IDResistorTolerance()
//=======================
ProgramTypes::MType TestFixtureFile::IDResistorTolerance() {
    std::string value = tf_.GetVariableValue("BoxID Verifications");
    Assert<FileError>(!value.empty(), name());
    std::pair<ProgramTypes::MType, ScaleUnits<ProgramTypes::MType>::Units> p;
    p = ScaleUnits<ProgramTypes::MType>::GetUnits(value);
    ProgramTypes::MType toRtn;
    try {
        toRtn = ScaleUnits<ProgramTypes::MType>::ScaleUp(p.first, p.second);
    } catch(...) {
        throw(FileError(name()));
    }
    return(toRtn);
}

//==================
// IoutShuntValue()
//==================
ProgramTypes::SetType 
               TestFixtureFile::IoutShuntValue(LoadTraits::Channels chan) {
    Assert<UnexpectedState>(!fixture_.empty(), name());
    std::string load;
    switch(chan) {
        case LoadTraits::ONE:
            load = "Iout1";
            break;
        case LoadTraits::TWO:
            load = "Iout2";
            break;
        case LoadTraits::THREE:
            load = "Iout3";
            break;
        case LoadTraits::FOUR:
            load = "Iout4";
            break;
        case LoadTraits::FIVE:
            load = "Iout5";
            break;
        default:
            throw(BadArg(name()));
    };
    load = load + " Shunt Value";
    std::string type = tf_.GetVariableValue(fixture_, load);
    Assert<FileError>(IsFloating(type), name());
    return(convert<ProgramTypes::SetType>(type));
}

//========
// Name()
//========
std::string TestFixtureFile::Name() {
    Assert<UnexpectedState>(! fixture_.empty(), name());
    return(fixture_);
}

//============
// Revision()
//============
std::string TestFixtureFile::Revision() {
    Assert<UnexpectedState>(!fixture_.empty(), name());
    std::string rev = tf_.GetVariableValue(fixture_, "Revision");
    Assert<FileError>(! rev.empty(), name());
    return(rev);
}

//=================
// RLLIDResistor()
//=================
ProgramTypes::MType TestFixtureFile::RLLIDResistor() {
    Assert<UnexpectedState>(!fixture_.empty(), name());
    std::string value = tf_.GetVariableValue(fixture_, "RLL ID Resistor");
    Assert<FileError>(IsFloating(value), name());
    return(convert<ProgramTypes::MType>(value));
}

//==========================
// RLLIDResistorTolerance()
//==========================
ProgramTypes::MType TestFixtureFile::RLLIDResistorTolerance() {
    std::string value = tf_.GetVariableValue("ResistorBoxID Verifications");
    Assert<FileError>(!value.empty(), name());
    std::pair<ProgramTypes::MType, ScaleUnits<ProgramTypes::MType>::Units> p;
    p = ScaleUnits<ProgramTypes::MType>::GetUnits(value);
    ProgramTypes::MType toRtn;
    try {
        toRtn = ScaleUnits<ProgramTypes::MType>::ScaleUp(p.first, p.second);
    } catch(...) {
        throw(FileError(name()));
    }
    return(toRtn);
}

//==============
// SetFixture()
//==============
void TestFixtureFile::SetFixture(const std::string& fixtureName) {
    fixture_ = fixtureName;
}

//===============
// Symmetrical()
//===============
bool TestFixtureFile::Symmetrical() {
    Assert<UnexpectedState>(!fixture_.empty(), name());
    return(isBoolean<FileError>(tf_.GetVariableValue(fixture_, "Symmetrical")));
} 

//=================
// SyncOutExists()
//=================
bool TestFixtureFile::SyncOutExists() {
    Assert<UnexpectedState>(!fixture_.empty(), name());
    return(isBoolean<FileError>(tf_.GetVariableValue(fixture_, "Sync Out Pin")));
}

//=================
// VinMultiplier()
//=================
ProgramTypes::MType TestFixtureFile::VinMultiplier() {
    Assert<UnexpectedState>(!fixture_.empty(), name());
    std::string mult = tf_.GetVariableValue(fixture_, "Vin Divider Multiplier");    
    Assert<FileError>(IsFloating(mult), name());
    return(convert<ProgramTypes::MType>(mult));
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
