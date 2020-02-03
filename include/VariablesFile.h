// Macro Guard
#ifndef VARIABLES_FILE_H
#define VARIABLES_FILE_H

// Files included
#include "ConverterOutput.h"
#include "LoadTraits.h"
#include "ProgramTypes.h"
#include "SingletonType.h"
#include "SPTSFiles.h"
#include "StandardFiles.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============  
   12/14/04, sjn,
   ==============
     Removed NewDashNumber() and NewFamilyNumber().
     Added Reload(): Assumes the jobs of NewDashNumber() and NewFamilyNumber().
     Added DoDUTDiagnostics() and DoDUTSanityChecks().
     These changes help to simplify general file i/o as well as allow for extensions
       to testing (ie; allow for deviations and engineering tests).
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

// Forward declaration
struct Converter;


struct VariablesFile : private NoCopy {
    //=================
    // Public typedefs
    //=================
    typedef std::set<std::string> RLLSet;
    typedef std::map<ProgramTypes::SetType, RLLSet> InnerMap; 
    typedef std::map<ConverterOutput::Output, LoadTraits::Channels> MapDut2Load;
    typedef FileTypes::VariablesFileType::JumperPullTable JumperPullTable;

    enum TemperatureOffsetType { COLDOFFSET, HOTOFFSET, ROOMOFFSET };

    //========================
    // Start Public Interface
    //========================
    bool DoDUTDiagnostics();
    bool DoDUTSanityChecks();
    std::string Fixture();
    std::vector<LoadTraits::Channels> GetAllLoadsUsed(long numOuts);
    JumperPullTable GetJumperPullIout(ConverterOutput::Output out) const;
    JumperPullTable GetJumperPullVout(ConverterOutput::Output out) const;
    std::vector< std::pair<ConverterOutput::Output, LoadTraits::Channels> >
                                                         GetLoadsMap(long numOuts);
    std::pair<bool, ProgramTypes::MType> GetOrientationOhms();
    std::pair< bool, std::vector<LoadTraits::Channels> > 
                                  GetParallelLoads(ConverterOutput::Output output);
    ProgramTypes::SetType GetTemperature();
    ProgramTypes::SetType GetTemperatureOffset(TemperatureOffsetType type);
    bool IsDeviationTest() const;
    bool IsJumperPullIout() const;
    bool IsJumperPullVout() const;
    std::pair<bool, ProgramTypes::SetType> LoadTransientTransitionTime();
    LoadTraits::Types LoadType();
    std::vector<std::string> MiscLines();
    std::string NoWirebondPull() const;
    std::pair<bool, ProgramTypes::SetType> PrimaryAuxSupply();
    void Reload();
    InnerMap RLoads(ConverterOutput::Output output);
    std::pair<bool, ProgramTypes::SetType> SecondaryAuxSupply();    
    std::pair<bool, ProgramTypes::PercentType> TODPercentage();
	//======================
    // End Public Interface
    //======================

private:
    friend class SingletonType<VariablesFile>;
    friend struct Converter;
    VariablesFile();
    ~VariablesFile();

private:
    std::string get(const std::string& str);
    ProgramTypes::SetType getHighestIin();
    ProgramTypes::SetType getHighestVin();
    ProgramTypes::SetType getHighLine();      
    ProgramTypes::MTypeContainer getIouts(long numOuts);    
	ProgramTypes::SetType getLowLine();
    ProgramTypes::SetType getMaxNoLoadIin();
	ProgramTypes::SetType getNominalLine();    
    ConverterOutput::Output getNumberOutputs();  
    long getInhibitLifecycles();  
    ProgramTypes::SetType getSyncAmplitude();   
    ProgramTypes::PercentType getSyncDutyCycle();  
    ProgramTypes::SetType getSyncOffset();  
    ProgramTypes::MTypeContainer getVouts(long numOuts);	

    bool isFrequencySplitter();
    bool isGroundedSync();
    bool isPrimaryInhibit();
    bool isRoomTemperature();
	bool isSyncOutPin();
	bool skipIinPard();
	bool UseLoadMeter();

private:
    typedef FileTypes::TestTypesFileType TTF;
    std::auto_ptr<TTF> testTypesFile_;
    std::auto_ptr<FileTypes::VariablesFileType> vf_;
    bool locked_;
    std::string dash_;
};

#endif // VARIABLES_FILE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
