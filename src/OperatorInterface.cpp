// Files included
#include "Assertion.h"
#include "DoneTesting.h"
#include "Functions.h"
#include "GenericAlgorithms.h"
#include "JavaGUI.h"
#include "OperatorInterface.h"
#include "SPTSException.h"
#include "SPTSSoftware.h"
#include "StandardStationFiles.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*

	==============
	08/04/06, mrb,
	==============
		Modified W/O size to allow any WO number since rules can't be followed.

   ==============
   11/20/05, sjn,
   ==============
     Modified DisplayTestSequence() to accept a collection of TestStepDiagnostics in
      order to show pre-test diagnostic measurements, if applicable.  Added second
      overload for DisplayTestResult() taking TestStepDiagnostic argument.
     Modified Print() to take a string argument.  No longer going through a file for
      printing --> send directly to GUI software.

   ==============
   05/11/05, sjn,
   ==============
     Now allow operator id's of the form: Byyy, for digits yyy to support contract
      employees in Taiwan.

   ==============  
   03/26/05, sjn,
   ==============
     Added void ShowSoaking(bool); to support GEN-029 changes.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { // unnamed
    std::string name() {
        return("Operator Interface class");
    }
    // isBoolean<ErrorType> class
    template <typename ErrorType>
    bool isBoolean(const std::string& s) {
        Assert<ErrorType>(! s.empty(), name());
        if ( ("1" == s) || ("TRUE" == Uppercase(s)) )
            return(true);
        if ( ("0" == s) || ("FALSE" == Uppercase(s)) )
            return(false);
        throw(ErrorType(name()));
    }

    // Homemade exception tags
    struct DashNumberTag {
        static std::string Name() { 
            return("Bad Dash Number Entry");
        }
    };

    struct OperIdTag {
        static std::string Name() {
            return("Bad Operator ID Entry");
        }
    };
   
    struct SerialNumberTag {
        static std::string Name() {
            return("Bad Serial Number Entry");
        }
    };

    struct WorkOrderTag {
        static std::string Name() {
            return("Bad Work Order Tag");
        }
    };

    // GUI Type
    typedef JavaGUI GUIType;

    // Station exception types
    typedef StationExceptionTypes::BadArg       BadArg;
    typedef StationExceptionTypes::InfiniteLoop InfiniteLoop;

    // User input exception types
    typedef UserInputExceptionTypes::BaseException UIBaseException;
    typedef ExceptionTypes::SomeException<
                                          ExceptionTypes::NoErrorNumber,
                                          UIBaseException,
                                          DashNumberTag
                                         > DashNumber;

    typedef ExceptionTypes::SomeException<
                                          ExceptionTypes::NoErrorNumber,
                                          UIBaseException,
                                          OperIdTag
                                         > OperID;

    typedef ExceptionTypes::SomeException<
                                          ExceptionTypes::NoErrorNumber,
                                          UIBaseException,
                                          SerialNumberTag
                                         > SerialNumber;

    typedef ExceptionTypes::SomeException<
                                          ExceptionTypes::NoErrorNumber,
                                          UIBaseException,
                                          WorkOrderTag
                                         > WorkOrder;
} // unnamed namespace

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=============
// Constructor
//=============
OperatorInterface::OperatorInterface() : graphics_(0), windowClosed_(true) { 
    try {
        graphics_.reset(new GUIType);         
    } catch(...) {
        throw(UserInputExceptionTypes::NoScreen());
    }
} 

//============
// Destructor
//============
OperatorInterface::~OperatorInterface() 
{ /* */ }

//==============
// checkInput()
//==============
void OperatorInterface::checkInput() {
    typedef std::string::size_type size_type;
    static const size_type serialSize  = 4;
    static const size_type maxWOSize   = 13;
    static const size_type minWOSize   = 1;
    static const size_type minDashSize = 3;
    static const size_type maxDashSize = 4;
    static const size_type minOpIDSize = 4;
    static const size_type maxOpIDSize = 5;

    // If it's a non-production test, then set the (workOrder_)
    if ( IsEngineeringTest() )
        workOrder_ = "ENGINEERING";
    else if ( IsGoldStandardTest() )
        workOrder_ = "GOLD STD";
    else if ( IsTestEngineeringTest() )
        workOrder_ = "TEST ENG";
    else if ( IsStationDebugMode() )
        workOrder_ = "DEBUG";

    // Basic error handling of user input
    std::string dash = dashNumber_;
    dash = dash.substr(0, dash.find('-'));
    std::string opID = Uppercase(operatorID_);
    Assert<SerialNumber>(serialNumber_.size() == serialSize, name());    
    Assert<WorkOrder>(workOrder_.size() <= maxWOSize, name());
    Assert<WorkOrder>(workOrder_.size() >= minWOSize, name());
    Assert<DashNumber>(dash.size() >= minDashSize, name());
    Assert<DashNumber>(dash.size() <= maxDashSize, name());
    Assert<OperID>(operatorID_.size() >= minOpIDSize, name());
    Assert<OperID>(operatorID_.size() <= maxOpIDSize, name());
    Assert<OperID>(opID.find_first_not_of("0123456789AB") == std::string::npos, name());

    // Can only support one of the following at a time
    long counter = 0;
    if ( goldTest_ )
        ++counter;
    if ( engTest_ )
        ++counter;
    if ( testEngTest_ )
        ++counter;
    if ( debugMode_ )
        ++counter;
    if ( rmTest_ )
        ++counter; 
    Assert<BadArg>(counter < 2, name());
}

