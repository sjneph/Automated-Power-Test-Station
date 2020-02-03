// Macro Guard
#ifndef SPTS_FUNCTIONGENERATOR_SCPI
#define SPTS_FUNCTIONGENERATOR_SCPI

// Files included
#include "FunctionGeneratorInterface.h"
#include "InstrumentTags.h"
#include "SCPI.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============
   06/23/05, sjn,
   ==============
     Modified all member functions --> changed from static to virtual.  Added public
       inheritance to FunctionGeneratorInterface.  Moving away from a static model to
       a dynamic one so multiple FGs can be swapped in/out of the station.  Added
       Identify(), BitDelimmitter(), OpsCompleteBits(), IsError(), IsDone(), ErrorBits()
       Concatenate() and SetOpsComplete().  Made all functions const.  Removed
       inheritance of SCPI<IEEE488>.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

template <>
struct SCPI<FunctionGeneratorTag> : public FunctionGeneratorInterface {

    typedef SCPI<IEEE488> BaseType;

    virtual char BitDelimmitter() const
        { return(BaseType::BITDELIMMITTER); }
    virtual std::string Concatenate() const
        { return(BaseType::Concatenate()); }
    virtual std::string ErrorBits() const
        { return(BaseType::ErrorBits()); }
    virtual std::string Identify() const
        { return(BaseType::Identify()); }
    virtual std::string Initialize() const {
          return(
                 BaseType::Reset() + 
                 Concatenate() + 
                 OutputOff() + 
                 Concatenate() + 
                 "OUTPUT:LOAD 50"
          ); 
        }

    virtual std::string IsDone() const
        { return(BaseType::IsDone()); }
    virtual std::string IsError() const
        { return(BaseType::IsError()); }
    virtual std::string OpCompleteBits() const
        { return(BaseType::OpCompleteBits()); }
    virtual std::string OutputOff() const
        { return("APPLY:DC DEF, DEF, 0"); }
    virtual NumberBase::Base RegisterReturnType() const
        { return(NumberBase::DECIMAL); }
    virtual std::string Reset() const
        { return(Initialize()); }
    virtual std::string SetAmplitude(const std::string& amplitude) const {
          return(
                 "VOLTAGE:UNIT VPP" + 
                 Concatenate() + 
                 "VOLTAGE " + 
                 amplitude
                );
        }
    virtual std::string SetDutyCycle(const std::string& dc) const
        { return("PULSE:DCYCLE " + dc); } 
    virtual std::string SetFrequency(const std::string& freq) const
        { return("FREQ " + freq); }
    virtual std::string SetOffset(const std::string& offset) const
        { return("VOLTAGE:OFFSET " + offset); }
    virtual std::string SetOpsComplete() const
        { return(BaseType::SetOpsComplete()); }
    virtual std::string SetSquareWave() const
        { return("FUNCTION:SHAPE SQUARE"); }
    virtual long TotalRegisterBits() const
        { return(8); }
    virtual std::string WhatError() const
		{ return("SYST:ERR?"); }

	virtual ~SCPI<FunctionGeneratorTag>() { /* */ }  
};

#endif // SPTS_FUNCTIONGENERATOR_SCPI

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
