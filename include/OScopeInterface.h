#ifndef SPTS_SCOPE_INTERFACE_H
#define SPTS_SCOPE_INTERFACE_H

// Files included
#include "NumberBase.h"
#include "ProgramTypes.h"
#include "OScopeParameters.h"
#include "SPTSException.h"
#include "Switch.h"


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
    Removed Measure() overload and its corresponding MakeMeasure() overload for 
        OScopeMeasurements::DELAY.  Not directly supported here any longer.
    Added std::string Measure(Channel chan, const ProgramTypes::SetType& refLevel,
                              MeasurementType type, SlopeType slope)
       and its corresponding MakeMeasure() pure virtual function (--> used for
       OScopeMeasurements::TIME2LEVEL).
    Added NumberHorizontalDivisions(), Start() and Stop() pure virtual functions.
    Removed SetGridStyle() member function --> not generic to all scope vendors.
    Removed several pure virtual functions for the same reason:
       virtual std::string SetGridStyle(Int2Type<ONE>)                      = 0;
       virtual std::string SetGridStyle(Int2Type<TWO>)                      = 0;
       virtual std::string SetGridStyle(Int2Type<FOUR>)                     = 0;
       virtual std::string SetTriggerMode(Int2Type<SINGLE>)                 = 0; 
       virtual std::string SetTriggerMode(Int2Type<STOP>)                   = 0;
       virtual std::string MakeMeasure(Int2Type<DELAY>, other parameters)   = 0;
       virtual std::string SetCoupling(other parameters, Int2Type<DCFIFTY>) = 0;
    Removed several member functions to get rid of external trigger capabilities:
        std::string SetTriggerCoupling(OScopeChannels::ExtTrig trigger, other)
        virtual std::string SetTriggerLevel(OScopeChannels::ExtTrig trig, other)
        std::string SetTriggerSlope(OScopeChannels::ExtTrig trig, other)
        virtual std::string SetTriggerSource(OScopeChannels::ExtTrig trig)
        virtual std::string SetTriggerCoupling(OScopeChannels::ExtTrig, other) = 0;
        virtual std::string SetTriggerSlope(OScopeChannels::ExtTrig trig, other) = 0;
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct OScopeInterface : public OScopeParameters, public OScopeMeasurements {

    typedef StationExceptionTypes::BadArg BadArg;

    virtual char BitDelimmitter() const = 0;

    virtual std::string ClearErrors() = 0;

    virtual std::string Concatenate() = 0;

    std::string ChannelState(OScopeChannels::Channel chan, Switch state) 
        {
            if ( state == ON )
                return(ChannelOn(chan));
            return(ChannelOff(chan));
        }

    std::string CleanMeasure(const std::string& measure) 
        { return(Clean(measure)); }

    virtual std::string ErrorBits()  = 0;
    virtual std::string Identify()   = 0;
	virtual std::string Initialize() = 0;
    virtual bool IsClipping(const std::string& value) = 0;
    virtual std::string IsDone()     = 0;
    virtual std::string IsError()    = 0;
    virtual bool IsError(const std::string& value) = 0;

    std::string Measure(OScopeChannels::Channel chan, MeasurementType m)
        {
            switch(m) {
                case FREQUENCY:
                    return(MakeMeasure(chan, Int2Type<FREQUENCY>()));
                case HIGHVALUE:
                    return(MakeMeasure(chan, Int2Type<HIGHVALUE>()));
                case LOWVALUE:
                    return(MakeMeasure(chan, Int2Type<LOWVALUE>()));
                case MAXIMUMVALUE:
                    return(MakeMeasure(chan, Int2Type<MAXIMUMVALUE>()));
                case MINIMUMVALUE:
                    return(MakeMeasure(chan, Int2Type<MINIMUMVALUE>()));
                case PEAK2PEAK:
                    return(MakeMeasure(chan, Int2Type<PEAK2PEAK>()));
            };
            throw(BadArg(ScopeInterfaceName()));
        }

    std::string Measure(OScopeChannels::Channel chan,
                        const ProgramTypes::SetType& refLevel,
                        MeasurementType type, SlopeType slope) {
        if ( type != TIME2LEVEL )
            throw(BadArg(ScopeInterfaceName()));
        return(MakeMeasure(chan, Int2Type<TIME2LEVEL>(), refLevel, slope));
    }

    virtual long NumberHorizontalDivisions() const = 0;
    virtual long NumberVerticalDivisions() const = 0;
    virtual std::string OpCompleteBits() = 0;
    virtual NumberBase::Base RegisterReturnType() = 0;

    std::string SetBandwidth(OScopeChannels::Channel chan, Switch state) 
        { return(state == ON ? BandwidthOn(chan) : BandwidthOff(chan)); }

    std::string SetCoupling(OScopeChannels::Channel chan, CouplingType type) 
        {
            switch(type) {
                case AC1M:
                    return(SetCoupling(chan, Int2Type<AC1M>()));
                case DC1M:
                    return(SetCoupling(chan, Int2Type<DC1M>()));
                case GND:
                    return(SetCoupling(chan, Int2Type<GND>()));
            }; 
            throw(BadArg(ScopeInterfaceName()));
        }

    virtual std::string SetHorzScale(const ProgramTypes::SetType& scale) = 0;

    std::string SetMemory(MemorySetting ms) {
        switch(ms) {
            case MIN: 
                return(SetMemory(Int2Type<MIN>()));
            case SMALL:
                return(SetMemory(Int2Type<SMALL>()));
            case MEDIUM:
                return(SetMemory(Int2Type<MEDIUM>()));
            case BIG:
                return(SetMemory(Int2Type<BIG>()));
            case MAX:
                return(SetMemory(Int2Type<MAX>()));
        };
        throw(BadArg(ScopeInterfaceName()));
    }
    
    virtual std::string SetOffset(OScopeChannels::Channel chan, 
                                  const ProgramTypes::SetType& off) = 0;

    virtual std::string SetOpsComplete() = 0;

    std::string SetTriggerCoupling(OScopeChannels::Channel chan, 
                                   TriggerCouplingType type) { 
        switch(type) {
            case AC:
                return(SetTriggerCoupling(chan, Int2Type<AC>()));
            case DC:
                return(SetTriggerCoupling(chan, Int2Type<DC>()));
            case HFR:
                return(SetTriggerCoupling(chan, Int2Type<HFR>()));
            case LFR:
                return(SetTriggerCoupling(chan, Int2Type<LFR>()));
        };
        throw(BadArg(ScopeInterfaceName()));
    }

    virtual std::string SetTriggerLevel(OScopeChannels::Channel chan, 
                                        const ProgramTypes::SetType& level) = 0;
        
    std::string SetTriggerMode(TriggerMode mode) 
        {
            switch(mode) {
                case AUTO:
                    return(SetTriggerMode(Int2Type<AUTO>()));
                case NORMAL:
                    return(SetTriggerMode(Int2Type<NORMAL>()));
            };
            throw(BadArg(ScopeInterfaceName()));
        }

    std::string SetTriggerSlope(OScopeChannels::Channel chan, SlopeType slope) 
        {
            switch(slope) {
                case POSITIVE:
                    return(SetTriggerSlope(chan, Int2Type<POSITIVE>()));
                case NEGATIVE:
                    return(SetTriggerSlope(chan, Int2Type<NEGATIVE>()));
            }; 
            throw(BadArg(ScopeInterfaceName()));
        }

    virtual std::string SetTriggerSource(OScopeChannels::Channel chan) = 0;

    virtual std::string SetVertScale(OScopeChannels::Channel chan, 
                                     const ProgramTypes::SetType scale) = 0;

    virtual std::string Start(TriggerMode mode) = 0;
    virtual std::string Stop() = 0;

    virtual long TotalRegisterBits() = 0;

    virtual std::string WhatError() = 0;

	virtual ~OScopeInterface() 
        { /* */ }

private:
    std::string ScopeInterfaceName() 
        { return("Scope Interface"); }

private:
    // template method receiving virtual member functions
    virtual std::string BandwidthOn(OScopeChannels::Channel chan)  = 0;
    virtual std::string BandwidthOff(OScopeChannels::Channel chan) = 0;

    virtual std::string ChannelOff(OScopeChannels::Channel chan) = 0;
    virtual std::string ChannelOn(OScopeChannels::Channel chan)  = 0;

    virtual std::string Clean(const std::string& measure)
        { return(measure); }

    virtual std::string MakeMeasure(OScopeChannels::Channel, 
                                    Int2Type<FREQUENCY>)    = 0;
    virtual std::string MakeMeasure(OScopeChannels::Channel,
                                    Int2Type<HIGHVALUE>)    = 0;
    virtual std::string MakeMeasure(OScopeChannels::Channel,
                                    Int2Type<LOWVALUE>)     = 0;
    virtual std::string MakeMeasure(OScopeChannels::Channel, 
                                    Int2Type<MAXIMUMVALUE>) = 0;
    virtual std::string MakeMeasure(OScopeChannels::Channel, 
                                    Int2Type<MINIMUMVALUE>) = 0;
    virtual std::string MakeMeasure(OScopeChannels::Channel,
                                    Int2Type<PEAK2PEAK>)    = 0;
    virtual std::string MakeMeasure(OScopeChannels::Channel,
                                    Int2Type<TIME2LEVEL>,
                                    const ProgramTypes::SetType&,
                                    SlopeType)              = 0;
    virtual std::string SetCoupling(OScopeChannels::Channel chan, Int2Type<AC1M>)    = 0;
    virtual std::string SetCoupling(OScopeChannels::Channel chan, Int2Type<DC1M>)    = 0;
    virtual std::string SetCoupling(OScopeChannels::Channel chan, Int2Type<GND>)     = 0;

    virtual std::string SetMemory(Int2Type<MIN>)    = 0;
    virtual std::string SetMemory(Int2Type<SMALL>)  = 0;
    virtual std::string SetMemory(Int2Type<MEDIUM>) = 0;
    virtual std::string SetMemory(Int2Type<BIG>)    = 0;
    virtual std::string SetMemory(Int2Type<MAX>)    = 0;

    virtual std::string SetTriggerCoupling(OScopeChannels::Channel, Int2Type<AC>)  = 0;
    virtual std::string SetTriggerCoupling(OScopeChannels::Channel, Int2Type<DC>)  = 0;
    virtual std::string SetTriggerCoupling(OScopeChannels::Channel, Int2Type<HFR>) = 0;
    virtual std::string SetTriggerCoupling(OScopeChannels::Channel, Int2Type<LFR>) = 0;

    virtual std::string SetTriggerMode(Int2Type<AUTO>)   = 0;
    virtual std::string SetTriggerMode(Int2Type<NORMAL>) = 0;

    virtual std::string SetTriggerSlope(OScopeChannels::Channel chan, 
                                        Int2Type<POSITIVE>) = 0; 
    virtual std::string SetTriggerSlope(OScopeChannels::Channel chan,
                                        Int2Type<NEGATIVE>) = 0;
};

#endif // SPTS_SCOPE_INTERFACE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
