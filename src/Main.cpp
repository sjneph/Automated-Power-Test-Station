/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/***************************************************************************************/
// Author: Shane J. Neph
//
// Comments:
// (1) Software changes should be documented at the place of change (or top of File in
//      which the change occured).  The place of change and the current software
//      revision should be annotated in SPTSSoftware.cpp.  Documented changes should be
//      ordered with the latest change first, first change last.
// (2) All special file/project dependencies should be annotated in SPTSSoftware.cpp
/***************************************************************************************/



//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes (in Main) <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   11/20/05, sjn,
   ==============
     Modified archiveData() to take fewer arguments.  Modified calls to archiveData()
       accordingly.  We used to print to a local file and then let the GUI software
       read it in for printing purposes.  We now send that information to the GUI
       software directly to make process more robust.  This will help to move files
       out to a network location eventually.  Removed code dealing with the local
       print file.  Removed #include "StationFile.h"

   ==============
   05/10/05, sjn,
   ==============
     Added call to PostSequenceReset() to ensure APS's are reset properly between DUTs.
     Added function checkPtr() to avoid performance warnings of current compiler -->
         all assertions of ptr or file stream handles will now use checkPtr().
     Changed all unnamed namespace functions to start with a lower case for consistency
         with remainder of project.

   ==============
   07/21/04, sjn,
   ==============
     Created archiveData() - moved that part of the code out of main()
     Upon receiving notice that operator is done testing (via DoneTesting object), reset
         the station (no custom resets) after sending temperature controller to Room.
     Removed using directives for NoLimitsFound and NoVariablesFound.
     Created synchronizeSingletons() and added several headers that are used by it.
     Added call to synchronizeSingletons() before each test sequence starts.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes (in Main) <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


// Files included
#include "Assertion.h"
#include "Converter.h"
#include "DataArchive.h"
#include "DateTime.h"
#include "DialogBox.h"
#include "DoneTesting.h"
#include "ErrorLogger.h"
#include "Functions.h"
#include "LimitsFile.h"
#include "OperatorInterface.h"
#include "OScopeSetupFile.h"
#include "Shutdown.h"
#include "SingletonType.h"
#include "SPTS.h"
#include "SPTSException.h"
#include "StationAlgorithms.h"
#include "StandardFiles.h"
#include "TestFixtureFile.h"
#include "TestSequence.h"
#include "VariablesFile.h"


namespace {
    // Exception types - typedefs
    typedef MinorExceptionTypes::WrongRevision ATPMismatch;
    typedef UserInputExceptionTypes::UserInterfaceError UIError;

    // Static constants
    static const std::string name = "main";

