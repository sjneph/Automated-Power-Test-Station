// Macro Guard
#ifndef SPTS_MEASUREMENT_FUNCTIONS_H
#define SPTS_MEASUREMENT_FUNCTIONS_H

// Files included
#include "Measurement.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   04/28/05, sjn,
   ==============
     Added performTest() to function object LoadTransientResponse().  See <thisfile>.cpp
       for details.

   ==============
   09/20/04, sjn,
   ==============
     Removed VoutPARD::beenDone().  The station has been re-designed and will no longer
       be capable of performing parallel oscilloscope measurements.
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


//========================
// Start Case Temperature
//========================
struct CaseTemperature : public Measurement {
    // Constructor and Destructor
    CaseTemperature() : Measurement() { /* */ }
    virtual ~CaseTemperature() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Case Temperature"); }
};
//=======================
// Stop Case Temperature
//=======================


//========================
// Start Cross Regulation
//========================
struct CrossRegulation : public Measurement {
    // Constructor and Destructor
    CrossRegulation() : Measurement() { /* */ }
    virtual ~CrossRegulation() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    bool beenDone(ConditionsPtr current, ConditionsPtr previous) const;
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Cross Regulation"); }
};
//=======================
// Stop Cross Regulation
//=======================


//========================
// Start Cross Regulation XX
//========================
struct CrossRegulationXX : public Measurement {
    // Constructor and Destructor
    CrossRegulationXX() : Measurement() { /* */ }
    virtual ~CrossRegulationXX() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    bool beenDone(ConditionsPtr current, ConditionsPtr previous) const;
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Cross Regulation XX"); }
};
//=======================
// Stop Cross Regulation XX
//=======================


//==================
// Start Efficiency
//==================
struct Efficiency : public Measurement {
    // Constructor and Destructor
    Efficiency() : Measurement() { /* */ }
    virtual ~Efficiency() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Efficiency"); }
};
//=================
// Stop Efficiency
//=================


//=================
// Start Frequency
//=================
struct Frequency : public Measurement {
    // Constructor and Destructor
    Frequency() : Measurement() { /* */ } 
    virtual ~Frequency() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Frequency"); }
};
//================
// Stop Frequency
//================


//===============================
// Start Generic Voltage Measure
//===============================
struct GenericVoltsMeasureDC : public Measurement {
    // Constructor and Destructor
    GenericVoltsMeasureDC() : Measurement() { /* */ }
    virtual ~GenericVoltsMeasureDC() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Generic Voltage Measurement"); }
};
//==============================
// Stop Generic Voltage Measure
//==============================


//===============
// Start  Iin DC
//===============
struct IinDC : public Measurement {
    // Constructor and Destructor
    IinDC() : Measurement() { /* */ }
    virtual ~IinDC() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Iin DC"); }
};
//=============
// Stop Iin DC
//=============


//================
// Start Iin PARD
//================
struct IinPARD : public Measurement {
    // Constructor and Destructor
    IinPARD() : Measurement() { /* */ }
    virtual ~IinPARD() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Iin PARD"); }
};
//===============
// Stop Iin PARD
//===============


//================
// Start  Inhibit Cycle Test
//================
struct InhibitCycleTest : public Measurement {
    // Constructor and Destructor
    InhibitCycleTest() : Measurement() { /* */ }
    virtual ~InhibitCycleTest() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Inhibit Cycle Test"); }
};
//==============
// Stop Inhibit Cycle Test
//==============


//================
// Start  Iout DC
//================
struct IoutDC : public Measurement {
    // Constructor and Destructor
    IoutDC() : Measurement() { /* */ }
    virtual ~IoutDC() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    bool beenDone(ConditionsPtr current, ConditionsPtr previous) const;
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Iout DC"); }
};
//==============
// Stop Iout DC
//==============


//=======================
// Start Iout Trip Point
//=======================
struct IoutTripPoint : public Measurement {
    // Constructor and Destructor
    IoutTripPoint() : Measurement() { /* */ }
    virtual ~IoutTripPoint() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Iout Trip Point"); }
};
//=======================
// Start Iout Trip Point
//=======================


//=========================
// Start Iout Trip Pretrim
//=========================
struct IoutTripPretrim : public IoutTripPoint {
    // Constructor and Destructor
    IoutTripPretrim() : IoutTripPoint() { /* */ }
    virtual ~IoutTripPretrim() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Iout Trip Pretrim"); }
};
//========================
// Stop Iout Trip Pretrim
//========================


//=======================
// Start Line Regulation
//=======================
struct LineRegulation : public Measurement {
    // Constructor and Destructor
    LineRegulation() : Measurement() { /* */ }
    virtual ~LineRegulation() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    bool beenDone(ConditionsPtr current, ConditionsPtr previous) const;    
    std::string name() const { return(Name()); }
    bool dealWithRampVinInBase() const { return(false); }

    // Static Funcs
    static std::string Name() { return("Line Regulation"); }
};
//======================
// Stop Line Regulation
//======================


//=======================
// Start Load Regulation
//=======================
struct LoadRegulation : public Measurement {
    // Constructor and Destructor
    LoadRegulation() : Measurement() { /* */ }
    virtual ~LoadRegulation() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    bool beenDone(ConditionsPtr current, ConditionsPtr previous) const;
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Load Regulation"); }
};
//======================
// Stop Load Regulation
//======================


//===============================
// Start Load Transient Recovery
//===============================
struct LoadTransientRecovery : public Measurement {
    // Constructor and Destructor
    LoadTransientRecovery() : Measurement() { /* */ }
    virtual ~LoadTransientRecovery() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits); 
    bool beenDone(ConditionsPtr current, ConditionsPtr previous) const;
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Load Transient Recovery"); }
};
//==============================
// Stop Load Transient Recovery
//==============================


