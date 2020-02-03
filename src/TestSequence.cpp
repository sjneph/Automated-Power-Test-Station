// Files included
#include "Assertion.h"
#include "Converter.h"
#include "ConverterOutput.h"
#include "DateTime.h"
#include "Functions.h"
#include "LimitsFile.h"
#include "MeasurementFunctions.h"
#include "ScaleUnits.h"
#include "SingletonType.h"
#include "SPTS.h"
#include "SPTSException.h"
#include "StandardStationFiles.h"
#include "StationAlgorithms.h"
#include "TestSequence.h"
#include "VariablesFile.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
  =================
  03/27/06, HQP,FAC
  =================
      Forced the test sequence tracker to false
	  when the total path resistance test step fails at
	  either end of the limits.

  ==============
  12/02/05, sjn,
  ==============
      Placed try/catch for aborted test are call to:
        SpacePowerTestStation::InitializeBaseTemperature().  Previously, we would not
        report the minor TestAborted exception, but bubbled up to a major station error.

  ==============
  11/20/05, sjn,
  ==============
      Modified doSequence() to add the potential check of the total path input
        resistance prior to ramping the input voltage to the DUT.  This ensures that
        we have a good input power connection prior to testing.
      Renamed class TestStepFacade to TestStepDiagnosticFacadeFailure.
      Modified HasAnyTests() to include any pre-test diagnostic measurements to report.
      Modified Synchronize() to reset fakeTest_, diagnosticFailure_ and
        diagnosticMeasurements_.
      Added GetPreTestDiagnosticsMeasurements().

  ==============
  05/05/05, sjn,
  ==============
      Modifed doSequence() slightly --> catch DUTException's and log appropriate error
        code for storage in database.
      Added GetPreTestDiagnosticFailure(), IsPreTestDiagnosticFailure().
      Added HasAnyTests().
      Added BadCommand typedef to unnamed namespace.
      Changed name() and GetTests() to const member function implementations.
      Modified nextTest() Overload1's implementation --> detect and track parametric
        DUT problems.
      Removing Delta Testing Capability from this software: Separate app has been made.
        Removed several unnamed namespace helper functions and classes - nameDelta(),
          DeltaFileTag(), getLimitsFilePointer(), some typedefs associated with delta
          related exception types, getDeltaNames().
        Removed members: deltaCompare(), deltaRecord()
        Modified doSequence() and Synchronize() to remove all delta-related code.

  ==============
  12/14/04, sjn,
  ==============
      Modified setResult() to deal with possibility of measurement being on min or 
        max boundary, but having a different precision (as a string representation).
      Added header - LimitsFile.h
      Removed member variables:   newSequence_, testType_, serialNumber_, dashNumber_
                                  and familyNumber_
      Removed member functions:   void initiateTests(), void startNewSequence()
                                  and void startSameSequence();
      Renamed Initialize() to Synchronize();
      Modified Synchronize():     Assumed some responsibility from old initiateTests(),
                                  startNewSequence() and startSameSequence()
      Modified PerformSequence(): Assumed most of responsibility split between 
                                  startNewSequence() and startSameSequence()
      Added getDeltaNames() local helper function.
      Modified getDeltaCompare(), deltaRecord() and deltaCompare().  The changes
        were to improve our delta testing capabilities.  In particular, we now save
        the first half of a delta test in a non-scaled format (575000 for 575kHz for
        example).  We record the precision of the delta measurements to match that
        for the test (565000 for a measurement of 565kHz w/limits of 555-575kHz, instead
        of 565748).  These changes allow us to exactly match the precision of the
        difference as would be determined by manually subtracting the two results from
        the test printouts.  It also allows for one test to use different units from the
        second (ie; Final Room Iin PARD with mApp and Final Cold using App).
      Modified PerformSequence(): error checking related to offset temperatures --> some
        flatpacks actually require offsets < 0 when going hot.
      Modified doSequence() --> DUT diagnostics are now an option, not a requirement.

  ==============
  11/05/03, sjn,
  ==============
     Modified doSequence().  Incrementing main loop counters before checking for a 
       test step failure/stop on first failure now.  We were not recording the actual 
       failed test step correctly.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    // Station exceptions
    typedef StationExceptionTypes::BadArg              BadArg;
    typedef StationExceptionTypes::BadCommand          BadCommand;
    typedef StationExceptionTypes::ProgramError        BadClassState;
    typedef StationExceptionTypes::BadRtnValue         BadRtnValue;
    typedef StationExceptionTypes::ContainerState      ContainerState;
    typedef StationExceptionTypes::FileError           FileError;
    typedef StationExceptionTypes::TemperatureOverflow TemperatureOverflow;
    typedef StationExceptionTypes::TemperatureTimeout  TemperatureTimeout;
    typedef StationExceptionTypes::UnexpectedState     UnexpectedState;

    enum { TENPCT = 10, HUNDREDPCT = 100 };
    typedef DUTExceptionTypes::BadVout<TENPCT>::BadVoutType BadVoutML; 
    typedef DUTExceptionTypes::BadVout<HUNDREDPCT>::BadVoutType BadVoutFL;
    typedef DUTExceptionTypes::BadInhibit<TENPCT>::BadInhibitType BadInhibitML;
    typedef DUTExceptionTypes::BadInhibit<HUNDREDPCT>::BadInhibitType BadInhibitFL;

    // Even though a power connection problem is not a DUT Exception, we will treat
    //  it that way so that we can send the information to the database via fakeTest_.
    struct PowerConnectionTag {
        static std::string Name() {
            return("Bad Vin power connection");
        }
    };

    // Homemade Exceptions
    typedef ExceptionTypes::SomeException<
                                          BadVoutML::ID, /* need some nonzero number */
                                          DUTExceptionTypes::BaseException,
                                          PowerConnectionTag
                                         > PowerConnection;

    // User interface exceptions
    typedef UserInputExceptionTypes::UserInterfaceError InterfaceError;

    ProgramTypes::MType tooBig   = 8E9;
    ProgramTypes::MType tooSmall = tooBig * ProgramTypes::MType(-1);
    std::string nonMeasure = SPTSMeasurement::Measurement::BadMeasurementStr;
} // namespace unnamed

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=============
// Constructor
//=============
TestSequence::TestSequence() : sequence_(new VecTestInfo),
                               fakeTest_("n/a", TestStepInfo::TestStep::NODUTERROR),
                               diagnosticFailure_(false),
                               speedSequence_(new SpeedMapType), status_(false),
                               testCounter_(0), sync_(false) {
    oi_ = SingletonType<OperatorInterface>::Instance();
}

