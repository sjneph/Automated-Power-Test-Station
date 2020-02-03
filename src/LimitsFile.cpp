// Files included
#include "Assertion.h"
#include "Converter.h"
#include "GenericAlgorithms.h"
#include "LimitsFile.h"
#include "OperatorInterface.h"
#include "SPTSException.h"
#include "StationFile.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   03/09/05, sjn,
   ==============
     Removing Delta Testing Capability from this software: Separate app has been made.
       Removed static constants DELTASTARTMARKER and DELTAENDMARKER
       Removed all nested class DeltaLimits definitions
       Removed private constructors:
           explicit LimitsFile(Int2Type<DeltaLimits::COMPARE>);
           explicit LimitsFile(Int2Type<DeltaLimits::STORE>);
       Removed auto_ptr<LF::Tests>'s - deltaCompare_ and deltaStore_
       Removed all references to delta testing from checkArguments()
       Removed typedef StationExceptionTypes::DeltaFile DeltaFile;
       Modified constructor to remove deltaCompare_ and deltaStore_

   ==============  
   07/21/04, sjn,
   ==============
     Added 'bool IsDeviationTest() const' and 'void NewWorkOrder()'
     Removed: void NewDashNumber()
              void NewTestType()
     Renamed: void NewFamilyNumber() to void Reload();
     Added new arguments required for creating a FileTypes::LimitsFileType object in
         Reload()'s implementation.
     Added #include "OperatorInterface.h" to clearly show dependency.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { // unnamed
    // Exception Types
    typedef StationExceptionTypes::BadCommand     BadCommand;
    typedef StationExceptionTypes::ContainerState ContainerState;
    typedef StationExceptionTypes::FileError      FileError;
    typedef StationExceptionTypes::NoFileInfo     NoFileInfo;
    typedef StationExceptionTypes::OutOfRange     OutOfRange;    
} // unnamed


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/                            

//=======================
// Constructor overload1
//=======================
LimitsFile::LimitsFile() : start_(0), number_(SOFTWARETESTNAME + 1), lf_(0), tests_(0),
                           stepNumber_(start_), nil_("*"), atEnd_(true), revision_("") {
    operatorInterface_ = SingletonType<OperatorInterface>::Instance();
}

//============
// Destructor
//============
LimitsFile::~LimitsFile()
{ /* */ }

//==============
// operator++()
//==============
void LimitsFile::operator++() {
    long size = static_cast<long>(tests_->size());
    if ( ++stepNumber_ == size )
        atEnd_ = true;
    else if ( stepNumber_ > size )
        throw(OutOfRange(Name()));
}

//=========
// AtEnd()
//=========
bool LimitsFile::AtEnd() {
    return(atEnd_);
}

//==================
// checkArguments()
//==================
void LimitsFile::checkArguments(const std::string&) {
    // Ensure the number of arguments is correct
    LF::Tests::iterator i = tests_->begin(), j = tests_->end();
    while ( i != j ) { 
        TestParameters tmp = SplitString(i->second, ';');
        Assert<FileError>(tmp.size() == SOFTWARETESTNAME, Name());
        ++i;
    }
}

//==============
// GetMiscDMM()
//==============
std::vector<std::string> LimitsFile::GetMiscDMM() {
    std::string str = GetTestStepParameter(MISCDMM);
    return(SplitString(str, "||"));
}

//==================
// GetMidtestMisc()
//==================
std::vector<std::string> LimitsFile::GetMidtestMisc() {
    std::string str = GetTestStepParameter(MIDTESTMISCELLANEOUS);
    return(SplitString(str, "||"));
}

//==================
// GetPretestMisc()
//==================
std::vector<std::string> LimitsFile::GetPretestMisc() {
    std::string str = GetTestStepParameter(PRETESTMISCELLANEOUS);
    return(SplitString(str, "||"));
}

//====================
// GetRevisionLevel()
//====================
std::string LimitsFile::GetRevisionLevel() {
    return(lf_->GetATPRevision());
}

//========================
// GetTestStepParameter()
//========================
std::string LimitsFile::GetTestStepParameter(TestInput which) {
    TestParameters tmp = SplitString((*tests_)[stepNumber_].second, ';');
    tmp.push_back((*tests_)[stepNumber_].first);
    if ( tmp[which] == nil_ )
        tmp[which] = "";
    return(tmp[which]);
}

//===================
// IsDeviationTest()
//===================
bool LimitsFile::IsDeviationTest() const {
    return(lf_->IsDeviationTest());
}

//========
// Name()
//========
std::string LimitsFile::Name() {
    return("Limits File");
}

//===============
// NumberTests()
//===============
std::size_t LimitsFile::NumberTests() {
    return(tests_->size());
}

//==========
// Reload()
//==========
void LimitsFile::Reload() {
    Converter* cptr = SingletonType<Converter>::Instance();
    cptr->Initialize(); // ensure DUT is ready before LimitsFile
    std::string testType = operatorInterface_->GetTestType();
    std::string wo = operatorInterface_->GetWorkOrder();
    std::string dash = cptr->DashNumber(); // includes alphas
    std::string id = operatorInterface_->GetOperatorID();
    std::string loc = SingletonType<StationFile>::Instance()->StationLocation();
    bool engTest = operatorInterface_->IsEngineeringTest();
    bool teTest = operatorInterface_->IsTestEngineeringTest();
    teTest = teTest ? teTest : operatorInterface_->IsStationDebugMode();

    try {        
        if ( operatorInterface_->IsGoldStandardTest() ) { // Gold Standard                        
            lf_.reset(new FileTypes::LimitsFileType(cptr->FamilyNumber(), true));
            tests_.reset(new LF::Tests(lf_->GetGoldTests(cptr->DashNumber(),
                                                         cptr->SerialNumber())));
        }
        else { // non-Gold Standard
            lf_.reset(new FileTypes::LimitsFileType(cptr->FamilyNumber(), wo, dash,
                                                    id, loc, teTest, engTest));
            tests_.reset(new LF::Tests(lf_->GetTests(cptr->DashNumber(), testType)));
        }
    } catch(StationExceptionTypes::FileFormatError& ffe) {
        throw(MinorExceptionTypes::NoLimitsFound(ffe.GetExceptionInfo()));
    } catch(...) {
        throw(MinorExceptionTypes::NoLimitsFound());
    }
    stepNumber_ = start_;
    atEnd_= false;
    Assert<FileError>(!tests_->empty(), Name());
    checkArguments(testType);
}

//===============
// RestartTest()
//===============
void LimitsFile::RestartSameTest() {
    stepNumber_ = start_;
    atEnd_ = false;
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
