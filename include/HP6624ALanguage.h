// Macro Guard
#ifndef SPTS_HP6624ALanguage_H
#define SPTS_HP6624ALanguage_H


// Files included
#include "GenericAlgorithms.h"
#include "InstrumentTags.h"
#include "ProgramTypes.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct HP6624ALanguage {    
    static std::string ClearErrors()
        { return("CLR"); }
    static void Clean(std::string& s) 
        { s = GetNumericInteger(s); }
    static std::string Concatenate() 
        { return(";"); }
    static std::string Identify()
        { return("ID?"); }
    static std::string Initialize() 
        { 
            std::string unmask = "254";
            return(
                   "CLR;SRQ 0;PON 0" +
                    Concatenate()    +
                   "UNMASK 1, "      +
                    unmask           +
                    Concatenate()    +
                   "UNMASK 2, "      +
                    unmask           +
                    Concatenate()    +
                   "UNMASK 3, "      +
                    unmask           +
                    Concatenate()    +
                   "UNMASK 4, "      +
                    unmask                     
                  ); 
        }        
    static std::string IsError()
        { return("ERR?"); }
    static std::string NoErrorCode() 
        { return("0"); }
    static std::string OutputOff(long channel)
        { return("OUT " + convert<std::string>(channel) + ",0"); }
    static std::string OutputOn(long channel)
        { return("OUT " + convert<std::string>(channel) + ",1"); }
    static std::string Reset()
        { return(Initialize()); }
    static std::string SetIin(long channel, const ProgramTypes::SetType& limit)
        { 
            return(
                    "ISET " + 
                     convert<std::string>(channel) + 
                    "," + 
                     convert<std::string>(limit)
                  ); 
        }
    static std::string SetVolts(long channel, const ProgramTypes::SetType& value)
        { 
            return(
                   "VSET " + 
                    convert<std::string>(channel) + 
                   "," + 
                    convert<std::string>(value) 
                  ); 
        }
    static std::string WhatError()
        { return("ERR?"); }

protected:
	~HP6624ALanguage() 
        { /* */ }  
};

#endif // SPTS_HP6624ALanguage_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