//============
// Destructor
//============
TestSequence::~TestSequence() 
{ /* */ }

//=====================
// checkSystemErrors()
//=====================
void TestSequence::checkSystemErrors(const std::string& testName) {
    typedef SpacePowerTestStation::SPTS StationType;    
    StationType* station = SingletonType<StationType>::Instance();
    if ( station->IsError() ) {
        typedef StationExceptionTypes::InstrumentError IE;
        InstrumentFile* ifile = SingletonType<InstrumentFile>::Instance();
        std::pair<SPTSInstrument::InstrumentTypes::Types, std::string> whatError; 
        whatError = station->WhatError();
        std::string errormsg = ifile->GetModelType(whatError.first);
        errormsg += (" " + whatError.second);
        bool throwError = false;
        testName.empty() ?
            Assert<IE>(throwError, errormsg)
                     :
            Assert<IE>(throwError, testName, errormsg);
    }
}

//=================
// checkTestName()
//=================
void TestSequence::checkTestName(TestStepInfo::TSPtr tptr) {
    typedef SPTSMeasurement::Measurement::MFactory MF;
    std::string tName = Uppercase(tptr->SoftwareTestName());
    Assert<FileError>(MF::Instance()->IsRegistered(tName), name());
}

//=====================
// createMeasurement()
//=====================
SPTSMeasurement::Measurement* 
                          TestSequence::createMeasurement(TestStepInfo::TSPtr tptr) {
    // Create measurement object 
    typedef SPTSMeasurement::Measurement::MFactory MF;    
    SPTSMeasurement::Measurement* toMeasure;
    toMeasure = MF::Instance()->CreateObject(Uppercase(tptr->SoftwareTestName()));    
    Assert<UnexpectedState>(toMeasure != 0, name());
    return(toMeasure);
}

