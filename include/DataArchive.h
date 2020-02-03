// Macro Guard
#ifndef SPTS_DATAARCHIVE_H
#define SPTS_DATAARCHIVE_H

// Files included
#include "NoCopy.h"
#include "ProgramTypes.h"
#include "StandardFiles.h"

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct DataArchive : private NoCopy {
    // Public Interface
    explicit DataArchive(const ProgramTypes::MType& elapsedTime);
    std::string Name();

    // Friends
    friend std::ostream& operator<<(std::ostream& os, const DataArchive& fn);

private:
    void addData(const ProgramTypes::MType& elapsedTime);

private:
    std::auto_ptr< std::vector<std::string> > data_;
};

#endif // SPTS_DATAARCHIVE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
