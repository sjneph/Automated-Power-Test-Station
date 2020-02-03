// Macro Guard
#ifndef SPTS_POWERSUPPLY_SCPI_H
#define SPTS_POWERSUPPLY_SCPI_H


// Files included
#include "GenericAlgorithms.h"
#include "InstrumentTags.h"
#include "ProgramTypes.h"
#include "SCPI.h"
#include "SupplyInterface.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============  
   05/23/05, sjn,
   ==============
     Made all functions const.  Added BitDelimmitter().

   ==============  
   12/10/04, sjn,
   ==============
     Modified Initialize() to set current to 10mA from 0A.  Some supplies go into
       overload with any arbitrarily small output voltage (like 3mV) without a small
       amount of current.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

template<>
struct SCPI<SingleSupplyTag> : private SCPI<IEEE488>, public SupplyInterface {
    typedef SCPI<IEEE488> BaseType;

    virtual char BitDelimmitter() const
        { return(BaseType::BITDELIMMITTER); }
    virtual std::string ClearErrors() const
        { return(BaseType::ClearErrors() + Concatenate() + "OUTP:PROT:CLE"); }
    virtual std::string Concatenate() const
        { return(BaseType::Concatenate()); }
    virtual std::string ErrorBits() const
        { return(BaseType::ErrorBits()); }
    virtual std::string Identify() const
        { return(BaseType::Identify()); }
    virtual std::string Initialize() const
        { 
          return(
                  "*RST;SYST:LANG TMSL" +
                   Concatenate()        +
                  "VOLT:LEVEL 0V"       +
                   Concatenate()        +
                  "CURR:LEVEL 0.01A"    +
                   Concatenate()        +
                   SetCurrentProtection(ON)
          ); 
        }
    virtual std::string IsDone() const
        { return(BaseType::IsDone()); }
    virtual std::string IsError() const
        { return(BaseType::IsError()); }
    virtual std::string MeasureVolts() const
        { return("MEAS:VOLT?"); }
    virtual std::string OpCompleteBits() const
        { return(BaseType::OpCompleteBits()); }
    virtual std::string OutputOff() const
        { return("OUTP:STAT OFF"); }
    virtual std::string OutputOn() const
        { return("OUTP:STAT ON"); }
    virtual std::string OverCurrentCheck() const 
        { return("STAT:QUES:EVEN?"); }
    virtual std::string OverCurrentCheckBits() const
        { return("1"); }
    virtual NumberBase::Base RegisterReturnType() const
        { return(NumberBase::DECIMAL); }
    virtual std::string Reset() const
        { return(Initialize()); }
    virtual std::string SetAmps(const ProgramTypes::SetType& limit) const
        { return("CURR:LEVEL " + convert<std::string>(limit) +  "A"); }
    virtual std::string SetCurrentProtection(Switch state) const
        { 
            std::string toRtn = "CURR:PROT:STAT ";
            if ( state == ON )
                toRtn += "1";
            else
                toRtn += "0";
            return(toRtn); 
        }
    virtual std::string SetOpsComplete() const
        { return(BaseType::SetOpsComplete()); }
    virtual std::string SetVolts(const ProgramTypes::SetType& value) const
        { return("VOLT:LEVEL " + convert<std::string>(value) + "V"); }
    virtual long TotalRegisterBits() const
        { return(8); }
    virtual std::string WhatError() const
        { return("SYST:ERR?"); }
	virtual ~SCPI<SingleSupplyTag>() 
        { /* */ }  
};

#endif // SPTS_POWERSUPPLY_SCPI_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