//===============================
// Start Load Transient Response
//===============================
struct LoadTransientResponse : public Measurement {
    // Constructor and Destructor
    LoadTransientResponse() : Measurement() { /* */ }
    virtual ~LoadTransientResponse() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Load Transient Response"); }

private:
    void performTest(ConditionsPtr conditions, const PairMType& limits);
};
//==============================
// Stop Load Transient Response
//==============================


//========================
// Start Low Line Dropout 
//========================
struct LowLineDropout : public Measurement {
    // Constructor and Destructor
    LowLineDropout() : Measurement() { /* */ }
    virtual ~LowLineDropout() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Low Line Dropout"); }
};
//=======================
// Stop Low Line Dropout
//=======================


//=========================
// Start Power Dissipation
//=========================
struct PowerDissipation : public Measurement {
    // Constructor and Destructor
    PowerDissipation() : Measurement() { /* */ }
    virtual ~PowerDissipation() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Power Dissipation"); }
};
//========================
// Stop Power Dissipation
//========================


//========================
// Start SC Release Delay
//========================
// function object is located near end of this file
//   due to its dependency of TurnOnDelay


//============================
// Start SC Release Overshoot
//============================
struct SCReleaseOvershoot : public Measurement {
    // Constructor and Destructor
    SCReleaseOvershoot() : Measurement() { /* */ }
    virtual ~SCReleaseOvershoot() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    bool beenDone(ConditionsPtr current, ConditionsPtr previous) const;
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("SC Release Overshoot"); }
};
//============================
// Start SC Release Overshoot
//============================


//=====================
// Start Turn On Delay
//=====================
struct TurnOnDelay : public Measurement {
    // Constructor and Destructor
    TurnOnDelay() : Measurement(), type_(PRIMARYINHIBIT) { /* */ }
    virtual ~TurnOnDelay() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    bool beenDone(ConditionsPtr current, ConditionsPtr previous) const;
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Turn On Delay"); }

protected:
    enum TriggerType { VINRAMP, PRIMARYINHIBIT, SHORT };
    TurnOnDelay(TriggerType type) : Measurement(), type_(type) { /* */ }

private:
    void performTest(ConditionsPtr conditions, const PairMType& limits);

private:
    TriggerType type_;
};
//====================
// Stop Turn On Delay
//====================


//=========================
// Start Turn On Overshoot
//=========================
struct TurnOnOvershoot : public Measurement {
    // Constructor and Destructor
    TurnOnOvershoot() : Measurement() { /* */ }
    virtual ~TurnOnOvershoot() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    bool beenDone(ConditionsPtr current, ConditionsPtr previous) const;
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Turn On Overshoot"); }
};
//========================
// Stop Turn On Overshoot
//========================


//===============
// Start  Vin DC
//===============
struct VinDC : public Measurement {
    // Constructor and Destructor
    VinDC() : Measurement() { /* */ }
    virtual ~VinDC() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Vin DC"); }
};
//=============
// Stop Vin DC
//=============


//======================
// Start Vin Ramp Delay
//======================
struct VinRampDelay : public TurnOnDelay {
    // Constructor and Destructor
    VinRampDelay() : TurnOnDelay(TurnOnDelay::VINRAMP) { /* */ }
    virtual ~VinRampDelay() { /* */ }

    // TurnOnDelay::Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    bool beenDone(ConditionsPtr current, ConditionsPtr previous) const;
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Vin Ramp Delay"); }
};
//======================
// Start Vin Ramp Delay
//======================


//==========================
// Start Vin Ramp Overshoot
//==========================
struct VinRampOvershoot : public Measurement {
    // Constructor and Destructor
    VinRampOvershoot() : Measurement() { /* */ }
    virtual ~VinRampOvershoot() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    bool beenDone(ConditionsPtr current, ConditionsPtr previous) const;
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Vin Ramp Overshoot"); }
};
//==========================
// Start Vin Ramp Overshoot
//==========================


//================
// Start  Vout DC
//================
struct VoutDC : public Measurement {
    // Constructor and Destructor
    VoutDC() : Measurement() { /* */ }
    virtual ~VoutDC() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    bool beenDone(ConditionsPtr current, ConditionsPtr previous) const;
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Vout DC"); }
};
//==============
// Stop Vout DC
//==============


//=======================
// Start Vout DC Pretrim
//=======================
struct VoutDCPretrim : public Measurement {
    // Constructor and Destructor
    VoutDCPretrim() : Measurement() { /* */ }
    virtual ~VoutDCPretrim() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Vout DC Pretrim"); }
};
//======================
// Stop Vout DC Pretrim
//======================


//=================
// Start Vout PARD
//=================
struct VoutPARD : public Measurement {
    // Constructor and Destructor
    VoutPARD() : Measurement() { /* */ }
    virtual ~VoutPARD() { /* */ }

    // Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("Vout PARD"); }
};
//================
// Stop Vout PARD
//================


//========================
// Start SC Release Delay
//========================
struct SCReleaseDelay : public TurnOnDelay {
    // Constructor and Destructor
    SCReleaseDelay() : TurnOnDelay(TurnOnDelay::SHORT) { /* */ }
    virtual ~SCReleaseDelay() { /* */ }

    // TurnOnDelay::Measurement Base Virtual Function Overrides
    void operator()(ConditionsPtr conditions, const PairMType& limits);
    bool beenDone(ConditionsPtr current, ConditionsPtr previous) const;
    std::string name() const { return(Name()); }

    // Static Funcs
    static std::string Name() { return("SC Release Delay"); }
};
//========================
// Start SC Release Delay
//========================


} // namespace SPTSMeasurement

#endif // SPTS_MEASUREMENT_FUNCTIONS_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
