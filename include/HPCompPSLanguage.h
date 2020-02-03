// Macro Guard
#ifndef SPTS_HPCOMP_PSLANG_H
#define SPTS_HPCOMP_PSLANG_H


// Files included
#include "GenericAlgorithms.h"
#include "InstrumentTags.h"
#include "ProgramTypes.h"
#include "SupplyInterface.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct HPCompPSLanguage : public SupplyInterface {    
    virtual std::string ClearErrors()
        { return("CLR;FOLD CC"); }
    virtual std::string Concatenate() 
        { return(";"); }
    virtual std::string ErrorBits() 
        { return("2,3,4,5,6,7"); }
    virtual std::string Identify() 
        { return("ID?"); }
    virtual std::string Initialize() 
        { return("RST;SRQ 0;VSET 0V;ISET 0.0A;FOLD CC;" + SetCurrentProtection(ON)); }
    virtual std::string IsDone() 
        { return(""); }
    virtual std::string IsError()
        { return("STS?"); }
    virtual std::string MeasureVolts()
        { return("VOUT?"); }
    virtual std::string OpCompleteBits()
        { return(""); }
    virtual std::string OutputOff()
        { return("OUT OFF"); }
    virtual std::string OutputOn()
        { return("OUT ON"); }
    virtual std::string OverCurrentCheck() 
        { return("STS?"); }
    virtual std::string OverCurrentCheckBits() 
        { return("6"); }
    virtual NumberBase::Base RegisterReturnType()
        { return(NumberBase::DECIMAL); }
    virtual std::string Reset()
        { return(Initialize()); }
    virtual std::string SetAmps(const ProgramTypes::SetType& limit)
        { return("ISET " + limit.ValueStr() + "A"); }
    virtual std::string SetCurrentProtection(Switch state)
        { return("FOLD " + convert<std::string>(state)); }
    virtual std::string SetOpsComplete() 
        { return(""); }
    virtual std::string SetVolts(const ProgramTypes::SetType& value)
        { return("VSET " + value.ValueStr() + "V"); }
    virtual long TotalRegisterBits() 
        { return(9); }
    virtual std::string WhatError()
        { return("ERR?"); }
	virtual ~HPCompPSLanguage()
        { /* */ }
};

#endif // SPTS_HPCOMP_PSLANG_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
