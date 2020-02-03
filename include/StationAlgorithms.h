#ifndef SPTS_STATION_ALGORITHMS
#define SPTS_STATION_ALGORITHMS

// Files included
#include "ConverterOutput.h"
#include "ProgramTypes.h"
#include "Switch.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   05/20/05, sjn,
   ==============
     Added VerifyPowerConnection() to ensure Vin power connection is intact, when
       applicable.

   ==============
   05/10/05, sjn,
   ==============
     Added PostSequenceReset() to help reduce consecutive sequence state coupling.
     Changed CheckVoutsAtInhibit() to take a templated Exception Type.

   ==============
   03/20/05, sjn,
   ==============
     Added WaitSoakDUT() as a new requirement per GEN-029.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SpacePowerTestStation {

    //=======
    // Enums
    //=======
    enum IinDCType { DYNAMIC, NOINHIBIT, ISINHIBITED };

    //====================
    // Station Algorithms
    //====================
    template<typename ExcType>
    extern void CheckVouts(const ProgramTypes::PercentType& tolerance = 19);
    template<typename ExcType>
    extern void CheckVoutsAtInhibit();
    extern bool DUTOrientation();
    extern void EnforceTemperature(bool initialTemperature);
    extern bool FixtureIDCheck();
    extern void InitializeAlgorithms();
    extern void InitializeBaseTemperature(
                                   const ProgramTypes::SetType& toTemp,
                                   const ProgramTypes::SetType& posTolerance,
                                   const ProgramTypes::SetType& negTolerance
                                         );
    extern void InitializeStation(bool resetTemp = true);
    extern ProgramTypes::MType MeasureIinDC(IinDCType type = DYNAMIC);
    extern void MeasureIoutDC(ProgramTypes::MTypeContainer& iouts, 
                        ConverterOutput::Output output, bool loadMeasure = true);
    extern ProgramTypes::MType MeasureVinDC();
    extern void MeasureVoutDC(ProgramTypes::MTypeContainer& vouts, 
                        ConverterOutput::Output output, bool loadMeasure = true);
    extern void PostSequenceReset();
    extern void RampVin();
    extern void Short(const std::vector<ConverterOutput::Output>& v, Switch state);
    extern std::pair<bool, ProgramTypes::MType> VerifyPowerConnection(
                                                          const ProgramTypes::MType& maxOhms
                                                                     );
    extern void WaitSoakDUT();

} // namespace SpacePowerTestStation

// Microsoft 7.0 Compiler workaround
#include "StationAlgorithms.template"

#endif // SPTS_STATION_ALGORITHMS

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
