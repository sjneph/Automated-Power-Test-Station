// Macro Guard
#ifndef SPTS_INSTRUMENTTYPES_H 
#define SPTS_INSTRUMENTTYPES_H

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============
   03/02/05, sjn,
   ==============
   Removed enumeration values DCPROBEHIGHCURRENT and DCPROBELOWCURRENT and added value
     CURRENTPROBE to reflect station design changes.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

   
/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

struct InstrumentTypes {

    // Public Enum
    enum Types {
        APS,
        CURRENTPROBE,
        DMM,
        ELECTRONICLOAD,
        FUNCTIONGENERATOR,
        INPUTRELAYCONTROL,
        MISC,
        OUTPUTRELAYCONTROL,
        OSCOPE,
        PS1,
        PS2,
        PS3,
        RLL,
        SWITCHMATRIXDC,
        SWITCHMATRIXFILTER,
        SWITCHMATRIXRF,
        TEMPCONTROLLER
    };

protected:
    ~InstrumentTypes() { /* */ }
};

} // namespace SPTSInstrument

#endif // SPTS_INSTRUMENTTYPES_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