//==============
// doSequence()
//==============
void TestSequence::doSequence() {
    // Initialize necessary members
    testCounter_ = 0;
    status_ = false;
    sync_ = false;

    // Locals
    ProgramTypes::MTypeContainer iouts;    
    iouts = SingletonType<Converter>::Instance()->Iouts();
    ProgramTypes::MTypeContainer::iterator a = iouts.begin();
    ProgramTypes::SetTypeContainer fullLoads, tenPercLoads;
    std::string name = TestSequence::name();
    while ( a != iouts.end() ) {
        fullLoads.push_back(a->Value());
        tenPercLoads.push_back(a->Value() / 10.0);
        ++a;
    }
    bool realStatus = true, last = false;  //This line was moved from line 319
										   //to here to make sure that we declared
										   //bool realStatus prior to using it.

    // Display upcoming tests to the operator    
    {        
        std::vector<TestStepInfo::TSPtr> toDisplay;
        VecTestInfo::iterator i = sequence_->begin();
        while ( i != sequence_->end() ) {
            toDisplay.push_back(getTestPointer(*i));
            ++i;
        }

        oi_->DisplayTestSequence(diagnosticMeasurements_, toDisplay);
        if ( oi_->IsUIError() )
            throw(InterfaceError(oi_->WhatUIError()));     
    } // end local scope

    // Ramp Vin and perform DUT diagnostics; see if operator aborts
    diagnosticFailure_ = false;
    try {
        typedef SpacePowerTestStation::SPTS StationType;
        VariablesFile* vf = SingletonType<VariablesFile>::Instance();
        static const std::string badConn = "Bad input power connection";
        ProgramTypes::MType minOhms = 0.04, maxOhms = 0.2, zero = 0;
        minOhms.SetPrecision(3);
        maxOhms.SetPrecision(3);
        std::pair<bool, ProgramTypes::MType> powerCheck =
                      SpacePowerTestStation::VerifyPowerConnection(maxOhms);
        std::string measuredOhms = powerCheck.second.ValueStr() + " ohms";
        Assert<PowerConnection>(powerCheck.first, badConn, measuredOhms);
        if( powerCheck.second > zero )
		{ // actual measurement made
            typedef ScaleUnits<ProgramTypes::MType> SU;
            std::string ohms = SU::GetUnits(SU::Ohm);
            ProgramTypes::PairMType lim = std::make_pair(minOhms, maxOhms);
            TestStepDiagnostic tsd("Total Path Resistance", lim,
                                   powerCheck.second, ohms);
            diagnosticMeasurements_.push_back(tsd);
            if ( !tsd.Result() ) //If fails, set to false.
								 //This is test sequence failure vs test step
								 //failure
                realStatus = false;
        }
        Assert<DUTExceptionTypes::TestAborted>(!oi_->DidAbort(), name);
        SpacePowerTestStation::RampVin();
        Assert<DUTExceptionTypes::TestAborted>(!oi_->DidAbort(), name);
        if ( vf->DoDUTDiagnostics() ) { // perform diagnostics
            SingletonType<StationType>::Instance()->SetLoad(tenPercLoads);
            SpacePowerTestStation::CheckVouts<BadVoutML>();
            Assert<DUTExceptionTypes::TestAborted>(!oi_->DidAbort(), name);
            SpacePowerTestStation::CheckVoutsAtInhibit<BadInhibitML>();
            Assert<DUTExceptionTypes::TestAborted>(!oi_->DidAbort(), name);
            SingletonType<StationType>::Instance()->SetLoad(fullLoads);
            SpacePowerTestStation::CheckVouts<BadVoutFL>();
            Assert<DUTExceptionTypes::TestAborted>(!oi_->DidAbort(), name);
            SpacePowerTestStation::CheckVoutsAtInhibit<BadInhibitFL>();
            Assert<DUTExceptionTypes::TestAborted>(!oi_->DidAbort(), name);
        }
    } catch(SPTSExceptions::DUTBase& db) {
        diagnosticFailure_ = true;
        status_ = false;
        fakeTest_ = 
          TestStepDiagnosticFacadeFailure(db.GetExceptionInfo(), db.GetExceptionID());
        oi_->SetSequenceResult(false); // show sequence failure
        throw;
    }

    // Re-display upcoming tests to the operator if diagnosticMeasurements_ non-empty
    {
        if ( !diagnosticMeasurements_.empty() ) {       
            std::vector<TestStepInfo::TSPtr> toDisplay;
            VecTestInfo::iterator i = sequence_->begin();
            while ( i != sequence_->end() ) {
                toDisplay.push_back(getTestPointer(*i));
                ++i;
            }

            oi_->DisplayTestSequence(diagnosticMeasurements_, toDisplay);
            if ( oi_->IsUIError() )
                throw(InterfaceError(oi_->WhatUIError()));

            // Update GUI with pretest results
            for ( std::size_t i = 0; i < diagnosticMeasurements_.size(); ++i )
                oi_->DisplayTestResult(diagnosticMeasurements_[i]);
        }   
    } // end local scope

    // Set members for sequence
    stopOnFailure_ = oi_->GetStopOnFirstFailure();
    bool initialTemp = true;    

    // Local vars
    VecTestInfo::iterator i = sequence_->begin(), j = sequence_->end();
    SPTSMeasurement::Measurement* toMeasure = 0;

    // Initialize temperature
    try{
        SpacePowerTestStation::EnforceTemperature(initialTemp);
    } catch(DUTExceptionTypes::TestAborted& ta) {
        diagnosticFailure_ = true;
        status_ = false;
        fakeTest_ = TestStepDiagnosticFacadeFailure(ta.GetExceptionInfo(),
                                                        ta.GetExceptionID());
        oi_->SetSequenceResult(false); // show sequence failure
        throw(ta);
    }
    initialTemp = false;

    // Time variables used for DUT temperature measurements
    ProgramTypes::MType time = SingletonType<StationFile>::Instance()->TempCheckFreq();
    Assert<BadRtnValue>(time > ProgramTypes::MType(0), name);
    Clock temperatureTimer;
    temperatureTimer.StartTiming();

    // Ensure there are no system errors
    checkSystemErrors();
    
    // Perform test sequence
    while ( i != j ) {
        last = false;
 
        try {           
            // See if there is any GUI error condition
            if ( oi_->IsUIError() )
                throw(InterfaceError(oi_->WhatUIError()));        

            // Create measurement object
            toMeasure = createMeasurement(*i);

            // See if we can get out of making the next measurement
            std::pair<bool, ProgramTypes::MType> speed = speedUp(*i, toMeasure);            

            if ( speed.first ) // measurement done previously
                last = nextTest(*i, *i, *i, speed.second);
            else { // make next measurement
                if ( temperatureTimer.ElapsedTime() > time ) { // time to check temp?
                    SpacePowerTestStation::EnforceTemperature(initialTemp);
                    temperatureTimer.Clear(); 
                    temperatureTimer.StartTiming(); // restart clock
                }
                last = nextTest(*i, *i, *i, toMeasure);
            } // if-else

            // If measurement is too big/small, set to value defined by ENG-019
            if ( getTestPointer(*i)->MeasuredValue() > tooBig ) {    
                i->setMeasuredValueExplicit(nonMeasure);
                i->setResult(false); // failure
            }
            else if ( getTestPointer(*i)->MeasuredValue() < tooSmall ) {
                i->setMeasuredValueExplicit(nonMeasure);
                i->setResult(false); // failure            
            }

            // See if the operator aborted the test
            Assert<DUTExceptionTypes::TestAborted>(!oi_->DidAbort(), name);

            // Update GUI
            oi_->DisplayTestResult(*i);

            // Update counters
            ++i;
            ++testCounter_;
        
            if ( !last ) { // failed test
                realStatus = false; // set sequence status
                if ( stopOnFailure_ )
                    break;
            }

            // Clean up
            delete toMeasure;
            toMeasure = 0;
        } catch(DUTExceptionTypes::TestAborted& ta) {
            if ( toMeasure ) // clean up
                delete(toMeasure);
            if ( i != sequence_->begin() ) {
                // Show an aborted test on previous test step via an error code,
                //  but do not change measurement or P-F status --> just keep
                //  aborted information
                --i;
                i->setErrorCode(ta.GetExceptionID());
            }
            else { // aborted at first step; treat as pre-test diagnostic failure
                diagnosticFailure_ = true;
                status_ = false;
                fakeTest_ = TestStepDiagnosticFacadeFailure(ta.GetExceptionInfo(),
                                                            ta.GetExceptionID());
            }
            oi_->SetSequenceResult(false); // show sequence failure
            throw(ta);
        } catch(DUTExceptionTypes::BaseException& dbe) {
            if ( toMeasure ) // clean up
                delete(toMeasure);
            if ( i != j ) {
                i->setMeasuredValueExplicit(nonMeasure);
                i->setErrorCode(dbe.GetExceptionID());
                i->setResult(false);
                ++testCounter_; ++i; // so we keep this test
            }
            oi_->SetSequenceResult(false); // show sequence failure
            throw(dbe);         
        } catch(...) {
            if ( toMeasure ) // clean up
                delete(toMeasure);
            oi_->SetSequenceResult(false); // show sequence failure
            throw;
        }         
    } // while

    // Determine sequence pass/fail status
    if ( i == j ) 
        status_ = realStatus;
    else
        status_ = false;

    // Power the system down
    SingletonType<SpacePowerTestStation::SPTS>::Instance()->SafeInhibit(ON);
    SingletonType<SpacePowerTestStation::SPTS>::Instance()->PowerDown();
    SingletonType<SpacePowerTestStation::SPTS>::Instance()->SafeInhibit(OFF);
  
    // Check for any instrument errors
    checkSystemErrors();

    // Show operator test sequence result
    oi_->SetSequenceResult(SequenceStatus());
}

