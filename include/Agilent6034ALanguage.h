// Macro Guard
#ifndef SPTS_Agilent6034ALanguage_H
#define SPTS_Agilent6034ALanguage_H

// Files included
#include "Assertion.h"
#include "GenericAlgorithms.h"
#include "NumberBase.h"
#include "OScopeInterface.h"
#include "ProgramTypes.h"
#include "SCPI.h"
#include "SPTSException.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
  ==============
  06/23/05, sjn,
  ==============
    Added BitDelimmitter().  Removed inheritance of SCPI<IEEE488>
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

class Agilent6034ALanguage : public OScopeInterface {

protected:

    typedef StationExceptionTypes::UnexpectedState UnexpectedState; 
    typedef SCPI<IEEE488> BaseType;

    // Bandwidth overloads
    virtual std::string BandwidthOn(OScopeChannels::Channel chan) 
        { return("CHAN" + convert<std::string>(chan) + ":BWL ON"); }
    virtual std::string BandwidthOff(OScopeChannels::Channel chan) 
        { return("CHAN" + convert<std::string>(chan) + ":BWL OFF"); }
    
    virtual char BitDelimmitter() const
        { return(BaseType::BITDELIMMITTER); }
    
    virtual std::string ChannelOff(OScopeChannels::Channel chan)
        { return("CHAN" + convert<std::string>(chan) + ":DISP OFF"); } 
    virtual std::string ChannelOn(OScopeChannels::Channel chan)
        { return("CHAN" + convert<std::string>(chan) + ":DISP ON"); }

    virtual std::string Clean(const std::string& measure)
        { return(measure); }
    virtual std::string ClearErrors()
        { return(BaseType::ClearErrors()); }

    virtual std::string Concatenate() 
        { return(BaseType::Concatenate()); }    

    virtual std::string ErrorBits() 
        { return(BaseType::ErrorBits()); }

    virtual std::string Identify() 
        { return(BaseType::Identify()); }

	virtual std::string Average()
		{
			std::string avg =
                   "ACQ:TYPE AVER"                       + // Acquisition Type
                    Concatenate()                        +
                   "ACQ:COUN 8";                           // Acquisition Type
			return(avg);
		}

	virtual std::string Initialize()
        {
            std::string s = 
                    BaseType::Reset()                    + // Reset
                    Concatenate()                        +
                   "DISP:CLE"                            + // Clear display1
                    Concatenate()                        +
                   "CDIS"                                + // Clear display2
                    Concatenate()                        +
                   "MARK:MODE OFF"                       + // Cursors off
                    Concatenate()                        +
                    SetOffset(OScopeChannels::ONE, 0)    + // Ch1 offset
                    Concatenate()                        +
                    SetOffset(OScopeChannels::TWO, 0)    + // Ch2 offset
                    Concatenate()                        +
                    SetOffset(OScopeChannels::THREE, 0)  + // Ch3 offset
                    Concatenate()                        +
                    SetOffset(OScopeChannels::TRIGGER, 0)+ // Ch4 offset
                    Concatenate()                        +
                    ChannelOff(OScopeChannels::ONE)      + // Ch1 off
                    Concatenate()                        + 
                    ChannelOff(OScopeChannels::TWO)      + // Ch2 off
                    Concatenate()                        +
                    ChannelOff(OScopeChannels::THREE)    + // Ch3 off
                    Concatenate()                        +
                    ChannelOff(OScopeChannels::TRIGGER)  + // Ch4 off
                    Concatenate()                        +
                    SetCoupling(OScopeChannels::ONE,       // Ch1 coupling
                                Int2Type<DC1M>())        +
                    Concatenate()                        +
                    SetCoupling(OScopeChannels::TWO,       // Ch2 coupling
                                Int2Type<DC1M>())        + 
                    Concatenate()                        +
                    SetCoupling(OScopeChannels::THREE,     // Ch3 coupling
                                Int2Type<DC1M>())        + 
                    Concatenate()                        +
                    SetCoupling(OScopeChannels::TRIGGER,   // Ch4 coupling
                                Int2Type<DC1M>())        + 
                    Concatenate()                        +
                    SetTriggerMode(Int2Type<AUTO>())     + // Trigger Mode
                    Concatenate()                        +
                    SetHorzScale(1e-6)                   + // Horizontal Scale
                    Concatenate()                        +
                   "TIM:MODE MAIN"                       + // Main Horz Mode
                    Concatenate()                        +
                    SetTriggerSource(OScopeChannels::ONE)+ // Trigger Select
                    Concatenate()                        +
                   "CHAN1:UNIT VOLT"                     + // Ch1 Units Type
                    Concatenate()                        +
                   "CHAN2:UNIT VOLT"                     + // Ch2 Units Type
                    Concatenate()                        +
                   "CHAN3:UNIT VOLT"                     + // Ch3 Units Type
                    Concatenate()                        +
                   "CHAN4:UNIT VOLT"                     + // Ch4 Units Type
                    Concatenate()                        +
                   "ACQ:TYPE NORM"                       + // Acquisition Type
                    Concatenate()                        +
                   "TIM:REF LEFT"                        + // Trigger delay1
                    Concatenate()                        +
                   "TIM:POS 0"                           + // Trigger delay2
                    Concatenate()                        +
                   SetHorzScale(100e-6)                  + // Horizontal Scale
                    Concatenate()                        +
                   "DISP:PERS MIN"                       + // Persistance
                    Concatenate()                        +
                   "DISP:VECT ON"                        + // Vectors
                    Concatenate()                        +
                    Start(AUTO)                          + // Not 'Stopped'
                    Concatenate()                        +
                   "SYST:DSP 'Space/Power Test Station'";  // Message to display
            return(s);             
        }

