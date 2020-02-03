// Macro Guard
#ifndef SPTS_TESTSEQUENCE_H
#define SPTS_TESTSEQUENCE_H

// Files included
#include "Measurement.h"
#include "NoCopy.h"
#include "OperatorInterface.h"
#include "ProgramTypes.h"
#include "SingletonType.h"
#include "StandardFiles.h"
#include "TestStepDiagnostic.h"
#include "TestStepFacade.h"
#include "TestStepInfo.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
  ==============
  11/20/05, sjn,
  ==============
      Renamed class TestStepFacade to TestStepDiagnosticFacadeFailure.  Added new private
        member diagnosticMeasurements_ and GetPreTestDiagnosticsMeasurements() public
        member function.  It is important to note that diagnosticMeasurements_ will
        never contain diagnostic failures that are considered nonrecoverable problems
        with the DUT, which would be covered by GetPreTestDiagnosticFailure().  Instead,
        it is a new add-on which allows one to report measurement values obtained prior
        to testing - measurements that are not considered problems with the DUT and are
        for reporting purposes only.

  ==============
  05/05/05, sjn,
  ==============
      Added GetPreTestDiagnosticFailure(), IsPreTestDiagnosticFailure() and
        diagnosticFailure_.  Added fakeTest_ and #include "TestStepFacade.h".
      Added HasAnyTests().
      Changed name() and GetTests() to const member functions.
      Removing Delta Testing Capability from this software: Separate app has been made.
        Removed deltaCompareSequence_ and deltaRecordSequence_ member variables.
        Removed deltaCompare(), deltaRecord() and getDeltaCompare().

  ==============
  07/16/03, sjn,
  ==============
      Removed member variables: newSequence_, testType_, serialNumber_, dashNumber_
                                and familyNumber_.
      Removed helper member functions: void initiateTests(), void startNewSequence()
                                       and void startSameSequence().
      Changed public function void Initialize() to function void Synchronize().
      These changes help to simplify general file i/o as well as allow for extensions
        to testing (ie; allow for deviations and engineering tests).
      Added member variable: sync_
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct TestSequence : private NoCopy {
    //========================
    // Start Public Interface
    //========================
    TestStepDiagnosticFacadeFailure GetPreTestDiagnosticFailure() const;
    std::vector<TestStepDiagnostic> GetPreTestDiagnosticsMeasurements() const;
    std::vector<TestStepInfo> GetTests() const;
    bool HasAnyTests() const;
    bool IsPreTestDiagnosticFailure() const;
    void PerformSequence();
    bool SequenceStatus();
    void Synchronize();
    //======================
    // End Public Interface
    //======================

private:
    friend class SingletonType<TestSequence>;
    TestSequence();
    ~TestSequence();

private:
    typedef SPTSMeasurement::Measurement::ReturnType ReturnType;
    typedef SPTSMeasurement::Measurement::RTypeFirst RTypeFirst;
    typedef SPTSMeasurement::Measurement::RTypeSecond RTypeSecond;
    typedef SPTSMeasurement::Measurement::ReturnTypeContainer ReturnTypeContainer;
    
private:
    // Helpers
    void checkSystemErrors(const std::string& testName = std::string(""));
    void checkTestName(TestStepInfo::TSPtr tptr);
    SPTSMeasurement::Measurement* createMeasurement(TestStepInfo::TSPtr tptr);
    void doSequence();
    TestStepInfo::CondPtr getCondPointer(TestStepInfo::CondPtr cptr);
    TestStepInfo::TSPtr getTestPointer(TestStepInfo::TSPtr tptr);
    bool initialize();
    std::string name() const;
    bool nextTest(TestStepInfo::TSPtr tptr, TestStepInfo::CondPtr cptr, 
                  TestStepInfo& currentTest, SPTSMeasurement::Measurement* toMeasure);
    bool nextTest(TestStepInfo::TSPtr tptr, TestStepInfo::CondPtr cptr,
                 TestStepInfo& currentTest, ProgramTypes::MType measuredValue);
    void setPrecision(const ProgramTypes::PairMType& limits, 
                      ProgramTypes::MType& measured, TestStepInfo& currentTest);
    void setResult(const ProgramTypes::PairMType& limits,
                   const ProgramTypes::MType& measured, TestStepInfo& currentTest);
    std::pair<bool, ProgramTypes::MType> speedUp(const TestStepInfo& currentTest,
                                            SPTSMeasurement::Measurement* toMeasure);
    ProgramTypes::MType updateSpeedMap(const TestStepInfo& tsi, const ReturnType& r);
    void updateSpeedMap(const TestStepInfo& tsi, const ReturnTypeContainer& rtc);

private:
    bool stopOnFailure_;
    bool status_;
    bool sync_;
    bool diagnosticFailure_;
    long testCounter_;
    OperatorInterface* oi_;
    typedef std::vector<TestStepInfo> VecTestInfo;
    typedef std::map<ConverterOutput::Output, ProgramTypes::MType> TestResults; 
    typedef std::multimap<TestStepInfo, TestResults> SpeedMapType;
    std::auto_ptr<VecTestInfo> sequence_;
    TestStepDiagnosticFacadeFailure fakeTest_;
    std::vector<TestStepDiagnostic> diagnosticMeasurements_;
    std::auto_ptr<SpeedMapType> speedSequence_;
};

#endif // SPTS_TESTSEQUENCE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
