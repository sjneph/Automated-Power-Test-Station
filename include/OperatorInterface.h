// Macro Guard
#ifndef SPTS_OPERATOR_INTERFACE_H
#define SPTS_OPERATOR_INTERFACE_H

// Files included
#include "GraphicsInterface.h"
#include "SingletonType.h"
#include "StandardFiles.h"
#include "TestStepDiagnostic.h"
#include "TestStepInfo.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   11/20/05, sjn,
   ==============
     Modified DisplayTestSequence() to accept a collection of TestStepDiagnostics in
      order to show pre-test diagnostic measurements, if applicable.  Added inclusion
      of TestStepFacade.h.  Added second overload for DisplayTestResult() taking
      TestStepDiagnostic argument.
     Modified Print() to take a string argument.  No longer going through a file for
      printing --> send directly to GUI software.

   ==============  
   03/26/05, sjn,
   ==============
     Added void ShowSoaking(bool); to support GEN-029 changes.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


// Forward declarations
struct Converter;
struct DialogBox;

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct OperatorInterface {

    //========================
    // Start Public Interface
    //========================
    void Close();
    bool DidAbort();
    bool DidClose();
    void DisplayTestResult(const TestStepDiagnostic&);
    void DisplayTestResult(TestStepInfo::TSPtr);
    void DisplayTestSequence(const std::vector<TestStepDiagnostic>&,
                             const std::vector<TestStepInfo::TSPtr>&);
    std::string GetLimitsRevision();
    std::string GetOperatorID();    
    bool GetStopOnFirstFailure();
    std::string GetTestType();
    std::string GetWorkOrder();
    bool IsEngineeringTest();
    bool IsGoldStandardTest();
    bool IsRMTest();
    bool IsStationDebugMode();
    bool IsTestEngineeringTest();
    void Print(const std::string&);
    void Reset();
    void SetRevisionLevel(const std::string&);
    void SetSequenceResult(bool);
    void ShowAtTemperature(Temperature);
    void ShowCoolingTemperature();
    void ShowSoaking(bool);
    void ShowWarmingTemperature();
    void Start();
    bool IsUIError();
    std::string WhatUIError();
    //======================
    // End Public Interface
    //======================    

private:
    friend struct Converter;
    friend struct DialogBox;
    friend class SingletonType<OperatorInterface>;

private:
    OperatorInterface();
    ~OperatorInterface();

private:
    void checkInput();
    std::string getDash();
    std::string getFamily();
    std::string getSerial();
    void initializeData();
    void showDialogInteractive(const std::string& info);
    void showDialogMessage(const std::string& message);
    void showDialogWarning(const std::string& warning);

private:
    std::string revision_;
    std::string operatorID_;
    std::string testType_;
    std::string workOrder_;
    std::string serialNumber_;
    std::string familyNumber_;
    std::string dashNumber_;
    bool stopOnFailure_;
    bool abort_, closed_, windowClosed_;
    bool debugMode_, engTest_, goldTest_, rmTest_, testEngTest_;
    std::auto_ptr<GraphicsInterface> graphics_;
};

#endif // SPTS_OPERATOR_INTERFACE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