    virtual bool IsClipping(const std::string& measure) 
        {
            ProgramTypes::MType m(measure), t = badMeasure();;
            return(m == t);
        }

    virtual std::string IsDone() 
        { return(BaseType::IsDone()); }

    // Error Check overloads
    virtual std::string IsError()
        { return(BaseType::IsError()); }
    virtual bool IsError(const std::string& measure) 
        {
            ProgramTypes::MType m(measure), t = badMeasure();
            return(m == t);
        }

    // Make Measurement overloads
    virtual std::string MakeMeasure(OScopeChannels::Channel chan, Int2Type<FREQUENCY>) 
        { return("MEAS:FREQ? CHAN" + convert<std::string>(chan)); }
    virtual std::string MakeMeasure(OScopeChannels::Channel chan, Int2Type<HIGHVALUE>)
        { return("MEAS:VTOP? CHAN" + convert<std::string>(chan)); }
    virtual std::string MakeMeasure(OScopeChannels::Channel chan, Int2Type<LOWVALUE>)
        { return("MEAS:VBAS? CHAN" + convert<std::string>(chan)); }
    virtual std::string MakeMeasure(OScopeChannels::Channel chan, Int2Type<MAXIMUMVALUE>) 
        { return("MEAS:VMAX? CHAN" + convert<std::string>(chan)); }
    virtual std::string MakeMeasure(OScopeChannels::Channel chan, Int2Type<MINIMUMVALUE>)
        { return("MEAS:VMIN? CHAN" + convert<std::string>(chan)); }
    virtual std::string MakeMeasure(OScopeChannels::Channel chan, Int2Type<PEAK2PEAK>)
        { return("MEAS:VPP? CHAN" + convert<std::string>(chan)); }
    virtual std::string MakeMeasure(OScopeChannels::Channel chan, Int2Type<TIME2LEVEL>,
                                  const ProgramTypes::SetType& level, SlopeType slope)
        {
            std::string toRtn = "MEAS:TVAL? " + level.ValueStr();
            toRtn += ",";
            toRtn += (slope == OScopeParameters::POSITIVE) ? "+1,CHAN" : "-1,CHAN";
            toRtn += convert<std::string>(chan);
            return(toRtn);
        }

    virtual long NumberHorizontalDivisions() const
        { return(10); }
    virtual long NumberVerticalDivisions() const
        { return(8); }
    virtual std::string OpCompleteBits() 
        { return(BaseType::OpCompleteBits()); }
    virtual NumberBase::Base RegisterReturnType()
        { return(NumberBase::DECIMAL); }

    // Channel Coupling overloads
    virtual std::string SetCoupling(OScopeChannels::Channel chan, Int2Type<AC1M>) 
        { return("CHAN" + convert<std::string>(chan) + ":COUP AC"); }
    virtual std::string SetCoupling(OScopeChannels::Channel chan, Int2Type<DC1M>) 
        { return("CHAN" + convert<std::string>(chan) + ":COUP DC"); }
    virtual std::string SetCoupling(OScopeChannels::Channel chan, Int2Type<GND>) 
        { return(SetCoupling(chan, Int2Type<DC1M>())); } // no gnd exists on this model

    virtual std::string SetHorzScale(const ProgramTypes::SetType& scale) 
        { return("TIM:RANG " + (10 * scale).ValueStr()); }

