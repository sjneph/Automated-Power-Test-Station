// Files included
#include "Assertion.h"
#include "Converter.h"
#include "DateTime.h"
#include "Functions.h"
#include "GenericAlgorithms.h"
#include "ProgramTypes.h"
#include "ScaleUnits.h"
#include "SPTSException.h"
#include "StationFile.h"
#include "VariablesFile.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   11/20/05, sjn,
   ==============
     Removed PrintArchive().

   ==============
   07/13/05, sjn,
   ==============
     Removed IinPardScopeScale().

   ==============
   07/21/04, sjn,
   ==============
     Added LocalErrorArchive() --> Allow test sequences that result in a station error
        to be logged.
     Modified nextFileNumber() --> Allow STATIONID_NUMBER for data file numbering system
        instead of just NUMBER.  Otherwise, could lead to stations overwriting each
        others' archive files (ie; SPTS\CHARLIE stores 00022778, later SPTS\DELTA
        stores 0022778 --> overwrites first instance :: Now CHARLIE_0022778 and 
        DELTA_0022778 respectively).
     Added specificStationName() helper to support STATIONID_NUMBER changes above.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    typedef StationExceptionTypes::BadArg    BadArg;
    typedef StationExceptionTypes::FileError FileError;

    static const std::string archive = "Archive";
    static const std::string archivePath = "Archive Path";
 
    std::string specificStationName(const std::string& fullName) {
        static const std::string generalName = "SPTS";
        static const std::string::size_type npos = std::string::npos;
        std::string stationName = Uppercase(fullName);
        std::string::size_type sz = stationName.find(generalName);
        Assert<FileError>(sz != npos, "Expect SPTS[backslash]XYZ for station name XYZ");
        sz += generalName.size() + 1; // add 1 for the '\' at the end of real name
        return(stationName.substr(sz)); // ie; "Charlie", "Delta", etc.
    }
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=============
// Constructor
//=============
StationFile::StationFile() : sf_(new FileTypes::StationFileType()) 
{ /* */ }

//============
// Destructor
//============
StationFile::~StationFile()
{ /* */ }

//======================
// BackupLocalStorage()
//======================
std::string StationFile::BackupLocalStorage() {
    std::string toRtn = 
             RemoveAllWhiteSpace(sf_->GetVariableValue(archive, "Temp Arch Path"));
    Assert<FileError>(!toRtn.empty(), name());
    toRtn += nextFileNumber() + ".log";
    return(toRtn);    
}

//=================
// GetShuntValue()
//=================
ProgramTypes::MType 
                StationFile::GetShuntValue(IinDCBoard board, IinShunt whichShunt) {
    std::string toRtn;
    std::string brd;
    typedef ProgramTypes::MType MType;
    switch(board) {
        case ONE:
            brd = "Iin DC Board1";
            break;
        case TWO:
            brd = "Iin DC Board2";
            break;
        case THREE:
            brd = "Iin DC Board3";
            break;
        default:
            throw(BadArg(name()));
    };

    switch(whichShunt) {
        case BIGOHM:
            toRtn = "Big Ohm Shunt";
            break;
        case MIDOHM:
            toRtn = "Mid Ohm Shunt";
            break;
        case SMALLOHM:
            toRtn = "Small Ohm Shunt";
            break;
        default: 
            throw(BadArg(name()));
    };    
    toRtn = RemoveAllWhiteSpace(sf_->GetVariableValue(brd, toRtn));
    Assert<FileError>(!toRtn.empty(), name());
    std::pair<ProgramTypes::MType, ScaleUnits<MType>::Units> p;

    try { 
        p = ScaleUnits<MType>::GetUnits(toRtn);
    } catch(...) {
        throw(FileError(name()));
    }
    typedef ProgramTypes::MType MType;
    return(ScaleUnits<MType>::ScaleUp(p.first, p.second));
}

