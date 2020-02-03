// Macro Guard
#ifndef DATE_TIME_SPTS_H
#define DATE_TIME_SPTS_H

// Files included
#include "NoCopy.h"
#include "ProgramTypes.h"
#include "StandardFiles.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct Date {
    // Warning:  This Date struct will only work until 2038 
    //           --> needs modification for support beyond that year.
    static std::string CurrentDate();
    static std::string Name();
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct Clock : private NoCopy {
    //==================
    // Public Interface
    //==================
    Clock();
    ~Clock(); 
    void Clear();
    static std::string CurrentTime();
    ProgramTypes::MType ElapsedTime(); 
    bool IsTiming();
    std::string Name();
    void PauseTiming();
    void StartTiming();
    void StopTiming();
    void UnPause();
    Clock& operator+=(const Clock& other);

private:
    void pause(bool toPause);

private:
    bool timing_, paused_, first_;
    std::time_t timeStart_;
    std::auto_ptr< std::vector<double> > accumulated_;
};

#endif // DATE_TIME_SPTS_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
