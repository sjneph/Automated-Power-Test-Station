// Macro Guard
#ifndef SPTS_FUNCTIONGENERATOR_INTERFACE_H
#define SPTS_FUNCTIONGENERATOR_INTERFACE_H

// Files included
#include "NumberBase.h"
#include "ProgramTypes.h"

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//============================
// FunctionGeneratorInterface
//============================
struct FunctionGeneratorInterface {
    virtual char BitDelimmitter() const = 0;
    virtual std::string Concatenate() const = 0;
    virtual std::string ErrorBits() const = 0;
    virtual std::string Identify() const = 0;
    virtual std::string Initialize() const = 0;
    virtual std::string IsDone() const = 0;
    virtual std::string IsError() const = 0;
    virtual std::string OutputOff() const = 0;
    virtual std::string OpCompleteBits() const = 0;
    virtual NumberBase::Base RegisterReturnType() const = 0;
    virtual std::string Reset() const = 0;
    virtual std::string SetAmplitude(const std::string& amplitude) const = 0;
    virtual std::string SetDutyCycle(const std::string& dc) const = 0;
    virtual std::string SetFrequency(const std::string& freq) const = 0;
    virtual std::string SetOffset(const std::string& offset) const = 0;
    virtual std::string SetOpsComplete() const = 0;
    virtual std::string SetSquareWave() const = 0;
    virtual long TotalRegisterBits() const = 0;
    virtual std::string WhatError() const = 0;

	virtual ~FunctionGeneratorInterface() { /* */ }
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/

#endif // SPTS_FUNCTIONGENERATOR_INTERFACE_H

