// Macro Guard
#ifndef SPTS_TESTFIXTURE_H
#define SPTS_TESTFIXTURE_H

// Files included
#include "LoadTraits.h"
#include "NoCopy.h"
#include "ProgramTypes.h"
#include "SingletonType.h"
#include "SPTSFiles.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   11/15/05, sjn,
   ==============
     Added GetPowerConnectionMiscLine().  When defined, the return value will be used
       to help ensure that we have a solid input power connection to the DUT on the
       fixture power lines.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct TestFixtureFile : private NoCopy {
    //========================
    // Start Public Interface
    //========================    
    bool DownLeaded();
    std::vector<LoadTraits::Channels> GetLoadsWired();
    std::pair<bool, std::string> GetPowerConnectionMiscLine();
    ProgramTypes::MType IDResistor();
    ProgramTypes::MType IDResistorTolerance();
    ProgramTypes::SetType IoutShuntValue(LoadTraits::Channels chan);    
    std::string Name();
    std::string Revision();
    ProgramTypes::MType RLLIDResistor();
    ProgramTypes::MType RLLIDResistorTolerance();
    void SetFixture(const std::string& fixtureName);
    bool Symmetrical();
    bool SyncOutExists();
    ProgramTypes::MType VinMultiplier();
    //======================
    // End Public Interface
    //======================

private:
    friend class SingletonType<TestFixtureFile>;
    TestFixtureFile() : fixture_("")
    { /* */ }
    ~TestFixtureFile() 
    { /* */ }

private:
    FileTypes::TestFixtureFileType tf_;
    std::string fixture_;
};

#endif // SPTS_TESTFIXTURE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
