// Macro Guard
#ifndef SPTS_DMM_SCPI
#define SPTS_DMM_SCPI

// Files included
#include "InstrumentTags.h"
#include "ProgramTypes.h"
#include "SCPI.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

template<>
struct SCPI<DMMWithMultiplexerTag> : public SCPI<IEEE488> { 
    static std::string ConfigureDCVolts(long channel, 
                                        const ProgramTypes::SetType& range = -1) 
        { 
            std::string s = closeMatrixRelay(channel);
            s += Concatenate();
            s += "CONF:VOLT:DC ";
            s += "(@" + convert<std::string>(channel) + ")"; 
            if ( range >= 0 ) {
                s += Concatenate();
                s += "VOLT:DC:RANG " + convert<std::string>(range);
                s += ", (@" + convert<std::string>(channel) + ")";
            }
            else {
                s += Concatenate();
                s += "VOLT:DC:RANGE:AUTO ON";
                s += ", (@" + convert<std::string>(channel) + ")";
            }
            return(s);
        }
    static std::string ConfigureOhms(long channel, 
                                     const ProgramTypes::SetType& range = -1) 
        { 
            std::string s = closeMatrixRelay(channel);
            s += Concatenate();
            s += "CONF:RES ";
            s += "(@" + convert<std::string>(channel) + ")";
            if ( range >= 0 ) {
                s += Concatenate();  
                s += "RESISTANCE:RANGE " + convert<std::string>(range);
                s += ", (@" + convert<std::string>(channel) + ")";   
            }            
            else {
                s += Concatenate();
                s += "RESISTANCE:RANGE:AUTO ON";
                s += ", (@" + convert<std::string>(channel) + ")";
            }
            return(s);
        }
    static std::string ConfigureTemperature(long channel) 
        { 
           std::string s = closeMatrixRelay(channel);
           s += Concatenate();
           s += "CONF:TEMP TCouple, (@" + convert<std::string>(channel) + ")";
           s += Concatenate();
           s += "UNIT:TEMP C";
           s += Concatenate();
           s += "SENSE:TEMP:TRAN:TC:TYPE T";
           return(s);
        }
	static std::string Initialize() 
        { return(Reset()); } 
    static std::string Measure() 
        { return("READ?"); }      
    static std::string WhatError()
		{ return("SYST:ERR?"); }

protected:
    static std::string closeMatrixRelay(long channel) 
        { 
            std::string toRtn = "ROUT:SCAN (@" + convert<std::string>(channel) + ")";
            toRtn += Concatenate();
            toRtn += "ROUT:MON:STAT ON";
            return(toRtn);
        }

protected:
	~SCPI<DMMWithMultiplexerTag>() { /* */ }  
};

#endif // SPTS_DMM_SCPI

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
