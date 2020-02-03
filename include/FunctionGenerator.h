// Macro Guard
#ifndef SPTS_FUNCTIONGENERATOR_H
#define SPTS_FUNCTIONGENERATOR_H

// Files included
#include "FunctionGeneratorInterface.h"
#include "FunctionGeneratorTraits.h"
#include "Instrument.h"
#include "NoCopy.h"
#include "ProgramTypes.h"
#include "StandardFiles.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============
   06/23/05, sjn,
   ==============
     Added member variable fg_ to support dynamic function generator model.  Added
       #include "FunctionGeneratorInterface.h".  Removed typedef named ModelType.
       Removed typedef named Language.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSInstrument {

struct FunctionGenerator 
   : private Instrument<FunctionGeneratorTraits::BusType>, private NoCopy {
     ~FunctionGenerator();

    //========================
    // Start Public Interface
    //========================	
    FunctionGenerator();
	bool Initialize();
	bool IsError();
    std::string Name() const;
    bool OpsComplete();
    void OutputOff();
	bool Reset();
    void SetAmplitude(const ProgramTypes::SetType& value);
    void SetDutyCycle(const ProgramTypes::PercentType& percent);
    void SetFrequency(const ProgramTypes::SetType& value);
    void SetOffset(const ProgramTypes::SetType& value);
    std::string WhatError();
    //======================
    // End Public Interface
    //======================

private:
    typedef ProgramTypes::SetType SetType;
    typedef ProgramTypes::PercentType PercentType;

private:
    bool bitprocess(const std::string& errorString, 
                    Instrument<FunctionGeneratorTraits::BusType>::Register toCheck);
    bool command(const std::string& cmd);
    std::string query(const std::string& q);

private:
    SetType ampl_;
    PercentType dc_;
    SetType freq_;
    SetType offset_;
    long address_;
    bool isOff_;
    bool locked_;
    std::auto_ptr<FunctionGeneratorInterface> fg_;
};

}

#endif // SPTS_FUNCTIONGENERATOR_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