    // Function prototypes
    void archiveData(const DataArchive&, bool = false);
    template <typename PtrType>
    bool checkPtr(const PtrType& ptr);
    void synchronizeSingletons();
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

int main() {
    // namespace alias
    namespace StationNS = SpacePowerTestStation;

    // Exception types - using declarations
    using StationExceptionTypes::FileError;
    using MinorExceptionTypes::LocalArchive;
    using StationExceptionTypes::NoArchive;
    using MinorExceptionTypes::Orientation;
    using MinorExceptionTypes::WrongFixture;    
    using StationExceptionTypes::UnknownException;
    using StationExceptionTypes::UnsafeCondition;

    // Declare loop variables
    OperatorInterface* operatorInterface = 0;
    SpacePowerTestStation::SPTS* spts = 0;
    TestSequence* testSequence = 0;
    bool done = false;
    bool noResetTemp = false;
    bool stationInitialized = false;
    std::string infiniteShutdown = "Infinite Shutdown Recursion Detected";
    DialogBox& screen = (*SingletonType<DialogBox>::Instance());
    ErrorLogger& errorLog = (*SingletonType<ErrorLogger>::Instance());

    try {
        // Get OperatorInterface instance and other singletons
        operatorInterface = SingletonType<OperatorInterface>::Instance();
        testSequence = SingletonType<TestSequence>::Instance();
        spts = SingletonType<SpacePowerTestStation::SPTS>::Instance();

        while ( !done ) { // keep testing

            // Get user input
            try {
                if ( stationInitialized )
                    StationNS::PostSequenceReset();
                operatorInterface->Reset();
            } catch(DoneTesting& dt) {
                if ( stationInitialized ) { // back to room temp, minus a little
                    spts->SetTemperatureBase(GetRoomTemperature().Value() - 2);
                    spts->Reset(false); // reset all except temperature controller
                }
                throw(dt); // rethrow DoneTesting object
            }

            // synchronize singleton variables
            try {
                synchronizeSingletons();
            } catch(SPTSExceptions::MinorStationBase& met) {
                screen << met.GetExceptionInfo();
                screen.DisplayInfo();
                continue;
            }

            // Start timing
            Clock clock;
            clock.StartTiming();

            try { // Deal with minor station problems and DUT problems

                // Initialize station - only done during first iteration
                if ( !stationInitialized ) {
                    // Initialize test station
                    StationNS::InitializeAlgorithms();                
                    StationNS::InitializeStation();
                    stationInitialized = true;
                }
                else
                    spts->NewDUTSetup();

                // Ensure test revision is current
                std::string rev =
                         SingletonType<LimitsFile>::Instance()->GetRevisionLevel();
                if ( rev != operatorInterface->GetLimitsRevision() ) {
                    if ( // In a non-production testing mode, supply the revision
                         operatorInterface->IsEngineeringTest()  ||
                         operatorInterface->IsGoldStandardTest() ||
                         operatorInterface->IsStationDebugMode() ||
                         operatorInterface->IsTestEngineeringTest() 
                       ) 
                        operatorInterface->SetRevisionLevel(rev);
                    else // revision mismatch
                        throw(ATPMismatch(std::string("Software Revision: ") + rev));
                }  

                // Perform system setup checks
                Assert<WrongFixture>(StationNS::FixtureIDCheck(), name);
                Assert<Orientation>(StationNS::DUTOrientation(), name);

                // Enable the converter
                spts->ResetPath(ControlMatrixTraits::RelayTypes::PRIMARYINHIBIT);

                // Synchronize the next test sequnce
                testSequence->Synchronize();
            } catch(SPTSExceptions::MinorStationBase& met) { // Minor Station Exception
                clock.StopTiming();
                screen << met.GetExceptionInfo();
                screen.DisplayInfo();
                continue;
            } catch(SPTSExceptions::DUTBase& det) { // DUT exception
                clock.StopTiming();
                screen << det.GetExceptionInfo();
                screen.DisplayInfo();
                continue;             
            } // try

            // Run the test sequence
            try {
                testSequence->PerformSequence();
            } catch(DUTExceptionTypes::TestAborted&) { // Aborted Test
                ShutDown(noResetTemp);                    
                StationNS::InitializeStation(noResetTemp);
            } catch(SPTSExceptions::DUTBase& det) { // DUT exception
                clock.PauseTiming();
                ShutDown(noResetTemp);
                StationNS::InitializeStation(noResetTemp);
                screen << det.GetExceptionInfo();
                screen.DisplayInfo();
                clock.UnPause();                                      
            } catch(...) { // Any other exception 
                ShutDown();
                throw;
            } // try

            // Stop timing
            clock.StopTiming();

            // Archive data if applicable
            DataArchive da(clock.ElapsedTime());
            archiveData(da);
           
            // Try to print; operator may still have chosen not to, however
              std::stringstream printValue;
              printValue << da;
              operatorInterface->Print(printValue.str());
              if ( operatorInterface->IsUIError() )
                  throw(UIError(operatorInterface->WhatUIError()));

            // Make sure there isn't a user interface error
            if ( operatorInterface->IsUIError() )
                throw(UIError(operatorInterface->WhatUIError()));

            errorLog.Clear();
        } // main test (while) loop


    // Exception handlers outside of main test while loop; shutting down program
    //  from here - point of no return
    } catch(UserInputExceptionTypes::NoScreen& ns) {
        try {            
            if ( stationInitialized )
                ShutDown();           
        } catch(bool) { /* */ }
        errorLog << ns.GetExceptionInfo();
        try {
            screen << ns.GetExceptionInfo();
            screen.DisplayWarning();
        } catch(...) { /* */ }
    } catch(SPTSExceptions::UserInputBase& ui) {
        try {
            if ( stationInitialized )
                ShutDown();
        } catch(bool) {
            screen << infiniteShutdown;
            screen.DisplayWarning();
        }
        errorLog << ui.GetExceptionInfo();
        screen << ui.GetExceptionInfo();
        screen.DisplayWarning();        
    } catch(SPTSExceptions::MinorStationBase& met) {
        try {
            if ( stationInitialized )
                ShutDown();
            screen << met.GetExceptionInfo();            
            screen.DisplayInfo();
        } catch(bool) {
            screen << met.GetExceptionInfo();
            screen << infiniteShutdown;
            screen.DisplayWarning();            
        }
    } catch(SPTSExceptions::MajorStationBase& met) {
        try {
            errorLog << met.GetExceptionInfo();
            try {
                bool isError = true;
                ProgramTypes::MType elapsedTime = 0;
                DataArchive da(elapsedTime);
                archiveData(da, isError);
            } catch(...) { /* can't archive this one */ }

            if ( stationInitialized ) 
                ShutDown();
            screen << met.GetExceptionInfo();
            screen.DisplayWarning();
        } catch(bool) {
            screen << met.GetExceptionInfo();
            screen << infiniteShutdown;
            screen.DisplayWarning();
        }
    } catch(SPTSExceptions::DUTBase& det) {
        try {
            if ( stationInitialized )
                ShutDown();
            screen << det.GetExceptionInfo();
            screen.DisplayInfo();        
        } catch(bool) {
            screen << det.GetExceptionInfo();
            screen << infiniteShutdown;
            screen.DisplayWarning();
        }
    } catch(SPTSExceptions::ExceptionBase& eb) {
        try {
            errorLog << eb.GetExceptionInfo();
            try {
                bool isError = true;
                ProgramTypes::MType elapsedTime = 0;
                DataArchive da(elapsedTime);
                archiveData(da, isError);
            } catch(...) { /* can't archive this one */ }

            if ( stationInitialized ) 
                ShutDown();
            screen << eb.GetExceptionInfo();
            screen.DisplayWarning();
        } catch(bool) {
            screen << eb.GetExceptionInfo();
            screen << infiniteShutdown;
            screen.DisplayWarning();
        }
    } catch(DoneTesting&) {
        // do nothing
    } catch(...) {
        errorLog << UnknownException::GetDialog()
                 << " Error Number: " 
                 << UnknownException::GetValue();
        try {
            bool isError = true;
            ProgramTypes::MType elapsedTime = 0;
            DataArchive da(elapsedTime);
            archiveData(da, isError);
        } catch(...) { /* can't archive this one */ }

        try {
            if ( stationInitialized )
                ShutDown();
            screen << UnknownException::GetDialog() 
                   << " Error Number: "
                   << UnknownException::GetValue();
            screen.DisplayWarning();        
        } catch(bool) {
            screen << UnknownException::GetDialog() 
                   << " Error Number: "
                   << UnknownException::GetValue()
                   << infiniteShutdown;
            screen.DisplayWarning();
        }
    } // end-try
  
    try {
        errorLog.Archive(); // only done if there exists a recorded error
    } catch(...) { /* */ }

    try {
        if ( operatorInterface ) // close
            operatorInterface->Close();
    } catch(...) { /* */ }  

    return(0);
}





//=======================================
// Implementation of function prototypes
//=======================================
namespace {

