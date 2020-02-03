// Macro Guard
#ifndef SPTS_TekTCPA300_LANGUAGE_H
#define SPTS_TekTCPA300_LANGUAGE_H

/*
  NOTES:
  This instrument is completely tied into the Aardvark USB-to-I2C converter.

  This instrument also cannot tell you if it has an error: only if it needs degaussing.
*/

// Files included
#include "GenericAlgorithms.h"
#include "ProgramTypes.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

// This instrument is primitive and cannot take real concatenated commands
//  there are a lot of work arounds

struct TekTCPA300Language {                            
    static std::string Degauss()
        { return(convertToString(0x11)); }
    static std::string DegaussDone() 
        { return(convertToString(0x00)); }
    static ProgramTypes::SetType GetCurrentScale(const std::string& s)
        { return((IsAppropriateRange(s) ? 5 : 10)); } // Should be 5A/V
    static bool IsAppropriateCoupling(const std::string& s)
        { return(s == convertToString(0x01)); }
    static bool IsAppropriateRange(const std::string& s)
        { return(s == convertToString(0x02)); }
    static std::string IsError() 
        { return(convertToString(0x09)); } // Read degauss state
    static std::string NeedsDegauss() 
        { return(convertToString(0x02)); }
    static std::string ReadCoupling()
        { return(convertToString(0x07)); }
    static std::string ReadRange()
        { return(convertToString(0x08)); }
	static std::string Reset()
        { return(convertToString(0x16)); }
    static std::string ToggleCoupling()
        { return(convertToString(0x14)); }
    static std::string ToggleRange()
        { return(convertToString(0x15)); } // Toggle to low current range

private:
    static std::string convertToString(int hex)
        {
            char c = static_cast<char>(hex);
            std::string toRtn;
            toRtn += c;
            if ( hex == 0 )
                return("");
            return(toRtn);
        }

protected:
	~TekTCPA300Language() { /* */ }  
};


#endif // SPTS_TekTCPA300_LANGUAGE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
