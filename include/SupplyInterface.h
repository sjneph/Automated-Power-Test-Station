// Macro Guard
#ifndef SPTS_SUPPLY_INTERFACE_H
#define SPTS_SUPPLY_INTERFACE_H

// Files included
#include "NumberBase.h"
#include "ProgramTypes.h"
#include "Switch.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============  
   05/23/05, sjn,
   ==============
     Made all functions const.  Added BitDelimmitter().
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct SupplyInterface {
    virtual char BitDelimmitter() const                                    = 0;
    virtual std::string OverCurrentCheck() const                           = 0; 
    virtual std::string OverCurrentCheckBits() const                       = 0;
    virtual std::string ClearErrors() const                                = 0;
    virtual std::string Concatenate() const                                = 0;
    virtual std::string ErrorBits() const                                  = 0;
    virtual std::string Identify() const                                   = 0;
    virtual std::string Initialize() const                                 = 0;
    virtual std::string MeasureVolts() const                               = 0;
    virtual std::string OutputOff() const                                  = 0;
    virtual std::string OutputOn() const                                   = 0;
    virtual std::string IsDone() const                                     = 0;
    virtual std::string IsError() const                                    = 0;
    virtual std::string OpCompleteBits() const                             = 0;
    virtual std::string Reset() const                                      = 0;
    virtual NumberBase::Base RegisterReturnType() const                    = 0;
    virtual std::string SetAmps(const ProgramTypes::SetType& limit) const  = 0;
    virtual std::string SetCurrentProtection(Switch state) const           = 0;
    virtual std::string SetOpsComplete() const                             = 0;
    virtual std::string SetVolts(const ProgramTypes::SetType& value) const = 0;
    virtual long TotalRegisterBits() const                                 = 0;
    virtual std::string WhatError() const                                  = 0;
    virtual ~SupplyInterface() { /* */ }
};

#endif // SPTS_SUPPLY_INTERFACE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
