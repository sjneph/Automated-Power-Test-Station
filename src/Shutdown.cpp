// Files included
#include "DialogBox.h"
#include "ProgramTypes.h"
#include "Shutdown.h"
#include "SingletonType.h"
#include "SPTS.h"
#include "SPTSException.h"
#include "VariablesFile.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============  
   07/21/04, sjn,
   ==============
     Added VariablesFile.h and ProgramTypes.h
     Added call to get room temperature offset from VariablesFile.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//============
// ShutDown()
//============
void ShutDown(bool resetTemp) {
    typedef StationExceptionTypes::UnsafeCondition UnsafeCondition;
    SpacePowerTestStation::SPTS* stationPtr 
                         = SingletonType<SpacePowerTestStation::SPTS>::Instance();

    static long counter = 0;
    try {
        if ( ++counter > 1 ) // prevent infinite recursion
            throw(false);
        
        typedef VariablesFile VF;
        typedef SingletonType<VF> VarFile;
        ProgramTypes::SetType offset = 0;
        try { 
            offset = absolute(VarFile::Instance()->GetTemperatureOffset(VF::ROOMOFFSET));
        } catch(...) { /* suppress */ }
        stationPtr->EmergencyShutdown(resetTemp, offset); // Shutdown station
        counter = 0;   // reset
    } catch(...) {
        try {
            DialogBox& screen = (*SingletonType<DialogBox>::Instance());
            screen <<  DialogBox::endl;
            screen <<  UnsafeCondition::GetDialog();
            screen <<  DialogBox::endl;
            screen << "Error Number: " << UnsafeCondition::GetValue();
            screen <<  DialogBox::endl;
            screen <<  DialogBox::endl;
            screen << "Unable to shutdown all of station's instruments!!!";
            screen <<  DialogBox::endl;
            screen << "Turn off all instruments manually before removing DUT!!!";
            screen <<  DialogBox::endl;
            screen.DisplayWarning();
        } catch(...) { /* can't do much more */ }
    } // catch-outer
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