    // Memory Setting overloads
    /* 
         This model has no such abilities --> just send benign command and pretend
         that we changed the memory setting.  Must be very careful only to do this
         type of technique on items that are benign!
    */
    virtual std::string SetMemory(Int2Type<MIN>) 
        { return(dummyCommand()); }
    virtual std::string SetMemory(Int2Type<SMALL>) 
        { return(dummyCommand()); }
    virtual std::string SetMemory(Int2Type<MEDIUM>) 
        { return(dummyCommand()); }
    virtual std::string SetMemory(Int2Type<BIG>) 
        { return(dummyCommand()); }
    virtual std::string SetMemory(Int2Type<MAX>) 
        { return(dummyCommand()); }

    virtual std::string SetOffset(OScopeChannels::Channel chan, 
                                  const ProgramTypes::SetType& off)
        {
            ProgramTypes::SetType tmp = off;
            tmp *= -1;
            return(
                   "CHAN" + convert<std::string>(chan) +
                   ":OFFS " + tmp.ValueStr() + "V"
                  );
        }

    virtual std::string SetOpsComplete() 
        { return(BaseType::SetOpsComplete()); }

    // Trigger Coupling overloads
    virtual std::string SetTriggerCoupling(OScopeChannels::Channel chan, Int2Type<AC>) 
        { 
            return(
                   SetTriggerSource(chan) + Concatenate() +
                   "TRIG:COUP AC" + Concatenate() + "TRIG:REJ OFF"
                  );
        }
    virtual std::string SetTriggerCoupling(OScopeChannels::Channel chan, Int2Type<DC>) 
        {
            return(
                   SetTriggerSource(chan) + Concatenate() + 
                   "TRIG:COUP DC" + Concatenate() + "TRIG:REJ OFF"
                  );
        }
    virtual std::string SetTriggerCoupling(OScopeChannels::Channel chan, Int2Type<HFR>)
        {
            return(
                   SetTriggerSource(chan) + Concatenate() + 
                   "TRIG:COUP DC" + Concatenate() + "TRIG:REJ HF"
                  );
        }
    virtual std::string SetTriggerCoupling(OScopeChannels::Channel chan, Int2Type<LFR>)
        {
            return(
                   SetTriggerSource(chan) + Concatenate() + 
                   "TRIG:COUP DC" + Concatenate() + "TRIG:REJ LF"
                  );
        }

    // Trigger Level overloads
    virtual std::string SetTriggerLevel(OScopeChannels::Channel chan, 
                                    const ProgramTypes::SetType& level) 
        { 
            return(
                   "TRIG:LEV " + level.ValueStr() + 
                   ",CHAN" + convert<std::string>(chan)
                   );
        }

    // Trigger Mode overloads
    virtual std::string SetTriggerMode(Int2Type<AUTO>) 
        { return("TRIG:SWE AUTO"); }
    virtual std::string SetTriggerMode(Int2Type<NORMAL>)
        { return("TRIG:SWE NORM"); }

    // Trigger Slope overloads
    virtual std::string SetTriggerSlope(OScopeChannels::Channel, Int2Type<POSITIVE>) 
        { return("TRIG:SLOP POS"); }
    virtual std::string SetTriggerSlope(OScopeChannels::Channel, Int2Type<NEGATIVE>)
        { return("TRIG:SLOP NEG"); }

    // Trigger Source
    virtual std::string SetTriggerSource(OScopeChannels::Channel chan)
        { return("TRIG:SOUR CHAN" + convert<std::string>(chan)); }

    virtual std::string SetVertScale(OScopeChannels::Channel chan, 
                                     const ProgramTypes::SetType scale) 
        { return("CHAN" + convert<std::string>(chan) + ":SCAL " + scale.ValueStr()); }

    virtual std::string Start(TriggerMode)
        { return("RUN"); }

    virtual std::string Stop()
        { return("STOP"); }

    virtual long TotalRegisterBits() 
        { return(8); }

    virtual std::string WhatError()
	    { return("SYST:ERR?"); }

public:
	virtual ~Agilent6034ALanguage() 
        { /* */ }  

private:
    std::string dummyCommand() const
        /* Use very judiciously; only turns cursors off */
        { return("MARK:MODE OFF"); }
    
    ProgramTypes::MType badMeasure() const
        { return(ProgramTypes::MType("9.9e37")); }

    static std::string name() 
        { return("Agilent 54624A Language"); }
};

#endif // SPTS_Agilent54624ALanguage_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
