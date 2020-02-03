// Macro Guard
#ifndef SPTS_LIMITS_FILE_H
#define SPTS_LIMITS_FILE_H

// Files included
#include "GenericAlgorithms.h"
#include "NoCopy.h"
#include "OperatorInterface.h"
#include "ProgramTypes.h"
#include "SingletonType.h"
#include "SPTSFiles.h"
#include "StandardFiles.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   03/09/05, sjn,
   ==============
     Removing Delta Testing Capability from this software: Separate app has been made.
       Removed static constants DELTASTARTMARKER and DELTAENDMARKER
       Removed (completely) nested class DeltaLimits
       Removed private constructors:
           explicit LimitsFile(Int2Type<DeltaLimits::COMPARE>);
           explicit LimitsFile(Int2Type<DeltaLimits::STORE>);
       Removed auto_ptr<LF::Tests>'s - deltaCompare_ and deltaStore_

   ==============  
   07/27/04, sjn,
   ==============
     Added 'bool IsDeviationTest() const' to public interface.
     Removed: void NewDashNumber() and void NewTestType().
     Renamed: void NewFamilyNumber() to void Reload().
     These changes help to simplify general file i/o as well as allow for extensions
       to testing (ie; allow for deviations and engineering tests).
     Added static const char's: DELTASTARTMARKER and DELTAENDMARKER
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct LimitsFile : private NoCopy {

private:
    typedef FileTypes::LimitsFileType LF;

public:
    //=================
    // Public Typedefs
    //=================
    typedef std::vector<std::string> TestParameters;

    //==============
    // Public Enums
    //==============
    enum TestInput {        
        PRINTEDTESTNAME = 0,
        VIN,
        IOUT1,
        IOUT2,
        IOUT3,
        IOUT4,
        IOUT5,
        MINLIMIT,
        MAXLIMIT,
        UNITS,
        NEXTVIN,
        NEXTIOUT1,
        NEXTIOUT2,
        NEXTIOUT3,
        NEXTIOUT4,
        NEXTIOUT5,
        REFVALUE,
        BANDWIDTH,
        SYNCINVALUE,
        ISINHIBITEDPRIMARY,
        ISINHIBITEDSECONDARY,
        ISSYNCHRONIZED,
        SHORTCHANNEL1,
        SHORTCHANNEL2,
        SHORTCHANNEL3,
        SHORTCHANNEL4,
        SHORTCHANNEL5,
        PRETESTMISCELLANEOUS,
        MIDTESTMISCELLANEOUS,
        MISCDMM,
        MISCOHM,
        APSPRIMARY,
        APSSECONDARY,
        ACQCOUNT,
        UNDEFINED4,
        UNDEFINED3,
        UNDEFINED2,
        UNDEFINED1,
        SPEEDUP,
        SOFTWARETESTNAME
    };

    //========================
    // Start Public Interface
    //========================
    void operator++();  
    bool AtEnd();    
    std::vector<std::string> GetMiscDMM();
    std::vector<std::string> GetMidtestMisc();
    std::vector<std::string> GetPretestMisc();
    std::string GetRevisionLevel();
    std::string GetTestStepParameter(TestInput which);
    bool IsDeviationTest() const;
    static std::string Name();
    std::size_t NumberTests();
    void Reload();
    void RestartSameTest();
    //======================
    // End Public Interface 
    //======================

private:
    friend class SingletonType<LimitsFile>;
    LimitsFile();
    ~LimitsFile();
    bool operator=(const LimitsFile&);
    void checkArguments(const std::string& testType);

private:
	const long start_;
    const long number_;
    std::auto_ptr<LF> lf_;
    std::auto_ptr<LF::Tests> tests_;
    OperatorInterface* operatorInterface_;
    long stepNumber_;	    
    const std::string nil_;
    bool atEnd_;
    std::string revision_;
};

#endif // SPTS_LIMITS_FILE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
