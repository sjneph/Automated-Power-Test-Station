// Macro Guard
#ifndef SPTS_Agilent3499AExternalRelays_SCPI
#define SPTS_Agilent3499AExternalRelays_SCPI

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   03/07/05, sjn,
   ==============
   Redesigned layout of station:  Removed IINPARDLOWCURR, IINPARDHIGHCURR and 
     PROBESELECT from InputRelay enumeration below.  Added IINPARD.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

struct Agilent3499AExternalRelays {
    enum InputRelay {
        CONNECTCOMMONS       = 505,
        IINDCA               = 506,
        IINDCB               = 507,
        IINPARD              = 508,
        SYNCENABLE           = 509,
        SYNCIN               = 510,
        SYNCCHECK            = 511,
        VINRISE              = 512, 
        PRIMARYINHIBITRISE   = 513,
        PRIMARYINHIBIT       = 514,
        PS1ISOLATION         = 515,
        PS2ISOLATION         = 516,
        PS3ISOLATION         = 517
    }; // InputRelay

    enum OutputRelay {
        SHORT1               = 500,  
        SHORT2               = 501,
        SHORT3               = 502,
        SHORT4               = 503,
        SHORT5               = 504,
        SECONDARYINHIBIT     = 520,
        LOADTRIGGER          = 521,
        VOUTPARD1            = 522,  
        LOADTRANSIENT1       = 523,
        VOUTPARD2            = 524,
        LOADTRANSIENT2       = 525, 
        VOUTPARD3            = 526,
        LOADTRANSIENT3       = 527,
        VOUTPARD4            = 528,
        LOADTRANSIENT4       = 529,
        VOUTPARD5            = 530,
        LOADTRANSIENT5       = 531
    }; // OutputRelay

    enum ResistiveLoadLines {
        RLL1  = 412, RLL2  = 413, RLL3  = 414,
        RLL4  = 415, RLL5  = 416, RLL6  = 417,
        RLL7  = 418, RLL8  = 419, RLL9  = 420,
        RLL10 = 421, RLL11 = 422, RLL12 = 423,
        RLL13 = 424, RLL14 = 425, RLL15 = 426,
        RLL16 = 427, RLL17 = 428, RLL18 = 429,
        RLL19 = 430, RLL20 = 431 
    }; // ResistiveLoadLines

    enum MiscRelay {
        MISC1  = 400, MISC2  = 401, MISC3  = 402,  
        MISC4  = 403, MISC5  = 404, MISC6  = 405,
        MISC7  = 406, MISC8  = 407, MISC9  = 408,
        MISC10 = 409, MISC11 = 410, MISC12 = 411
    }; // MiscRelay
};

} // namespace SPTSInstrument

#endif // SPTS_Agilent3499AExternalRelays_SCPI


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
