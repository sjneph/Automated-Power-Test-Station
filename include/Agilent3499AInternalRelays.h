// Macro Guard
#ifndef SPTS_Agilent3499AInternalRelays_SCPI
#define SPTS_Agilent3499AInternalRelays_SCPI



//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   03/09/05, sjn,
   ==============
   Redesigned layout of station:  Removed IINPARDLOWCURR and IINPARDHIGHCURR from
     RFRelay enumeration below.  Added IINPARD to same enumeration.

   ==============
   01/14/05, sjn,
   ==============
     Layout changes to the station require renaming, renumbering, deletions and 
       additions to the RFRelay and FilterRelay enumeration types below.  Parallel
       AC measurement capabilities are being removed from the station.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

struct Agilent3499AInternalRelays {
    enum RFRelay {
        VOUTPARD1            = 111,   
        LOADTRANSIENT1       = 100,
        VOUTPARD2            = 112,
        LOADTRANSIENT2       = 101,
        IINPARD              = 110,
        VOUTPARD3            = 210,  
        LOADTRANSIENT3       = 102,
        SYNCOUT              = 300,
        VOUTPARD4            = 211, 
        LOADTRANSIENT4       = 200,
        EXTENDEDTRANSIENT    = 103,
        EXTENDEDPARD         = 113,
        VOUTPARD5            = 212,
        LOADTRANSIENT5       = 201, 
        SYNCCHECK            = 301,
        LOADTRIGGER          = 310,
        PRIMARYINHIBITRISE   = 311,
        VINRISE              = 312
    }; // ACRelay

    enum FilterRelay {
        TWOMHZIN             = 111, 
        TWOMHZOUT            = 101,
        TENMHZIN             = 112, 
        TENMHZOUT	         = 102,
        PASSTHRUIN           = 113, 
        PASSTHRUOUT          = 103,
    }; // FilterRelay

    enum DCRelay {
        INPUTVOLTAGE         = 500,
        BOXID		         = 501,
        RESBOXID             = 502,
        APSPRIMARYVOLTAGE    = 503,
        IINDCONESMALLOHM     = 505,
        IINDCONEMEDIUMOHM    = 506,
        IINDCONEBIGOHM       = 507,
        IINDCTWOSMALLOHM     = 508,
        IINDCTWOMEDIUMOHM    = 509,
        IINDCTWOBIGOHM       = 510,
        IINDCTHREESMALLOHM   = 511,
        IINDCTHREEMEDIUMOHM  = 512,
        IINDCTHREEBIGOHM     = 513,
        PS1VOLTS             = 514,
        PS2VOLTS             = 515,
        PS3VOLTS             = 516,
        APSSYSTEM5V          = 517,
        APSSYSTEM12V         = 518,
        VOUTDC1              = 520,  
        IOUTDC1              = 521,
        VOUTDC2              = 522, 
        IOUTDC2              = 523, 
        VOUTDC3              = 524,  
        IOUTDC3              = 525,   
        VOUTDC4              = 526, 
        IOUTDC4              = 527,
        VOUTDC5              = 528,
        IOUTDC5              = 529,
        APSSECONDARYVOLTAGE  = 530,
        MISC1                = 531,
        MISC2                = 532,
        MISC3                = 533,
        MISC4                = 534,
        MISC5                = 535,
        MISC6                = 536,
        MISC7                = 537,
        MISC8                = 538,
        MISC9                = 539
    }; // DCRelay
};

} // namespace SPTSInstrument

#endif // SPTS_Agilent3499AInternalRelays_SCPI

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
