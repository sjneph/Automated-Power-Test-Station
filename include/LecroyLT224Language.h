// Macro Guard
#ifndef SPTS_LT224Language_H
#define SPTS_LT224Language_H

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
   05/23/05, sjn,
   ==============
     Added BitDelimmitter()

   ==============
   10/11/04, sjn,
   ==============
     Modified IsClipping() and IsError() --> assertions >=1 are now >=2
     Modified ChannelOff() and ChannelOn() --> more terse now, but does same thing.
     Removed all SetGridStyle() overloads --> not generic to all scope vendors.
     Removed SetTriggerMode(Int2Type<SINGLE>) and SetTriggerMode(Int2Type<STOP>) virtual
        member functions --> not generic to all/most scope vendors.
     Removed BadArg Exception Type typedef --> not used.
     Changed everything in LecroyLT224Language from private to protected, so it can
        be inherited from in a more reasonable manner in the future.
     Changed SetTriggerSource() overloads from Single Source (SNG) to Edge triggering:
        this is more common among scopes and is supported by IVI requirements.
     Changed SCPI<IEEE488> inheritence from private to protected.
     Removed all: (Getting rid of ExtTrig)
        virtual std::string SetTriggerCoupling(OScopeChannels::ExtTrig, other)
        virtual std::string SetTriggerLevel(OScopeChannels::ExtTrig, other)
        virtual std::string SetTriggerSlope(OScopeChannels::ExtTrig, other)
        virtual std::string SetTriggerSource(OScopeChannels::ExtTrig)
     Removed all MakeMeasure() methods associated directly with delay measurements.
     Added MakeMeasure() method associated with TIME2LEVEL.
     Removed: (New station layout has dedicated 50-ohm line)
        virtual std::string SetCoupling(OScopeChannels::Channel chan, Int2Type<DCFIFTY>)
     Changed all OScopeChannels::FOUR's to OScopeChannels::TRIGGER to reflect station
        changes.
     Added Start() and Stop() functions.     
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

class LecroyLT224Language : protected SCPI<IEEE488>, public OScopeInterface { 

protected:

    typedef StationExceptionTypes::UnexpectedState UnexpectedState; 
    typedef SCPI<IEEE488> BaseType;

    // Bandwidth overloads
    virtual std::string BandwidthOn(OScopeChannels::Channel chan) 
        { return("BWL C" + convert<std::string>(chan) + ",ON"); }
    virtual std::string BandwidthOff(OScopeChannels::Channel chan) 
        { return("BWL C" + convert<std::string>(chan) + ",OFF"); }

    virtual char BitDelimmitter() const
        { return(BaseType::BITDELIMMITTER); }
    
    virtual std::string ChannelOff(OScopeChannels::Channel chan)
        { return("C" + convert<std::string>(chan) + ":TRA OFF"); } 
    virtual std::string ChannelOn(OScopeChannels::Channel chan)
        { return("C" + convert<std::string>(chan) + ":TRA ON"); }

    virtual std::string Clean(const std::string& measure)
        { 
            std::vector<std::string> toRtn = SplitString(measure, ',');
            Assert<UnexpectedState>(toRtn.size() >= 1, name());
            return(toRtn.at(1));
        }
    virtual std::string ClearErrors()
        { return(BaseType::ClearErrors()); }

    virtual std::string Concatenate() 
        { return(";"); }    

    virtual std::string ErrorBits() 
        { return(BaseType::ErrorBits()); }

    virtual std::string Identify() 
        { return(BaseType::Identify()); }