//=========
// Close()
//=========
void OperatorInterface::Close() {
    if ( ! windowClosed_ )
        graphics_->Close();
}

//============
// DidAbort()
//============
bool OperatorInterface::DidAbort() {
    if ( ! abort_ )
        return(graphics_->UserRequestAbort());
    return(true);
}

//============
// DidClose()
//============
bool OperatorInterface::DidClose() {    
    if ( ! closed_ )
        return(graphics_->UserRequestClosed());
    return(true);
}

//===============================
// DisplayTestResult() Overload1
//===============================
void OperatorInterface::DisplayTestResult(const TestStepDiagnostic& tptr) {
    graphics_->DisplayResults(tptr.Result(), tptr.MeasuredValue().ValueStr());
}

//===============================
// DisplayTestResult() Overload2
//===============================
void OperatorInterface::DisplayTestResult(TestStepInfo::TSPtr tptr) {
    graphics_->DisplayResults(tptr->Result(), tptr->MeasuredValue().ValueStr());
}

//=======================
// DisplayTestSequence()
//=======================
void OperatorInterface::DisplayTestSequence(
                            const std::vector<TestStepDiagnostic>& pre,
                            const std::vector<TestStepInfo::TSPtr>& sequence
                                           ) {
    // Locals    
    std::vector<std::string> toShow;
    std::string delim = "<<>>";

    std::vector<TestStepDiagnostic>::const_iterator preI = pre.begin();
    while ( preI != pre.end() ) {
        TestStepInfo::PairMType limits = preI->Limits();
        std::string toDisplay = preI->TestName();
        toDisplay += delim;
        toDisplay += limits.first.ValueStr();
        toDisplay += delim;
        toDisplay += limits.second.ValueStr();
        toDisplay += delim;
        toDisplay += preI->Units();
        toShow.push_back(toDisplay);
        ++preI;
    } // while

    std::vector<TestStepInfo::TSPtr>::const_iterator tptr = sequence.begin();
    while ( tptr != sequence.end() ) {        
        TestStepInfo::PairMType limits = (*tptr)->Limits();
        std::string toDisplay = (*tptr)->TestName();
        toDisplay += delim;
        toDisplay += limits.first.ValueStr();
        toDisplay += delim;
        toDisplay += limits.second.ValueStr();
        toDisplay += delim;
        toDisplay += (*tptr)->Units();
        toShow.push_back(toDisplay);
        ++tptr;
    }

    // Send to GUI
    graphics_->ShowTestInfo(toShow);
}

//===========
// getDash()
//===========
std::string OperatorInterface::getDash() { 
    return(dashNumber_); 
}

//=============
// getFamily()
//=============
std::string OperatorInterface::getFamily() { 
    return(familyNumber_); 
}

//=====================
// GetLimitsRevision()
//=====================
std::string OperatorInterface::GetLimitsRevision() { 
    return(revision_); 
}

//=================
// GetOperatorID()
//=================
std::string OperatorInterface::GetOperatorID() { 
    return(operatorID_); 
}

//=============
// getSerial()
//=============
std::string OperatorInterface::getSerial() { 
    return(serialNumber_); 
}

//=========================
// GetStopOnFirstFailure()
//=========================
bool OperatorInterface::GetStopOnFirstFailure() { 
    return(stopOnFailure_); 
}

//===============
// GetTestType()
//===============
std::string OperatorInterface::GetTestType() { 
    return(testType_); 
}

//================
// GetWorkOrder()
//================
std::string OperatorInterface::GetWorkOrder() { 
    return(workOrder_); 
}

