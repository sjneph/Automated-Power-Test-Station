// Macro Guard
#ifndef	SPTS_MEASUREMENT_BASE_H
#define SPTS_MEASUREMENT_BASE_H

// Files included
#include "Converter.h"
#include "InstrumentTypes.h"
#include "ProgramTypes.h"
#include "SingletonType.h"
#include "SPTS.h"
#include "StandardFiles.h"
#include "StandardStationFiles.h"
#include "TestStepInfo.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   05/06/05, sjn,
   ==============
   Changed member variable returnType_ from static to non-static.
   Added member variable errorCode_.
   Added IsDUTError(), onException and WhatDUTError().
   Added BadMeasurementStr.
   Change pre/postMeasurement() helpers from private to protected to give access to
     derived structures --> allow derived class to re-establish initial conditions if
     necessary.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace SPTSMeasurement {

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

class Measurement : public ProgramTypes {    
    typedef SingletonType<VariablesFile> VarFile;
public:
    // Public Typedefs
	typedef std::string RTypeFirst;
	typedef MTypeContainer RTypeSecond;
	typedef std::pair<RTypeFirst, RTypeSecond> ReturnType;
    typedef std::vector<ReturnType> ReturnTypeContainer;
    typedef const TestStepInfo::Conditions* ConditionsPtr;
    typedef SingletonType< Factory<Measurement, std::string> > MFactory;

    // static constants
    static const MType BadMeasurement;
    static const std::string BadMeasurementStr;

    // Constructor and Destructor
    Measurement();
    virtual ~Measurement();

    //========================
    // Start Public Interface
    //========================
    bool DoneAlready(ConditionsPtr current, ConditionsPtr previous);
    ReturnTypeContainer ExtraMeasurements();
    std::string GetName() const;
    bool IsDUTError() const;
    ReturnType Measure(ConditionsPtr conditions, const ProgramTypes::PairMType& limits);
    ReturnType MeasureWithoutPrePostConditions(ConditionsPtr conditions, 
                                               const ProgramTypes::PairMType& limits);
    long WhatDUTError() const;
    //======================
    // End Public Interface
    //======================

private:
    // private helpers
    void onException();
    static void initialize();

protected:
    void postMeasurement(ConditionsPtr conditions);
    void preMeasurement(ConditionsPtr conditions);

private:
    // virtuals
    virtual bool beenDone(ConditionsPtr current, ConditionsPtr previous) const;
    virtual void operator()(ConditionsPtr, const PairMType& limits) = 0;
    virtual std::string name() const = 0;
    virtual bool dealWithRampVinInBase() const;

protected:
    static SpacePowerTestStation::SPTS* spts_;
    static Converter* dut_;
    long errorCode_;
    ReturnType returnType_;
    std::auto_ptr<ReturnTypeContainer> extraMeasures_;

private:
    static bool initialized_;
};

} // namespace SPTSMeasurement

#endif // SPTS_MEASUREMENT_BASE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