//==================
// getCondPointer()
//==================
TestStepInfo::CondPtr TestSequence::getCondPointer(TestStepInfo::CondPtr cptr) {
    return(cptr);
}

//==================
// getTestPointer()
//==================
TestStepInfo::TSPtr TestSequence::getTestPointer(TestStepInfo::TSPtr tptr) {
    return(tptr);
}

//===============================
// GetPreTestDiagnosticFailure()
//===============================
TestStepDiagnosticFacadeFailure TestSequence::GetPreTestDiagnosticFailure() const {
    Assert<BadCommand>(IsPreTestDiagnosticFailure(), name());
    return(fakeTest_);
}

//=====================================
// GetPreTestDiagnosticsMeasurements()
//=====================================
std::vector<TestStepDiagnostic> TestSequence::GetPreTestDiagnosticsMeasurements() const {
    /*
       These should not include any catastrophic (failed) pre-test diagnostics.  Use
         fakeTest_ for that --> there should 0 or 1 only of those
    */
    return(diagnosticMeasurements_);
}

//============
// GetTests()
//============
std::vector<TestStepInfo> TestSequence::GetTests() const {
    std::vector<TestStepInfo> toRtn;
    Assert<UnexpectedState>(std::size_t(testCounter_) <= sequence_->size(), name());
    std::vector<TestStepInfo>::const_iterator s = sequence_->begin(), beg = s;
    std::advance(s, testCounter_);
    std::copy(beg, s, std::back_inserter(toRtn));
    return(toRtn);
}