//===========================
// GetTemperatureTolerance()
//===========================
std::pair<ProgramTypes::MType, ProgramTypes::MType> 
               StationFile::GetTemperatureTolerance(bool initialize) {

    std::string request;
    if ( initialize ) {
        if ( SingletonType<VariablesFile>::Instance()->GetTemperature() == 
                                                      GetRoomTemperature() 
           )
            request = "Initial Room Temperature Tolerance";
        else
            request = "Initial At Temperature Tolerance";
    }
    else
        request = "Temperature Tolerance";

    typedef ProgramTypes::MType MType;
    std::string value = sf_->GetVariableValue(request);
    Assert<FileError>(!value.empty(), name());
    std::vector<std::string> split = SplitString(value, ',');
    Assert<FileError>(split.size() == std::size_t(2), name());

    std::pair<MType, MType> toRtn;
    std::pair<MType, ScaleUnits<MType>::Units> p;    
    try {
        p = ScaleUnits<MType>::GetUnits(split[0]);
        toRtn.first = ScaleUnits<MType>::ScaleUp(p.first, p.second);
        p = ScaleUnits<MType>::GetUnits(split[1]);
        toRtn.second = ScaleUnits<MType>::ScaleUp(p.first, p.second);
    } catch(...) {
        throw(FileError(name()));
    }
    return(toRtn);
}

//================
// LocalArchive()
//================
std::string StationFile::LocalArchive() {
    std::string toRtn = sf_->GetVariableValue(archive, archivePath);
    Assert<FileError>(!toRtn.empty(), name());
    toRtn += SingletonType<Converter>::Instance()->FamilyNumber();
    toRtn += ".log";
    return(toRtn);
}

//===================
// LocalEngArchive()
//===================
std::string StationFile::LocalEngArchive() {
    std::string toRtn = sf_->GetVariableValue(archive, archivePath);
    Assert<FileError>(!toRtn.empty(), name());
    toRtn += "ENG.log";
    return(toRtn);
}

//=====================
// LocalErrorArchive()
//=====================
std::string StationFile::LocalErrorArchive() {
    std::string toRtn = sf_->GetVariableValue(archive, archivePath);
    Assert<FileError>(!toRtn.empty(), name());
    toRtn += "STATIONERRORS.log";
    return(toRtn);
}

//====================
// LocalGoldArchive()
//====================
std::string StationFile::LocalGoldArchive() {
    std::string toRtn = sf_->GetVariableValue(archive, "Gold Path");
    Assert<FileError>(!toRtn.empty(), name());
    toRtn += "Gold";
    toRtn += SingletonType<Converter>::Instance()->FamilyNumber();
    toRtn += ".log";
    return(toRtn);
}

//=======================
// LocalTestEngArchive()
//=======================
std::string StationFile::LocalTestEngArchive() {
    std::string toRtn = sf_->GetVariableValue(archive, archivePath);
    Assert<FileError>(!toRtn.empty(), name());
    toRtn += "TE.log";
    return(toRtn);
}

//===================
// MaxCurrentValue()
//===================
ProgramTypes::SetType 
            StationFile::MaxCurrentValue(IinDCBoard board, IinShunt whichShunt) {

    typedef ProgramTypes::SetType SetType;
    std::string toRtn;
    std::string brd;
 
    // Convert board info
    switch(board) {
        case ONE:
            brd = "Iin DC Board1";
            break;
        case TWO:
            brd = "Iin DC Board2";
            break;
        case THREE:
            brd = "Iin DC Board3";
            break;
        default:
            throw(BadArg(name()));
    };

    // Convert shunt info
    switch(whichShunt) {
        case BIGOHM:
            toRtn = "Max Amps Big Ohm Shunt";
            break;
        case MIDOHM:
            toRtn = "Max Amps Mid Ohm Shunt";
            break;
        case SMALLOHM:
            toRtn = "Max Amps Small Ohm Shunt";
            break;
        default: 
            throw(BadArg(name()));
    };    
    toRtn = RemoveAllWhiteSpace(sf_->GetVariableValue(brd, toRtn));
    Assert<FileError>(!toRtn.empty(), name());
    std::pair<SetType, ScaleUnits<SetType>::Units> p;

    try {
        p = ScaleUnits<SetType>::GetUnits(toRtn);
    } catch(...) {
        throw(FileError(name()));
    }
    return(ScaleUnits<SetType>::ScaleUp(p.first, p.second));       
}

