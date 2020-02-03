// Files included
#include "Assertion.h"
#include "Converter.h"
#include "CustomTestHandler.h"
#include "DataArchive.h"
#include "DateTime.h"
#include "GenericAlgorithms.h"
#include "LimitsFile.h"
#include "OperatorInterface.h"
#include "SingletonType.h"
#include "SPTSException.h"
#include "SPTSSoftware.h"
#include "StandardStationFiles.h"
#include "TestSequence.h"
#include "VariablesFile.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
  ==============
  11/20/05, sjn,
  ==============
    Renamed class TestStepFacade to TestStepDiagnosticFacadeFailure.  Modified
      addData() to allow for pre-test diagnostic measurements to be printed at the
      beginning of a test sequence, if made. 

   ==============
   05/05/05, sjn,
   ==============
     Modified addData() to allow for pre-test diagnostic failure reporting.

   ==============  
   01/10/05, sjn,
   ==============
     Added call to CustomTestHandler in addData() --> give ability for Arbitrary Test
       Sequence Names.
     Added deviation support --> prepend "DEV" to part number when applicable.
     Added #include "VariablesFile.h.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//




/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    typedef StationExceptionTypes::UnexpectedState UnexpectedState;

    TestStepInfo::TSPtr getTSPtr(TestStepInfo::TSPtr tptr) {
        return(tptr);
    }
} // unnamed namespace

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=============
// Constructor
//=============
DataArchive::DataArchive(const ProgramTypes::MType& elapsedTime) {
    addData(elapsedTime);
}

//============
// Destructor
//============
DataArchive::~DataArchive() 
{ /* */ }