//===============
// HasAnyTests()
//===============
bool TestSequence::HasAnyTests() const {
    // true if any test run or if diagnostic pre-test failure generated
    return(IsPreTestDiagnosticFailure() ||
           !GetTests().empty()          ||
           !diagnosticMeasurements_.empty());
}

//==============================
// IsPreTestDiagnosticFailure()
//==============================
bool TestSequence::IsPreTestDiagnosticFailure() const {
    return(diagnosticFailure_);
}

//========
// name()
//========
std::string TestSequence::name() const {
    return("Test Sequence Class");
}

//======================
// nextTest() Overload1
//======================
bool TestSequence::nextTest(TestStepInfo::TSPtr tptr, TestStepInfo::CondPtr cptr,
                   TestStepInfo& currentTest, SPTSMeasurement::Measurement* toMeasure) {
        
    // Make measurement and call overloaded nextTest()
    ReturnType value = toMeasure->Measure(cptr, tptr->ScaledLimits());
    updateSpeedMap(currentTest, toMeasure->ExtraMeasurements());
    if ( toMeasure->IsDUTError() ) // parametric DUT problem was detected?
        currentTest.setErrorCode(toMeasure->WhatDUTError());
    return(nextTest(tptr, cptr, currentTest, updateSpeedMap(currentTest, value)));
}

