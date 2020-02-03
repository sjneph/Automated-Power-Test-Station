// Macro Guard
#ifndef SPTS_MeasurementTraits_H
#define SPTS_MeasurementTraits_H

// Files included
#include "FloatingNumber.h"
#include "SPTSException.h"
#include "StandardFiles.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct ProgramTypes {
	// Typedefs
	typedef FloatingNumber<double, false> MType;
	typedef FloatingNumber<double, false, -1> SetType;
    typedef FloatingNumber<int, 
                           true, 
                           0,
                           100, 
                           StationExceptionTypes::OutOfRange> PercentType;
    typedef FloatingNumber<int,
                           true,
                           -100,
                           100,
                           StationExceptionTypes::OutOfRange> PlusMinusPercentType;
	typedef std::vector<MType> MTypeContainer;
	typedef std::vector<SetType> SetTypeContainer;
	typedef std::pair<MType, MType> PairMType;
    typedef std::pair<SetType, SetType> PairSetType;

protected:
	~ProgramTypes() { /* */ }
};

#endif  // SPTS_MeasurementTraits_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
