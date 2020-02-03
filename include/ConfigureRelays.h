// Files included
#include "ControlMatrixTraits.h"
#include "FilterSelects.h"
#include "StandardFiles.h"
#include "SwitchMatrixTraits.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=======================
// ConvertToFilterType()
//=======================
extern FilterSelects::FilterType ConvertToFilterType(const std::string& filterType);

//=================
// ConvertToMisc()
//=================
extern ControlMatrixTraits::RelayTypes::MiscRelay 
                                      ConvertToMisc(const std::string& miscValue);

//====================
// ConvertToMiscDMM()
//====================
extern SwitchMatrixTraits::RelayTypes::DCRelay 
                                      ConvertToMiscDMM(const std::string& miscDMM);

//================
// ConvertToRLL()
//================
extern ControlMatrixTraits::RelayTypes::ResistiveLoadLines 
                                          ConvertToRLL(const std::string& rllValue);

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
