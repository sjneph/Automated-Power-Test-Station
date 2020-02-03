// Macro Guard
#ifndef SPTS_TESTSTEP_DIAGNOSTIC_H
#define SPTS_TESTSTEP_DIAGNOSTIC_H

// Files included
#include "ProgramTypes.h"
#include "ScaleUnits.h"
#include "StandardFiles.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/* meant for pre-test diagnostic measurements that will be actually reported */
struct TestStepDiagnostic {
    // For printing purposes --> exception names can be arbitrarily long
    enum { MAXTESTNAMESIZE = 25 };

    TestStepDiagnostic(const std::string& name,
                       const ProgramTypes::PairMType& limits,
                       const ProgramTypes::MType& measured,
                       const std::string& units) 
                   : name_(name), units_(units), limits_(limits),
                     measured_(measured), result_(false) {        
        if ( name_.size() > MAXTESTNAMESIZE )
            name_ = name_.substr(0, MAXTESTNAMESIZE);
        setMeasuredPrecision();
    }
	ProgramTypes::PairMType Limits() const {
        return(limits_);
    }
	ProgramTypes::MType MeasuredValue() const {
        return(measured_);
    }
	bool Result() const {
        return(result_);
    }
	const std::string& TestName() const {
        return(name_);
    }
	const std::string& Units() const {
        return(units_);
    }

private:
    void setMeasuredPrecision() {
        bool result = ((measured_ >= limits_.first) && (measured_ <= limits_.second));

        // The following 'if' selections account for measurements which would be
        //  'failing' until they are rounded per the precision we have to report them to.
        //  Keep in mind that limits_.first's precision may not be the same as
        //  limit_.second's.
        ProgramTypes::MType min = limits_.first, max = limits_.second;
        if ( measured_.GetPrecision() > min.GetPrecision() )
            min.SetPrecision(measured_.GetPrecision());
        else if ( measured_.GetPrecision() > max.GetPrecision() )
            max.SetPrecision(measured_.GetPrecision());

        if ( measured_.ValueStr() == min.ValueStr() )   
            result = true;
        else if ( measured_.ValueStr() == max.ValueStr() )
            result = true;
        result_ = result;
    }

private:
    std::string name_;
    std::string units_;
    ProgramTypes::PairMType limits_;
    ProgramTypes::MType measured_;
    bool result_;
};


#endif // SPTS_TESTSTEP_DIAGNOSTIC_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
