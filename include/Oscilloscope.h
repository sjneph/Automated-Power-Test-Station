// Macro Guard
#ifndef SPTS_OSILLOSCOPE_H
#define SPTS_OSILLOSCOPE_H

// Files included
#include "Instrument.h"
#include "NoCopy.h"
#include "OscilloscopeTraits.h"
#include "OScopeInterface.h"
#include "OScopeParameters.h"
#include "ProgramTypes.h"
#include "StandardFiles.h"
#include "Switch.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
  ==============
  09/29/04, sjn,
  ==============
    Removed SetGridNumber() member function --> not generic to all scope vendors.
    Removed all Measure() overloads dealing with direct delay measurements.  Replaced
      with one Measure() overload for MeasurementType::TIME2LEVEL.
    Added boolean stopped_ and member funcs: Start() and Stop().  This is to allow a
      user to keep a waveform without the worry of re-triggering.
    Added GetHorzScale() and NumberHorizontalDivisions().
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

struct Oscilloscope : protected Instrument<OscilloscopeTraits::BusType>, 
                      protected NoCopy, public OScopeParameters,
                      public OScopeMeasurements {

    // Public Typedefs
    typedef OScopeChannels::Channel Channel;
    typedef OScopeMeasurements::MeasurementType MeasurementType;

    //========================
    // Start Public Interface
    //========================
    Oscilloscope();
    ~Oscilloscope();
    void AddScopeChannel(Channel chan);
    void ChannelOff(Channel chan);
    void ChannelOn(Channel chan);
    void Concatenate(Switch state);
    ProgramTypes::SetType GetHorzScale() const;
    std::pair<ProgramTypes::SetType, ProgramTypes::SetType> GetVertRange(Channel chan);
    ProgramTypes::SetType GetVertScale(Channel chan);
    void ImmediateMode();
    bool Initialize();
    bool IsClipping();
    bool IsError();
    // First overload for non-delay measurements, second overload for TIME2LEVEL
    ProgramTypes::MType Measure(MeasurementType type, Channel chan);
    ProgramTypes::MType Measure(MeasurementType type, Channel chan,
                                ProgramTypes::SetType refLevel,
                                SlopeType slope);
    std::string Name();
    long NumberHorizontalDivisions() const;
    long NumberVerticalDivisions() const;
    bool OperationComplete();
    void RescaleUp(Channel chan);
    bool Reset();
    void SetBandwidth(Channel chan, Switch state);
    void SetCoupling(Channel chan, CouplingType type);
    void SetHorizontalMemory(MemorySetting setting);
    void SetHorizontalScale(const ProgramTypes::SetType& scale);
    void SetOffset(Channel chan, const ProgramTypes::SetType& offset);     
    void SetTriggerCoupling(Channel chan, TriggerCouplingType type);
    void SetTriggerLevel(Channel chan, const ProgramTypes::SetType& level);
    void SetTriggerMode(TriggerMode mode);
    void SetTriggerSlope(Channel chan, SlopeType type);
    void SetTriggerSource(Channel chan);
    void SetVerticalScale(Channel chan, const ProgramTypes::SetType& scale);
    void Start();
    void Stop();
    std::string WhatError();
    //======================
    // End Public Interface
    //======================

public:
    struct ScopeChannel;

private: // Private Helpers
	bool bitprocess(const std::string& errorString, 
                    Instrument<OscilloscopeTraits::BusType>::Register toCheck);
    void channelOnOff(Channel chan, Switch state);
	bool command();
    void channelOnOff(Channel chan);
	std::string query();
    bool validChannel(Channel chan);
    void waitOnScope();

private:
    friend struct ScopeChannel;
    typedef std::map<OScopeChannels::Channel, ScopeChannel> ChannelMap;    

private:
    enum Nested { SCOPECHANNEL, EXTERNALTRIGGERCHANNEL };

private:
    bool locked_;
    bool concatenate_;
    bool clipping_;
    bool stopped_;
    long address_;
    std::string syntax_;
    std::string name_;
    std::string totalSyntax_;
    TriggerMode trigMode_;
    Channel trigSource_;
    ProgramTypes::SetType horzScale_;
    bool trigModeSet_;
    std::auto_ptr<ChannelMap> channelMap_;
    std::auto_ptr<OScopeInterface> scope_;
};

} // namespace SPTSInstrument

#endif // SPTS_OSILLOSCOPE_H


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