    //===============
    // archiveData()
    //===============
    void archiveData(const DataArchive& da, bool stationError) {
        // using declarations
        using StationExceptionTypes::FileError;
        using MinorExceptionTypes::LocalArchive;
        using StationExceptionTypes::NoArchive;

        // vars
        OperatorInterface* oi = SingletonType<OperatorInterface>::Instance();
        TestSequence* testSequence = SingletonType<TestSequence>::Instance();
        DialogBox& screen = (*SingletonType<DialogBox>::Instance());
        ErrorLogger& errorLog = (*SingletonType<ErrorLogger>::Instance());

        try { 
            if ( testSequence->HasAnyTests() ) { // something to archive
                StationFile* sf = SingletonType<StationFile>::Instance();

                if ( !oi->IsStationDebugMode() ) { // non-debug mode                      
                    std::ofstream of;
                    std::ofstream::open_mode mode = std::ofstream::app;

                    // Archive to local directory
                    if ( !stationError ) { // ok
                        if ( oi->IsEngineeringTest() ) // engineering mode
                            of.open(sf->LocalEngArchive().c_str(), mode);
                        else if ( oi->IsTestEngineeringTest() ) // test eng
                            of.open(sf->LocalTestEngArchive().c_str(), mode);
                        else if ( oi->IsGoldStandardTest() ) // gold standard
                            of.open(sf->LocalGoldArchive().c_str(), mode);
                        else // normal production run
                            of.open(sf->LocalArchive().c_str(), mode);
                    }
                    else // log data in error archive
                        of.open(sf->LocalErrorArchive().c_str(), mode);
                    Assert<FileError>(checkPtr(of), "main");
                    of << da << std::endl << std::endl;
                    of.close();
                } // if

                try { // Archive to network
                    if ( !oi->IsStationDebugMode() && !stationError ) { // non-debug                        
                        std::ofstream oracle(sf->OraclePath().c_str());
                        Assert<FileError>(checkPtr(oracle), name);
                        oracle << da;
                        oracle.close();
                    }
                } catch(...) {
                    // Network problem --> store to temporary local location
                    std::ofstream local(sf->BackupLocalStorage().c_str());
                    Assert<FileError>(checkPtr(local), name);
                    local << da;
                    local.close();
                    screen << LocalArchive::GetDialog() 
                           << " ErrorNumber: " 
                           << LocalArchive::GetValue();
                    screen.DisplayInfo();
                } // try
            } // if
        } catch(...) { // Data Archiving problem
            std::stringstream s;
            s << NoArchive::GetDialog();
            s << " ErrorNumber: ";
            s << NoArchive::GetValue();
            screen  << s.str();
            screen.DisplayWarning();
            errorLog << s.str();
            throw;
        } // try
    }

    //============
    // checkPtr()
    //============
    template <typename PtrType>
    bool checkPtr(const PtrType& ptr) {
        return(ptr != 0);
    }

    //=========================
    // synchronizeSingletons()
    //=========================
    void synchronizeSingletons() {
        // This order of initialization should be maintained
        // This function should be called only after operator interface gets its input
        // This function should be called before every test.
        // main() calls other singletons prior to calling this; all of those
        // constructors are independent of the need for the following singletons.

        SingletonType<Converter>::Instance()->Initialize();
        // VariablesFile is always Reloaded() via Converter::Initialize()
        SingletonType<LimitsFile>::Instance()->Reload();
        SingletonType<OScopeSetupFile>::Instance()->Reload();
        VariablesFile* vf = SingletonType<VariablesFile>::Instance();
        SingletonType<TestFixtureFile>::Instance()->SetFixture(vf->Fixture());
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