//========
// name()
//========
std::string StationFile::name() {
     return("Station File");
}

//===============
// NeedDegauss()
//===============
bool StationFile::NeedDegauss() {
    static const std::string first = "Degauss";
    static const std::string second = "Degauss Date";
    std::string toRtn = RemoveAllWhiteSpace(sf_->GetVariableValue(first, second));
    Assert<FileError>(!toRtn.empty(), name());
    if ( Uppercase(toRtn) != Uppercase(Date::CurrentDate()) )
        return(true);
    return(false);
}

//==================
// nextFileNumber()
//==================
std::string StationFile::nextFileNumber() {    
    static const long maximum = 99999999;
    static const std::string generalName = "SPTS";
 
    // Get number stored; get station name
    std::string toRtn = "", newValue = "";
    std::string stationName = specificStationName(StationName());
    toRtn = RemoveAllWhiteSpace(sf_->GetVariableValue("Archive", "Last File Number"));
    toRtn = Uppercase(toRtn);

    // Remove station name portion of result --> only integer left
    std::string::size_type sz = toRtn.find(stationName);
    Assert<FileError>(sz != std::string::npos, name());
    sz += stationName.size();
    toRtn = toRtn.substr(++sz); // +1 for _ character
    Assert<FileError>(IsInteger(toRtn), name());
        
    // Add 1 to current number
    long number = convert<long>(toRtn);
    long size = static_cast<long>(toRtn.size());
    newValue = convert<std::string>(number + 1);
    if ( number > maximum )
        newValue = convert<std::string>(1);
    for ( long idx = static_cast<long>(newValue.size()); idx < size; ++idx )
        newValue = "0" + newValue;
    newValue = stationName + std::string("_") + newValue;

    // Update number stored
    Assert<FileError>
                    (
                      sf_->SetVariableValue("Archive", "Last File Number", newValue), 
                      name()
                    );    
    return(newValue);
}

//==============
// OraclePath()
//==============
std::string StationFile::OraclePath() {
    std::string toRtn = 
             RemoveAllWhiteSpace(sf_->GetVariableValue("Archive", "Oracle Path"));
    Assert<FileError>(!toRtn.empty(), name());
    toRtn += nextFileNumber() + ".log";
    return(toRtn);
}

//===================
// StationLocation()
//===================
std::string StationFile::StationLocation() {
    std::string toRtn = sf_->GetVariableValue("Station Location");
    Assert<FileError>(!toRtn.empty(), name());
    return(toRtn);
}

//===============
// StationName()
//===============
std::string StationFile::StationName() {
    std::string toRtn = sf_->GetVariableValue("Station Name");
    Assert<FileError>(!toRtn.empty(), name());
    return(toRtn);
}

//===================
// StationRevision()
//===================
std::string StationFile::StationRevision() {
    std::string toRtn = sf_->GetVariableValue("Station Revision");
    Assert<FileError>(!toRtn.empty(), name());
    return(toRtn);    
}

//=================
// TempCheckFreq()
//=================
ProgramTypes::SetType StationFile::TempCheckFreq() {
    std::string toRtn = sf_->GetVariableValue("Temperature Check Frequency");

    typedef ProgramTypes::SetType SetType;
    std::pair<SetType, ScaleUnits<SetType>::Units> p;
    try {
        p = ScaleUnits<SetType>::GetUnits(toRtn);
    } catch(...) {
        throw(FileError(name()));
    }
    return(ScaleUnits<SetType>::ScaleUp(p.first, p.second));  
}

//=================
// UpdateDegauss()
//=================
void StationFile::UpdateDegauss() {
    static const std::string first = "Degauss";
    static const std::string second = "Degauss Date";
    sf_->SetVariableValue(first, second, Date::CurrentDate());      
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
