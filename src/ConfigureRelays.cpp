// Files included
#include "ConfigureRelays.h"
#include "SPTSException.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============  
   09/20/04, sjn,
   ==============
     Modified ConvertToFilterType() --> removed HUNDREDKHZ as possibility --> no longer
       selectable with new station layout.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    typedef StationExceptionTypes::BadArg BadArg;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=======================
// ConvertToFilterType()
//=======================
FilterSelects::FilterType ConvertToFilterType(const std::string& filterType) {
    std::string tmp = Uppercase(filterType);
    if ( tmp == "2MHZ" )
        return(FilterSelects::TWOMHZ);
    else if ( tmp == "10MHZ" )
        return(FilterSelects::TENMHZ);
    else if ( (tmp == "20MHZ") || (tmp == "PASSTHRU") )
        return(FilterSelects::PASSTHRU);
    throw(BadArg("ConvertToFilterType()"));
}

//=================
// ConvertToMisc()
//=================
ControlMatrixTraits::RelayTypes::MiscRelay ConvertToMisc(const std::string& miscValue) {
	std::string tmp = Uppercase(miscValue);
	if ( tmp == "MISC1" )
		return(ControlMatrixTraits::RelayTypes::MISC1);
	else if ( tmp == "MISC2" )
		return(ControlMatrixTraits::RelayTypes::MISC2);
	else if ( tmp == "MISC3" )
		return(ControlMatrixTraits::RelayTypes::MISC3);
	else if ( tmp == "MISC4" )
		return(ControlMatrixTraits::RelayTypes::MISC4);
	else if ( tmp == "MISC5" )
		return(ControlMatrixTraits::RelayTypes::MISC5);
	else if ( tmp == "MISC6" )
		return(ControlMatrixTraits::RelayTypes::MISC6);
	else if ( tmp == "MISC7" )
		return(ControlMatrixTraits::RelayTypes::MISC7);
	else if ( tmp == "MISC8" )
		return(ControlMatrixTraits::RelayTypes::MISC8);
	else if ( tmp == "MISC9" )
		return(ControlMatrixTraits::RelayTypes::MISC9);
	else if ( tmp == "MISC10" )
		return(ControlMatrixTraits::RelayTypes::MISC10);
	else if ( tmp == "MISC11" )
		return(ControlMatrixTraits::RelayTypes::MISC11);
	else if ( tmp == "MISC12" )
		return(ControlMatrixTraits::RelayTypes::MISC12);	
    throw BadArg("ConvertToMisc()");
}

//====================
// ConvertToMiscDMM()
//====================
SwitchMatrixTraits::RelayTypes::DCRelay ConvertToMiscDMM(const std::string& miscDMM) {
    std::string tmp = Uppercase(miscDMM);
    if ( tmp == "MISC1" )
        return(SwitchMatrixTraits::RelayTypes::MISC1);
    else if ( tmp == "MISC2" )
        return(SwitchMatrixTraits::RelayTypes::MISC2);
    else if ( tmp == "MISC3" )
        return(SwitchMatrixTraits::RelayTypes::MISC3);
    else if ( tmp == "MISC4" )
        return(SwitchMatrixTraits::RelayTypes::MISC4);
    else if ( tmp == "MISC5" )
        return(SwitchMatrixTraits::RelayTypes::MISC5);
    else if ( tmp == "MISC6" )
        return(SwitchMatrixTraits::RelayTypes::MISC6);
    else if ( tmp == "MISC7" )
        return(SwitchMatrixTraits::RelayTypes::MISC7);
    else if ( tmp == "MISC8" )
        return(SwitchMatrixTraits::RelayTypes::MISC8);
    else if ( tmp == "MISC9" )
        return(SwitchMatrixTraits::RelayTypes::MISC9);    
    throw BadArg("ConvertToMiscDMM()");    
}

//=================
// ConvertToRLLs()
//=================
ControlMatrixTraits::RelayTypes::ResistiveLoadLines 
                                      ConvertToRLL(const std::string& rllValue) {
	std::string tmp = Uppercase(rllValue);
	if ( tmp == "RLL1" )
		return(ControlMatrixTraits::RelayTypes::RLL1);
	else if ( tmp == "RLL2" )
		return(ControlMatrixTraits::RelayTypes::RLL2);
	else if ( tmp == "RLL3" )
		return(ControlMatrixTraits::RelayTypes::RLL3);
	else if ( tmp == "RLL4" )
		return(ControlMatrixTraits::RelayTypes::RLL4);
	else if ( tmp == "RLL5" )
		return(ControlMatrixTraits::RelayTypes::RLL5);
	else if ( tmp == "RLL6" )
		return(ControlMatrixTraits::RelayTypes::RLL6);
	else if ( tmp == "RLL7" )
		return(ControlMatrixTraits::RelayTypes::RLL7);
	else if ( tmp == "RLL8" )
		return(ControlMatrixTraits::RelayTypes::RLL8);
	else if ( tmp == "RLL9" )
		return(ControlMatrixTraits::RelayTypes::RLL9);
	else if ( tmp == "RLL10" )
		return(ControlMatrixTraits::RelayTypes::RLL10);
	else if ( tmp == "RLL11" )
		return(ControlMatrixTraits::RelayTypes::RLL11);
	else if ( tmp == "RLL12" )
		return(ControlMatrixTraits::RelayTypes::RLL12);
	else if ( tmp == "RLL13" )
		return(ControlMatrixTraits::RelayTypes::RLL13);
	else if ( tmp == "RLL14" )
		return(ControlMatrixTraits::RelayTypes::RLL14);
	else if ( tmp == "RLL15" )
		return(ControlMatrixTraits::RelayTypes::RLL15);
	else if ( tmp == "RLL16" )
		return(ControlMatrixTraits::RelayTypes::RLL16);
	else if ( tmp == "RLL17" )
		return(ControlMatrixTraits::RelayTypes::RLL17);
	else if ( tmp == "RLL18" )
		return(ControlMatrixTraits::RelayTypes::RLL18);
	else if ( tmp == "RLL19" )
		return(ControlMatrixTraits::RelayTypes::RLL19);
	else if ( tmp == "RLL20" )
		return(ControlMatrixTraits::RelayTypes::RLL20);
	throw BadArg("ConvertToRLL()");
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