//===========
// addData()
//===========
void DataArchive::addData(const ProgramTypes::MType& elapsedTime) {
    OperatorInterface* oi = SingletonType<OperatorInterface>::Instance();

    // Gather all test data
    TestSequence* sequence = SingletonType<TestSequence>::Instance();
    std::vector<TestStepInfo> vec = sequence->GetTests();
    bool diagnosticFailure = sequence->IsPreTestDiagnosticFailure();
    Assert<UnexpectedState>(sequence->HasAnyTests(), Name());
    std::vector<TestStepDiagnostic> diags =
                                   sequence->GetPreTestDiagnosticsMeasurements();

    // Reset (data_)
    data_.reset(new std::vector<std::string>);
   
    // Locals
    long counter = 0;
    char delimmitter = ',';    
    Converter* dut = SingletonType<Converter>::Instance();    
    VariablesFile* var = SingletonType<VariablesFile>::Instance();
    LimitsFile* lim = SingletonType<LimitsFile>::Instance();
    ProgramTypes::SetType temperature = var->GetTemperature();
    temperature.SetPrecision(0);

    // Convert (elapsedTime) to minutes
    ProgramTypes::MType timeSpent = elapsedTime;
    timeSpent /= 60;
    timeSpent.SetPrecision(2);

    // Deal with test names possibly being arbitrary
    std::string testName = "";
    CustomTestHandler cth(oi->GetTestType());
    if ( cth.IsArbitraryTestName() )
        testName = cth.GetArbitraryTestName();
    else
        testName = cth.GetFullTestName();

    // (header) configured per ENG-019 - 'Test Data Header'
    std::string header = dut->FamilyNumber();

    if ( oi->IsGoldStandardTest() )
        header = std::string("GLD") + header;
    if ( oi->IsEngineeringTest() )
        header = std::string("ENG") + header;
    else if ( oi->IsRMTest() )
        header = std::string("RM") + header;
    else if ( oi->IsTestEngineeringTest() )
        header = std::string("TE") + header;
    else if ( oi->IsStationDebugMode() )
        header = std::string("DBG") + header;

    if ( lim->IsDeviationTest() || var->IsDeviationTest() ) {
        // always prepend on deviation; even if ENG test too
        header = std::string("DEV") + header;
    }

    header += "-";
    header += dut->DashNumber();
    header += delimmitter;
    header += oi->GetWorkOrder();
    header += delimmitter;
    header += dut->SerialNumber();
    header += delimmitter;
    header += testName;
    header += delimmitter;
    header += temperature.ValueStr();
    header += delimmitter;
    header += Date::CurrentDate();
    header += delimmitter;
    header += Clock::CurrentTime();
    header += delimmitter;
    header += timeSpent.ValueStr();
    header += delimmitter;
    header += (sequence->SequenceStatus() ? "P" : "F");
    header += delimmitter;
    header += oi->GetOperatorID();
    header += delimmitter;
    header += convert<std::string>(static_cast<long>(vec.size()));
    header += delimmitter;
    header += SingletonType<StationFile>::Instance()->StationName();
    header += delimmitter;
    header += SingletonType<StationFile>::Instance()->StationRevision();
    header += delimmitter;
    header += SingletonType<TestFixtureFile>::Instance()->Name();
    header += delimmitter;
    header += SingletonType<TestFixtureFile>::Instance()->Revision();
    header += delimmitter;
    header += Software::Name();
    header += delimmitter;
    header += Software::Revision();
    header += delimmitter;
    header += lim->GetRevisionLevel();
    data_->push_back(header);

    // Push back all test results - configured per ENG-019 'Test Data Lines'
    //
    // Precision not reported for historical (ie; 50000-600) reasons.
    //   If used in the future, the following will do:
    //     convert<std::string>(tptr->MeasuredValue().GetPrecision());
    // New field addition allows for error code reporting for each test step
    if ( !diagnosticFailure ) { // no catastrophic pre-test diagnostic failure

        // First, see if there are any non-catastrophic pre-test diagnostic
        //   measurements to report.
        std::vector<TestStepDiagnostic>::const_iterator di = diags.begin(),
                                                        dj = diags.end();
        while ( di != dj ) {
            std::string result = convert<std::string>(++counter);
            result += delimmitter;
            result += di->TestName();
            result += delimmitter;
            result += di->Limits().first.ValueStr();
            result += delimmitter;
            result += di->Limits().second.ValueStr();
            result += delimmitter;
            result += di->MeasuredValue().ValueStr();
            result += delimmitter;
            // precision, not reported               
            result += delimmitter;
            result += di->Units();
            result += delimmitter;
            result += (di->Result() ? "P" : "F");
            result += delimmitter;

            // Store the (result)
            data_->push_back(result);
            ++di;
        } // while

        // Now, report actual test sequence information
        std::vector<TestStepInfo>::iterator i = vec.begin(), j = vec.end();
        while ( i != j ) {        
            TestStepInfo::TSPtr tptr = getTSPtr(*i);

            std::string result = convert<std::string>(++counter);
            result += delimmitter;
            result += tptr->TestName();
            result += delimmitter;
            result += tptr->Limits().first.ValueStr();
            result += delimmitter;
            result += tptr->Limits().second.ValueStr();
            result += delimmitter;
            result += tptr->MeasuredValue().ValueStr();
            result += delimmitter;
            // precision, not reported               
            result += delimmitter;
            result += tptr->Units();
            result += delimmitter;
            result += (tptr->Result() ? "P" : "F");
            result += delimmitter;
            if ( tptr->ErrorCode() != TestStepInfo::TestStep::NODUTERROR )
                result += convert<std::string>(tptr->ErrorCode());

            // Store the (result)
            data_->push_back(result);
            ++i;
        } // while
    }
    else { // pre-test diagnosticFailure detected; no actual test(s) run
        TestStepDiagnosticFacadeFailure tsf = sequence->GetPreTestDiagnosticFailure();
        std::string result = convert<std::string>(0); // use step# 0
        result += delimmitter;
        result += tsf.TestName();
        result += delimmitter;
        result += tsf.Limits().first.ValueStr();
        result += delimmitter;
        result += tsf.Limits().second.ValueStr();
        result += delimmitter;
        result += tsf.MeasuredValue().ValueStr();
        result += delimmitter;
        // precision, not reported               
        result += delimmitter;
        result += tsf.Units();
        result += delimmitter;
        result += "F";
        result += delimmitter;
        result += convert<std::string>(tsf.ErrorNumber());

        // Store the (result)
        data_->push_back(result);
    }
}

//========
// Name()
//========
std::string DataArchive::Name() {
    return("Data Archive Class");
}

//=====================
// extraction operator
//=====================
std::ostream& operator<<(std::ostream& os, const DataArchive& data) {
    std::ostream_iterator<std::string> output(os, "\n");
    std::copy(data.data_->begin(), data.data_->end(), output);
    return(os);
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
