// Macro Guard
#ifndef SPTS_LOAD_SCPI
#define SPTS_LOAD_SCPI

// Files included
#include "InstrumentTags.h"
#include "ProgramTypes.h"
#include "SCPI.h"
	

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

template <> 
struct SCPI<ElectronicLoadTag> : public SCPI<IEEE488> {
	typedef ProgramTypes::SetType SetType;

	static std::string Initialize() 
		{ return(ClearErrors()); }
	static std::string InitializeChannel(long channel) {
			std::string modetype = SetCCMode(channel); 
			std::string val = convert<std::string>(0);

			std::string syntax;
			syntax += SetCCMode(channel);
            syntax += Concatenate();
			syntax += "CURR:SLEW MAX";	
            syntax += Concatenate();
			syntax += "CURR:RANG MAX";
            syntax += Concatenate();
			syntax += "CURR ";
			syntax +=  val;
            syntax += Concatenate();
			syntax += "INP:SHORT OFF";
            syntax += Concatenate();
			syntax += "TRAN:STATE OFF";
            syntax += Concatenate();
			syntax += "INP OFF";
			return(syntax); 
		}
	static std::string InputOff(long channel)
		{ return("CHAN " + convert<std::string>(channel) + ";:INPUT OFF"); }
	static std::string InputOn(long channel)
		{ return("CHAN " + convert<std::string>(channel) + ";:INPUT ON"); }
	static std::string MaxSlew()
		{ return("MAX"); }
    static std::string MeasureCurrent(long channel) 
        { return("CHAN " + convert<std::string>(channel) + ";:MEAS:CURR?"); }
    static std::string MeasureVoltage(long channel)
        { return("CHAN " + convert<std::string>(channel) + ";:MEAS:VOLT?"); }
	static std::string SetAmps(long channel, const SetType& value) { 
			std::string syntax;
			syntax  = "CHAN " + convert<std::string>(channel);
		    syntax += ";:MODE:CURR";	
		    syntax += ";:CURR"; 
		    syntax += ":RANG " + convert<std::string>(value);
		    syntax += ";:CURR " + convert<std::string>(value);
			return(syntax); 
		}
	static std::string SetCCMode(long channel) 
		{ return("CHAN " + convert<std::string>(channel) + ";:MODE:CURR"); }
	static std::string SetCRMode(long channel) 
		{ return("CHAN " + convert<std::string>(channel) + ";:MODE:RES"); }
	static std::string SetOhms(long channel, const SetType& value) { 
			std::string syntax;
			syntax  = "CHAN " + convert<std::string>(channel);
			syntax += ";:MODE:RES";	
			syntax += ";:RES"; 
			syntax += ":RANG " + convert<std::string>(value);
			syntax += ";:RES " + convert<std::string>(value);
			return(syntax); 
		}
	static std::string SetTransient(long channel, const SetType& fromVal, 
		                            const SetType& toVal, const std::string& rate) {
			std::string syntax;
			syntax  = "CHAN " + convert<std::string>(channel);
			syntax += ";:CURR:LEV " + convert<std::string>(fromVal);
			syntax += ";TLEV " + convert<std::string>(toVal);
			syntax += ";SLEW " + rate;
			syntax += ";:TRAN:MODE TOGG";
			syntax += ";:TRAN:STATE ON";
			syntax += ";:TRIG:SOUR HOLD";	
			return(syntax);
		}
	static std::string TransientOff(long channel)
		{ return("CHAN " + convert<std::string>(channel) + ";:TRAN:STATE OFF"); }
	static std::string TransientOn()
		{ return("TRIG:IMM"); }
	static std::string WhatError() 
		{ return("SYST:ERR?"); }
	
protected:
	~SCPI<ElectronicLoadTag>() { /* */ }  
};

#endif // SPTS_LOAD_SCPI

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