//==================
// initializeData()
//==================
void OperatorInterface::initializeData() {
    // Wait for user to press the 'Test' button
    while ( ! graphics_->UserRequestTesting() ) { 
        /* wait */
        if ( graphics_->UserRequestClosed() ) {
            throw DoneTesting();            
        }
    }

    // Get user input and assign to members
    std::vector<std::string> info;
    std::string s = Uppercase(graphics_->GetTestInfo());
    info = SplitString(s, GraphicsInterface::Delimiter);
    Assert<BadArg>(static_cast<long>(info.size()) == graphics_->NumberParameters(), 
                                     name());

    revision_      = info[GraphicsInterface::LIMITSREV];
    operatorID_    = info[GraphicsInterface::OPID];
    testType_      = info[GraphicsInterface::TESTYPE];
    workOrder_     = info[GraphicsInterface::WO];
    engTest_       = isBoolean<BadArg>(info[GraphicsInterface::ENGTEST]);
    goldTest_      = isBoolean<BadArg>(info[GraphicsInterface::GOLDTEST]);
    rmTest_        = isBoolean<BadArg>(info[GraphicsInterface::RMTEST]);
    testEngTest_   = isBoolean<BadArg>(info[GraphicsInterface::TESTENGTEST]);
    debugMode_     = isBoolean<BadArg>(info[GraphicsInterface::DEBUGMODE]);
    stopOnFailure_ = isBoolean<BadArg>(info[GraphicsInterface::STOPFIRSTFAIL]);
    dashNumber_    = info[GraphicsInterface::DASH];
    std::string sn = info[GraphicsInterface::SN];
    std::string fn = info[GraphicsInterface::PN];
    Assert<BadArg>(IsInteger(sn), name());
    Assert<BadArg>(IsInteger(fn), name());
    serialNumber_  = sn;
    familyNumber_  = fn;
    abort_         = graphics_->UserRequestAbort();
    closed_        = graphics_->UserRequestClosed();

    checkInput();
}

//=====================
// IsEngineeringTest()
//=====================
bool OperatorInterface::IsEngineeringTest() {
    return(engTest_);
}

//======================
// IsGoldStandardTest()
//======================
bool OperatorInterface::IsGoldStandardTest() {
    return(goldTest_);
}

//============
// IsRMTest()
//============
bool OperatorInterface::IsRMTest() {
    return(rmTest_);
}

//======================
// IsStationDebugMode()
//======================
bool OperatorInterface::IsStationDebugMode() {
    return(debugMode_);
}

//=========================
// IsTestEngineeringTest()
//=========================
bool OperatorInterface::IsTestEngineeringTest() {
    return(testEngTest_);
}

//=============
// IsUIError()
//=============
bool OperatorInterface::IsUIError() {
    return(graphics_->IsError());
}

//=========
// Print()
//=========
void OperatorInterface::Print(const std::string& str) {
    graphics_->Print(str);
    long maxLoopCount = 100000;
    long counter = 0;
    while ( !graphics_->IsDonePrinting() )
        Assert<InfiniteLoop>(++counter < maxLoopCount, name());   
}

//=========
// Reset()
//=========
void OperatorInterface::Reset() {
    if ( windowClosed_ ) {
        // throw up screen to user     
        std::vector<std::string> info;
        info.push_back(SingletonType<StationFile>::Instance()->StationName());
        info.push_back(Software::Revision());
        graphics_->Initialize(info);
        windowClosed_ = false;        
    }
    else 
        graphics_->Reset();
    initializeData();
}

//====================
// SetRevisionLevel()
//====================
void OperatorInterface::SetRevisionLevel(const std::string& realRevision) {
    revision_ = realRevision;
}

//=====================
// SetSequenceResult()
//=====================
void OperatorInterface::SetSequenceResult(bool result) {
    graphics_->SetSequenceResult(result);
}

//=====================
// ShowAtTemperature()
//=====================
void OperatorInterface::ShowAtTemperature(Temperature temp) {
    graphics_->AtTemperature(temp);
}    

//=========================
// showDialogInteractive()
//=========================
void OperatorInterface::showDialogInteractive(const std::string& info) {
    graphics_->DisplayDialogInteractive(info);
}

//=====================
// showDialogMessage()
//=====================
void OperatorInterface::showDialogMessage(const std::string& message) {
    graphics_->DisplayDialogMessage(message);
}

//=====================
// showDialogWarning()
//=====================
void OperatorInterface::showDialogWarning(const std::string& warning) {
    graphics_->DisplayDialogWarning(warning);  
}

//==========================
// ShowCoolingTemperature()
//==========================
void OperatorInterface::ShowCoolingTemperature() {
    graphics_->RampingToTemperature(COLD);
}

//===============
// ShowSoaking()
//===============
void OperatorInterface::ShowSoaking(bool isSoaking) {
    graphics_->SoakingAtTemperature(isSoaking);
}

//==========================
// ShowWarmingTemperature()
//==========================
void OperatorInterface::ShowWarmingTemperature() {
    graphics_->RampingToTemperature(HOT);
}

//===============
// WhatUIError()
//===============
std::string OperatorInterface::WhatUIError() {
    return(graphics_->WhatError());
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
