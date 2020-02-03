// Macro Guard
#ifndef SPTS_DMM_H
#define SPTS_DMM_H

// Files Included
#include "DMMTraits.h"
#include "Instrument.h"
#include "ProgramTypes.h"
#include "NoCopy.h"
#include "StandardFiles.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

struct DMM : private Instrument<DMMTraits::ModelType::BusType>, private NoCopy {
    // Static Constants
    static const ProgramTypes::SetType AUTO;

    // Public enum
    enum Mode { OHMS, DCV, TEMP };

    //========================
    // Constructor/Destructor
    //========================
	DMM();
	~DMM();

    //========================
    // Start Public Interface
    //========================
	bool Initialize();
	bool IsError();
    ProgramTypes::MType MeasureOhms();
    ProgramTypes::MType MeasureDCVolts();
    ProgramTypes::MType MeasureTemperature();
    std::string Name();
    bool OpsComplete();
	bool Reset();
    void SetMode(Mode mode);
    void SetRange(const ProgramTypes::SetType& range = AUTO);    
    std::string WhatError();
    //======================
    // End Public Interface
    //======================

private:    
    bool bitprocess(const std::string& errorString, 
                    Instrument<DMMTraits::ModelType::BusType>::Register toCheck);
	bool command(const std::string& cmd);
    ProgramTypes::MType measure(Mode nextMode);
	std::string query(const std::string& str);
    void setModeRange();

private:
    typedef DMMTraits::ModelType Model;    
    typedef Model::Language Language;
    typedef DMMTraits::ModelType::BusType BusType;

private:
	long address_;
    bool locked_;
    Mode configuration_;
    ProgramTypes::SetType rangeDCV_;
    ProgramTypes::SetType rangeOhm_;
    ProgramTypes::SetType rangeoC_;
    bool needReset_;
    std::string name_;
};

} // namespace SPTSInstrument

#endif // SPTS_DMM_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
