// Macro Guard
#ifndef SPTS_STATIONFILE_H
#define SPTS_STATIONFILE_H

// Files Included
#include "NoCopy.h"
#include "ProgramTypes.h"
#include "SingletonType.h"
#include "SPTSFiles.h"
#include "StandardFiles.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   11/20/05, sjn,
   ==============
     Removed PrintArchive() since we no longer go through a file.

   ==============
   07/13/05, sjn
   ==============
     Removed IinPardScopeScale() to reflect instrument changes in the station.

   ==============
   07/21/04, sjn,
   ==============
     Added LocalErrorArchive() --> Allow test sequences that result in a station error
        to be logged.  Added StationLocation() --> physical location of the station.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct StationFile : private NoCopy {		
    //==============
    // Public Enums
    //==============
    enum IinShunt { SMALLOHM, MIDOHM, BIGOHM };
    enum IinDCBoard { ONE, TWO, THREE };

    //========================
    // Start Public Interface
    //========================
    std::string BackupLocalStorage();
    ProgramTypes::MType GetShuntValue(IinDCBoard board, IinShunt whichShunt);
    std::pair<ProgramTypes::MType, ProgramTypes::MType>
                                      GetTemperatureTolerance(bool initialize);
    std::string LocalArchive();
    std::string LocalEngArchive();
    std::string LocalErrorArchive();
    std::string LocalGoldArchive();
    std::string LocalTestEngArchive();
    ProgramTypes::SetType MaxCurrentValue(IinDCBoard board, IinShunt whichShunt);
    bool NeedDegauss();    
    std::string OraclePath();
    std::string StationLocation();
    std::string StationName();
    std::string StationRevision();
    ProgramTypes::SetType TempCheckFreq();
    void UpdateDegauss();
    //======================
    // End Public Interface
    //======================

private:
    std::string name();
    std::string nextFileNumber();

private:
    friend class SingletonType<StationFile>;
	~StationFile();  
	StationFile();   

private:
    typedef FileTypes::StationFileType SF;
    std::auto_ptr<SF> sf_;
};

#endif // SPTS_STATIONFILE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
