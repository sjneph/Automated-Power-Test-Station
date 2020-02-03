// Macro Guard
#ifndef SPTS_SIGMAC4_LANGUAGE_H
#define SPTS_SIGMAC4_LANGUAGE_H

// Files included
#include "ProgramTypes.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct SigmaC4Language { 
    static void Clean(std::string& str) {
        std::size_t t = str.find(' ');
        str = str.substr(t+1);
    }
    static std::string ClearErrors() 
        { return("DC"); }
	static std::string ErrorBits()
        { return("1"); }
    static std::string GoImmediateMode()
        { return("SI"); }
    static std::string GoToTemperature(const ProgramTypes::SetType& temperature)
        { 
            ProgramTypes::SetType sn(temperature);
            sn.SetPrecision(1);
            return("RA " + sn.ValueStr() + " 0 1"); 
        }
	static std::string Identify()
		{ return("*IDN?"); }
    static std::string Initialize()
        { return(ClearErrors()); }
    static std::string IsDone()
        { return("RSA"); }
    static std::string IsError() 
        { return("RSA"); }
	static std::string OpCompleteBits()
        { return("4"); }
	static std::string Reset() 
        { return(Initialize()); }	
    static std::string SetControllingProbe() {
        { return("PN 1"); }
    }
    static std::string SetTemperatureLimits(const ProgramTypes::SetType& lowEnd,
                                            const ProgramTypes::SetType& hiEnd)
        { 
          ProgramTypes::SetType one(lowEnd), two(hiEnd);
          one.SetPrecision(1); two.SetPrecision(1);
          return("SL " + 
                 one.ValueStr() + 
                 " " + 
                 two.ValueStr()
                ); 
        }
    static std::string WhatError()
        { return("REA"); }
    static std::string WhatTemperature() 
        { return("PT 1"); }
protected:
	~SigmaC4Language() { /* */ }  
};


#endif // SPTS_SIGMAC4_LANGUAGE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