//======================
// nextTest() Overload2
//======================
bool TestSequence::nextTest(TestStepInfo::TSPtr tptr, TestStepInfo::CondPtr,
                 TestStepInfo& currentTest, ProgramTypes::MType measuredValue) {

    // Clean up measurement info and store any extra measurement info
    ProgramTypes::PairMType limits = tptr->Limits();    
    setPrecision(limits, measuredValue, currentTest);
    setResult(limits, measuredValue, currentTest);
    return(tptr->Result());    
}

//===================
// PerformSequence()
//===================
void TestSequence::PerformSequence() {
    Assert<BadClassState>(sync_, name());

    // Set up test sequence information and variables
    typedef SpacePowerTestStation::SPTS StationType;
    StationType* station = SingletonType<SpacePowerTestStation::SPTS>::Instance();
    VariablesFile* vf = SingletonType<VariablesFile>::Instance();

    // See if new/current base temperatures are the same
    bool resetTemp = vf->GetTemperature() != station->GetTemperatureSetpoint();

    if ( resetTemp ) { // Reset the base temperature

        // Base plate temperature variables
        static const ProgramTypes::SetType roomTemperature     = GetRoomTemperature();
        static const ProgramTypes::SetType roomTemperatureLow  = roomTemperature - 5;
        static const ProgramTypes::SetType roomTemperatureHigh = roomTemperature + 5;
        ProgramTypes::SetType nextTemp, posTol = 5, negTol = -5;
        nextTemp = vf->GetTemperature();
        VariablesFile::TemperatureOffsetType offset;
        ProgramTypes::SetType zero = 0;

        // Grab temperature base offset info
        if ( nextTemp < roomTemperatureLow )
            offset = VariablesFile::COLDOFFSET;
        else if ( nextTemp > roomTemperatureHigh )
            offset = VariablesFile::HOTOFFSET;
        else
            offset = VariablesFile::ROOMOFFSET;

        // Make sure offset info makes sense WRT (toTemp)
        ProgramTypes::SetType offsetTemp = vf->GetTemperatureOffset(offset);
        if ( nextTemp < roomTemperatureLow ) 
            Assert<FileError>(offsetTemp < zero, name());
        else if ( nextTemp > roomTemperatureHigh ) // some flatpacks require < 0
            Assert<FileError>(offsetTemp > zero - ProgramTypes::SetType(5), name());
        else // room temp
            Assert<FileError>(offsetTemp <= zero, name());

        // Set base plate temperature
        nextTemp += offsetTemp;
        try {
            SpacePowerTestStation::InitializeBaseTemperature(nextTemp, posTol, negTol);
        } catch(DUTExceptionTypes::TestAborted& ta) {
            diagnosticFailure_ = true;
            status_ = false;
            fakeTest_ = TestStepDiagnosticFacadeFailure(ta.GetExceptionInfo(),
                                                         ta.GetExceptionID());
            oi_->SetSequenceResult(false); // show sequence failure
            throw(ta);
        }
    }

    // Perform the test sequence
    doSequence();
}

