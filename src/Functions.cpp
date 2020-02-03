// Files included
#include "Assertion.h"
#include "Functions.h"
#include "SPTSException.h"

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//======================
// GetRoomTemperature()
//======================
ProgramTypes::MType GetRoomTemperature() {
    return(25);
}

//=========
// Pause()
//=========
void Pause(const ProgramTypes::SetType& timeInSeconds) {
    static ProgramTypes::SetType zero = 0;
    if ( timeInSeconds <= zero )
        return;
    ProgramTypes::SetType::ValueType time = timeInSeconds.Value();
    std::clock_t badClock = std::clock_t(-1);
    std::clock_t t1 = std::clock();         
    Assert<StationExceptionTypes::BadSystemClock>(t1 != badClock);
    bool done = false;
    while ( ! done ) {
        std::clock_t t2 = std::clock();
        if ( t2 == badClock )
            throw(StationExceptionTypes::BadSystemClock());     
        if ( (static_cast<double>(t2-t1)/CLOCKS_PER_SEC) >= time )
            done = true;
    } // while
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
