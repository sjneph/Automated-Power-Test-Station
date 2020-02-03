// Files included
#include "Assertion.h"
#include "DateTime.h"
#include "GenericAlgorithms.h"
#include "SPTSException.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    typedef StationExceptionTypes::BadSystemClock  BadClock;
    typedef StationExceptionTypes::BadCommand      BadCommand;
    typedef StationExceptionTypes::UnexpectedState UnexpectedState;
} // unnamed namespace

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//===============
// CurrentDate()
//===============
std::string Date::CurrentDate() {
    // Warning:  This Date class will only work until 2038 --> needs
    //           modification for support beyond that year.

    // Warning: Low-level C-functions used here
    std::time_t t(0);
    Assert<UnexpectedState>(std::time(&t) != std::time_t(-1), Name());
    tm* gt = localtime(&t);
    std::stringstream s;        
    s << gt->tm_mon+1 << '/' << gt->tm_mday << '/' << 1900+gt->tm_year;
    return(s.str());
}

//========
// Name() 
//========
std::string Date::Name() {
    return("Date Structure");
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=============
// Constructor
//=============
Clock::Clock() : accumulated_(new std::vector<double>), timing_(false), paused_(false),
                 first_(true), timeStart_(std::time(0))
{ /* */ }

//============
// Destructor
//============
Clock::~Clock() 
{ /* */ }

//=========
// Clear()
//=========
void Clock::Clear() {
    timing_ = false;
    paused_ = false;
    first_  = true;
    accumulated_->erase(accumulated_->begin(), accumulated_->end());
}

//===============
// CurrentTime()
//===============
std::string Clock::CurrentTime() {
    // Warning: Low-level C-functions and buffers used here
    std::time_t t = std::time(0);
    char buf[20];
    strftime(buf, 20, "%X", localtime(&t));
    std::stringstream s(buf);
    return(s.str());
}

//===============
// ElapsedTime()
//===============
ProgramTypes::MType Clock::ElapsedTime() {
    Assert<BadCommand>(!first_, Name());

    bool toChange = false;
    if ( timing_ ) {
        if ( ! paused_ ) {
            PauseTiming();
            toChange = true;
        }
    }
  
    ProgramTypes::MType toRtn = 
          std::accumulate(accumulated_->begin(), accumulated_->end(), 0.0);
    if ( toChange )
        UnPause();
    return(toRtn);
}

//============
// IsTiming()
//============
bool Clock::IsTiming() {
    return(timing_ && (! paused_));
}

//========
// Name()
//========
std::string Clock::Name() {
    return("Clock Class");
}

//=========
// pause()
//=========
void Clock::pause(bool toPause) {
    if ( toPause ) {
        Assert<UnexpectedState>(timing_, Name());
        if ( ! paused_ ) {
            if ( accumulated_->size() < std::size_t(100) )
                accumulated_->push_back(std::difftime(std::time(0), timeStart_));
            else { // consolidate
                double d = std::accumulate(accumulated_->begin(), accumulated_->end(), 0.0);
                accumulated_.reset(new std::vector<double>);
                accumulated_->push_back(d);
            } // if-else
            paused_ = true;
        } // if
        return;
    }

    // toPause == false
    Assert<UnexpectedState>(paused_ && (!first_), Name());
    timeStart_ = std::time(0);
    paused_ = false;
}

//===============
// PauseTiming()
//===============
void Clock::PauseTiming() {
    pause(true);
}

//===============
// StartTiming()
//===============
void Clock::StartTiming() {
    Assert<UnexpectedState>(first_, Name());
    timeStart_ = std::time(0);
    timing_ = true;
    paused_ = false;
    first_  = false;
}

//==============
// StopTiming()
//==============
void Clock::StopTiming() {
    Assert<UnexpectedState>(timing_, Name());
    if ( ! paused_ )
        accumulated_->push_back(std::difftime(std::time(0), timeStart_));
    timing_ = false;
    paused_ = false;
}

//===========
// UnPause()
//===========
void Clock::UnPause() {
    pause(false);
}

//=============
// operator +=
//=============
Clock& Clock::operator+=(const Clock& other) {
    std::copy(other.accumulated_->begin(), 
              other.accumulated_->end(), 
              std::back_inserter(*accumulated_));
    return(*this);
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