//==================
// SequenceStatus()
//==================
bool TestSequence::SequenceStatus() {
    return(status_);
}

//================
// setPrecision()
//================
void TestSequence::setPrecision(const ProgramTypes::PairMType& limits, 
                                ProgramTypes::MType& measured,
                                TestStepInfo& currentTest) {

    if ( limits.first.GetPrecision() > limits.second.GetPrecision() )
        measured.SetPrecision(limits.first.GetPrecision());        
    else
        measured.SetPrecision(limits.second.GetPrecision());
    currentTest.setMeasuredValue(measured);
}

//=============
// setResult()
//=============
void TestSequence::setResult(const ProgramTypes::PairMType& limits,
                             const ProgramTypes::MType& measured,
                             TestStepInfo& currentTest) {
    bool result = ((measured >= limits.first) && (measured <= limits.second));

    // The following 'if' selections account for measurements which would be 'failing'
    //    until they are rounded per the precision we have to report them to.  Keep
    //    in mind that limits.first's precision may not be the same as limit.second's.
    ProgramTypes::MType min = limits.first, max = limits.second;
    if ( measured.GetPrecision() > min.GetPrecision() )
        min.SetPrecision(measured.GetPrecision());
    else if ( measured.GetPrecision() > max.GetPrecision() )
        max.SetPrecision(measured.GetPrecision());

    if ( measured.ValueStr() == min.ValueStr() )   
        result = true;
    else if ( measured.ValueStr() == max.ValueStr() )
        result = true;
    currentTest.setResult(result);
}

//===========
// speedUp()
//===========
std::pair<bool, ProgramTypes::MType> 
                   TestSequence::speedUp(const TestStepInfo& currentTest, 
                                 SPTSMeasurement::Measurement* toMeasure) {

    // Locals
    typedef std::pair<SpeedMapType::iterator, SpeedMapType::iterator> pairType;
    pairType p = speedSequence_->equal_range(currentTest);
    TestStepInfo::CondPtr cptr = getCondPointer(currentTest);
    TestStepInfo::TSPtr t = getTestPointer(currentTest);
    ScaleUnits<ProgramTypes::MType>::Units u;
    u = ScaleUnits<ProgramTypes::MType>::MakeUnits(t->Units());

    // If channel info is correct and test is DoneAlready(), 
    //   then return (true, measurement)
    while ( p.first != p.second ) {
        TestResults::iterator tResults = p.first->second.find(cptr->Channel());
        if ( tResults != p.first->second.end() ) {
            if ( toMeasure->DoneAlready(currentTest, p.first->first) ) {
                ProgramTypes::MType toRtn = 
                        ScaleUnits<ProgramTypes::MType>::ScaleDown(tResults->second, u);
                p.first->second.erase(tResults); // don't reuse this value
                return(std::make_pair(true, toRtn));
            }
        }
        ++p.first;
    }    
    // Nothing found if here     
    return(std::make_pair(false, 0));
}