	virtual std::string Initialize() 
        {
            std::string s = 
                    BaseType::Reset()                    + // Reset
                    Concatenate()                        +
                   "CHDR OFF"                            + // Communication header format
                    Concatenate()                        +
                   "CHLP EO"                             + // Remote Assistance
                    Concatenate()                        +
                   "CFMT DEF9, WORD, BIN"                + // Communication format
                    Concatenate()                        +
                   "CORD HI"                             + // MSB's first, LSB's last
                    Concatenate()                        +
                   "CLM M1"                              + // Clear memory
                    Concatenate()                        +
                   "CLM M2"                              + // Clear memory
                    Concatenate()                        +
                   "CLM M3"                              + // Clear memory
                    Concatenate()                        +
                   "CLM M4"                              + // Clear memory
                    Concatenate()                        +
                   "CRMS OFF"                            + // Cursors off
                    Concatenate()                        +
                   "DPNT NORMAL"                         + // Data points display
                    Concatenate()                        + 
                   "DISP OFF"                            + // Display off
                    Concatenate()                        +
                   "INTS TRACE, 90"                      + // Trace intensity
                    Concatenate()                        +
                   "INTS GRID, 90"                       + // Grid intensity
                    Concatenate()                        +
                   "ILVD OFF"                            + // Interleaved feature 
                    Concatenate()                        +
                   "SCSV NO"                             + // Screen saver
                    Concatenate()                        +
                    SetMemory(Int2Type<MEDIUM>())        + // Set memory size
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
                                Int2Type<AC1M>())        +
                    Concatenate()                        +
                    SetCoupling(OScopeChannels::TWO,       // Ch2 coupling
                                Int2Type<AC1M>())        + 
                    Concatenate()                        +
                    SetCoupling(OScopeChannels::THREE,     // Ch3 coupling
                                Int2Type<AC1M>())        + 
                    Concatenate()                        +
                    SetCoupling(OScopeChannels::TRIGGER,   // Ch4 coupling
                                Int2Type<AC1M>())        + 
                    Concatenate()                        +
                    SetTriggerMode(Int2Type<AUTO>())     + // Trigger Mode
                    Concatenate()                        +
                    SetHorzScale(1e-6)                   + // Horizontal Scale
                    Concatenate()                        +
                    SetTriggerSource(OScopeChannels::ONE)+ // Trigger Select
                    Concatenate()                        +
                   "TRDL 10.00 PCT"                      + // Trigger delay
                    Concatenate()                        +
                   "TOPA OPAQUE"                         + // Opaque traces
                    Concatenate()                        +
                   "PACL"                                + // Parameter clear
                    Concatenate()                        +
                   "PERS OFF"                            + // Persistance
                    Concatenate()                        +
                   "PELT OFF"                            + // Persist last
                    Concatenate()                        + 
                   "MGAT OFF"                            + // Measure gate feature
                    Concatenate()                        +
                   "FSCR ON"                             + // Screen display
                    Concatenate()                        +
                   "GRID QUAD"                           + // Quad grid style
                    Concatenate()                        +
                   "GBWL OFF"                            + // Global bandwidth off 
                    Concatenate()                        +
                   "CSCH U1"                             + // Color scheme U1 (user scheme 1)
                    Concatenate()                        +
                   "PECL COLOR_GRADED"                   + // Persist color
                    Concatenate()                        + 
                   "COLR C1,GREEN"                       + // Channel 1 green
                    Concatenate()                        +
                   "COLR TA,GREEN"                       + // Trace A green
                    Concatenate()                        +
                   "COLR C2,RED"                         + // Channel 2 red
                    Concatenate()                        +
                   "COLR TB,RED"                         + // Trace B red
                    Concatenate()                        +
                   "COLR C3,CYAN"                        + // Channel 3 cyan
                    Concatenate()                        +
                   "COLR TC,CYAN"                        + // Trace C cyan
                    Concatenate()                        +
                   "COLR C4,YELLOW"                      + // Channel 4 yellow
                    Concatenate()                        +
                   "COLR TD,YELLOW"                      + // Trace D yellow
                    Concatenate()                        +
                   "COLR GRID,LTGRAY"                    + // Grid light gray
                    Concatenate()                        +
                   "COLR TEXT,LTGRAY"                    + // Text light gray
                    Concatenate()                        +
                   "COLR CURSOR,DKCYAN"                  + // Cursor dark cyan
                    Concatenate()                        +
                   "COLR NEUTRAL,GRAY"                   + // Neutral gray
                    Concatenate()                        +
                   "COLR BACKGND,BLACK"                  + // Background black
                    Concatenate()                        +
                   "COLR WARNING,RED"                    + // Warning red
                    Concatenate()                        +
                   "COLR OVERLAYS,BLACK"                 + // Overlays black   
                    Concatenate()                        +
                    Start(AUTO)                          + // No 'Stopped'
                    Concatenate()                        +
                   "MSG 'Space/Power Test Station'";       // Message to display
            return(s);             
        }

    virtual bool IsClipping(const std::string& measure) 
        {             
            std::vector<std::string> toRtn = SplitString(measure, ',');
            Assert<UnexpectedState>(toRtn.size() >= 2, name());
            if ( toRtn.at(2) == "OF" )
                return(true);
            else if ( toRtn.at(2) == "OU" )
                return(true);
            else if ( toRtn.at(2) == "UF" )
                return(true);
            return(false);
        }
    virtual std::string IsDone() 
        { return(BaseType::IsDone()); }

    // Error Check overloads
    virtual std::string IsError()
        { return(BaseType::IsError()); }
    virtual bool IsError(const std::string& measure) 
        {
            std::vector<std::string> toRtn = SplitString(measure, ',');
            Assert<UnexpectedState>(toRtn.size() >= 2, name());
            return((toRtn[2] != "OK") && (toRtn[2] != "AV") && (toRtn[2] != "NP"));
        }

    // Make Measurement overloads
    virtual std::string MakeMeasure(OScopeChannels::Channel chan, Int2Type<FREQUENCY>) 
        { return("C" + convert<std::string>(chan) + ":PAVA? FREQ"); }
    virtual std::string MakeMeasure(OScopeChannels::Channel chan, Int2Type<HIGHVALUE>)
        { return("C" + convert<std::string>(chan) + ":PAVA? TOP"); }
    virtual std::string MakeMeasure(OScopeChannels::Channel chan, Int2Type<LOWVALUE>)
        { return("C" + convert<std::string>(chan) + ":PAVA? BASE"); }
    virtual std::string MakeMeasure(OScopeChannels::Channel chan, Int2Type<MAXIMUMVALUE>) 
        { return("C" + convert<std::string>(chan) + ":PAVA? MAX"); }
    virtual std::string MakeMeasure(OScopeChannels::Channel chan, Int2Type<MINIMUMVALUE>)
        { return("C" + convert<std::string>(chan) + ":PAVA? MIN"); }
    virtual std::string MakeMeasure(OScopeChannels::Channel chan, Int2Type<PEAK2PEAK>)
        { return("C" + convert<std::string>(chan) + ":PAVA? PKPK"); }
    virtual std::string MakeMeasure(OScopeChannels::Channel chan, Int2Type<TIME2LEVEL>,
                                    const ProgramTypes::SetType& level, SlopeType slope)
        {
            std::string slopePolarity;
            if ( slope == OScopeParameters::POSITIVE )
                slopePolarity = "POS";
            else
                slopePolarity = "NEG";

            static std::string hysteresis = "0.1";

            std::string toRtn =  "PACU 1,TLEV,C";
                        toRtn += convert<std::string>(chan);
                        toRtn += ",";
                        toRtn += slopePolarity;
                        toRtn += ",";
                        toRtn += level.ValueStr();
                        toRtn += ",";
                        toRtn += hysteresis;
                        toRtn += Concatenate();
                        toRtn += "PAVA? CUST1";
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
        { return("C" + convert<std::string>(chan) + ":CPL A1M"); }
    virtual std::string SetCoupling(OScopeChannels::Channel chan, Int2Type<DC1M>) 
        { return("C" + convert<std::string>(chan) + ":CPL D1M"); }
    virtual std::string SetCoupling(OScopeChannels::Channel chan, Int2Type<GND>) 
        { return("C" + convert<std::string>(chan) + ":CPL GND"); }

    virtual std::string SetHorzScale(const ProgramTypes::SetType& scale) 
        { return("TDIV " + scale.ValueStr()); }

    // Memory Setting overloads
    virtual std::string SetMemory(Int2Type<MIN>) 
        { return("MSIZ " + ProgramTypes::SetType(500).ValueStr()); }
    virtual std::string SetMemory(Int2Type<SMALL>) 
        { return("MSIZ " + ProgramTypes::SetType(2500).ValueStr()); }
    virtual std::string SetMemory(Int2Type<MEDIUM>) 
        { return("MSIZ " + ProgramTypes::SetType(10e3).ValueStr()); }
    virtual std::string SetMemory(Int2Type<BIG>) 
        { return("MSIZ " + ProgramTypes::SetType(50e3).ValueStr()); }
    virtual std::string SetMemory(Int2Type<MAX>) 
        { return("MSIZ " + ProgramTypes::SetType(100e3).ValueStr()); }

    virtual std::string SetOffset(OScopeChannels::Channel chan, 
                                  const ProgramTypes::SetType& off)
        {
            return(
                   "C" + convert<std::string>(chan) +
                   ":OFST " + off.ValueStr() + "V"
                  );
        }

    virtual std::string SetOpsComplete() 
        { return(BaseType::SetOpsComplete()); }

    // Trigger Coupling overloads
    virtual std::string SetTriggerCoupling(OScopeChannels::Channel chan, Int2Type<AC>) 
        { return("C" + convert<std::string>(chan) + ":TRCP AC"); }
    virtual std::string SetTriggerCoupling(OScopeChannels::Channel chan, Int2Type<DC>) 
        { return("C" + convert<std::string>(chan) + ":TRCP DC"); }
    virtual std::string SetTriggerCoupling(OScopeChannels::Channel chan, Int2Type<HFR>)
        { return("C" + convert<std::string>(chan) + ":TRCP HFREJ"); }
    virtual std::string SetTriggerCoupling(OScopeChannels::Channel chan, Int2Type<LFR>)
        { return("C" + convert<std::string>(chan) + ":TRCP LFREJ"); }

    // Trigger Level
    virtual std::string SetTriggerLevel(OScopeChannels::Channel chan, 
                                    const ProgramTypes::SetType& level) 
        { return("C" + convert<std::string>(chan) + ":TRLV " + level.ValueStr()); }

    // Trigger Mode overloads
    virtual std::string SetTriggerMode(Int2Type<AUTO>) 
        { return("TRMD AUTO"); }
    virtual std::string SetTriggerMode(Int2Type<NORMAL>)
        { return("TRMD NORM"); }

    // Trigger Slope overloads
    virtual std::string SetTriggerSlope(OScopeChannels::Channel chan, Int2Type<POSITIVE>) 
        { return("C" + convert<std::string>(chan) + ":TRSL POS"); }
    virtual std::string SetTriggerSlope(OScopeChannels::Channel chan, Int2Type<NEGATIVE>)
        { return("C" + convert<std::string>(chan) + ":TRSL NEG"); }

    // Trigger Source
    virtual std::string SetTriggerSource(OScopeChannels::Channel chan)
        { return("TRSE EDGE, SR, C" + convert<std::string>(chan)); }

    virtual std::string SetVertScale(OScopeChannels::Channel chan, 
                                     const ProgramTypes::SetType scale) 
        { return("C" + convert<std::string>(chan) + ":VDIV " + scale.ValueStr()); }

    virtual std::string Start(TriggerMode)
        { return(dummyCommand()); }

    virtual std::string Stop()
        { return(dummyCommand()); }

    virtual long TotalRegisterBits() 
        { return(8); }

    virtual std::string WhatError()
	    { return("CHL? CLR"); }

public:
	virtual ~LecroyLT224Language() 
        { /* */ }  

private:
    static std::string dummyCommand()
        /* Use very judiciously; only changes text color */
        { return("COLR TEXT,LTGRAY"); }

    static std::string name() 
        { return("LT224 Language"); }
};

#endif // SPTS_LT224Language_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