//===============
// Synchronize()
//===============
void TestSequence::Synchronize() {
    // Initialize members
    testCounter_ = 0;
    status_ = false;
    speedSequence_.reset(new SpeedMapType);
    sequence_.reset(new VecTestInfo);
    fakeTest_ = TestStepDiagnosticFacadeFailure(
                                             "n/a",
                                             TestStepInfo::TestStep::NODUTERROR
                                               );
    diagnosticFailure_ = false;
    diagnosticMeasurements_.clear();

    // Grab tests for the sequence; check names
    LimitsFile* lf = SingletonType<LimitsFile>::Instance();
    lf->RestartSameTest();
    Assert<UnexpectedState>(lf->NumberTests() > 0, name());    
    while ( !lf->AtEnd() ) {
        TestStepInfo tmp(lf);
        sequence_->push_back(tmp);
        checkTestName(tmp);
        ++(*lf); // increment to next test
    }
    lf->RestartSameTest();
    sync_ = true;
}

//==================
// updateSpeedMap()
//==================
ProgramTypes::MType TestSequence::updateSpeedMap(const TestStepInfo& tsi, 
                                                 const ReturnType& r) {

    // Warning: The underlying assumption is that (r) contains
    //  measurements in the order from ConverterOutput::One to
    //  the number of outputs of the converter.  If no extra 
    //  measurements were made, however, then (r) will only 
    //  contain the present measurement of interest, and may not
    //  be in ascending order as specified above (since only one
    //  measurement was made).

    // Locals
    TestStepInfo::CondPtr c = getCondPointer(tsi);
    TestStepInfo::TSPtr t = getTestPointer(tsi);
    ScaleUnits<ProgramTypes::MType>::Units u;
    u = ScaleUnits<ProgramTypes::MType>::MakeUnits(t->Units());

    // If no extra measurements were made, then nothing to update
    if ( (c->Channel() == ConverterOutput::ALL) || (1 == r.second.size()) )
        return(ScaleUnits<ProgramTypes::MType>::ScaleDown(r.second[0], u));

    // Locals
    TestResults tResults;
    ProgramTypes::MType toRtn;
    bool set = false;
    std::vector<ConverterOutput::Output> outputs
                                = SingletonType<Converter>::Instance()->Outputs();
    Assert<UnexpectedState>(outputs.size() >= r.second.size(), name());

    // Store extra measurements for future use; return current measurement
    long idx = 0;
    ProgramTypes::MType value;
    RTypeSecond::const_iterator i = r.second.begin(), j = r.second.end(); 
    while ( i != j ) {
        ConverterOutput::Output chan = outputs[idx];
        if ( chan == c->Channel() ) { // current measurement to be returned
            toRtn = *i;
            set = true;
        }
        else // extra measurement to store for future use
            tResults.insert(std::make_pair(chan, *i));       
        ++idx;       
        ++i;
    } // while

    // Ensure toRtn was actually set
    Assert<UnexpectedState>(set, name());

    if ( ! tResults.empty() ) // store info
        speedSequence_->insert(std::make_pair(tsi, tResults)); 
    return(ScaleUnits<ProgramTypes::MType>::ScaleDown(toRtn, u));
}

//============================
// updateSpeedMap() Overload2
//============================
void TestSequence::updateSpeedMap(const TestStepInfo& tsi, 
                                  const ReturnTypeContainer& rtc) {
    TestStepInfo copy = tsi;    
    ReturnTypeContainer::const_iterator i = rtc.begin();
    while ( i != rtc.end() ) {
        copy.setName(Uppercase(i->first));        
        TestStepInfo::CondPtr cptr = getCondPointer(copy);
        RTypeSecond rts = i->second;
        for ( std::size_t idx = 0; idx < rts.size(); ++idx ) {
            TestResults tResults;
            tResults.insert(std::make_pair(cptr->Channel(), rts[idx]));            
            speedSequence_->insert(std::make_pair(copy, tResults));
        }
        ++i;
    }
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
