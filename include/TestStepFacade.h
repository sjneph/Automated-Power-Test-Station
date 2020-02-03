// Macro Guard
#ifndef SPTS_TESTSTEP_FACADE_H
#define SPTS_TESTSTEP_FACADE_H

// Files included
#include "Measurement.h"
#include "ProgramTypes.h"
#include "ScaleUnits.h"
#include "StandardFiles.h"
#include "TestStepDiagnostic.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   11/20/05, sjn,
   ==============
       Renamed class TestStepFacade to TestStepDiagnosticFacadeFailure.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/* meant for nonrecoverable pre-test diagnostic failures */
struct TestStepDiagnosticFacadeFailure {
    // For printing purposes --> exception names can be arbitrarily long
    enum { MAXTESTNAMESIZE = TestStepDiagnostic::MAXTESTNAMESIZE };

    TestStepDiagnosticFacadeFailure(const std::string& name, int err)
                                                       : name_(name), err_(err) {
        typedef ScaleUnits<ProgramTypes::MType> SU;
        units_ = SU::GetUnits(SU::V);
        // Get rid of any newlines or nonessential PREPEND information
        static const char newline = '\n';        
        static const std::string prepend = DUTExceptionTypes::BaseException::Prepend();
        name_ = name_.substr(0, name_.find(newline));
        if ( name_.find(prepend) != std::string::npos )
            name_ = name_.substr(name_.find(prepend) + prepend.size());
        if ( name_.size() > MAXTESTNAMESIZE )
            name_ = name_.substr(0, MAXTESTNAMESIZE);
    }
    long ErrorNumber() const {
        return(err_);
    }
	ProgramTypes::PairMType Limits() const {
        ProgramTypes::PairMType pm = std::make_pair(0, 1);
        pm.first.SetPrecision(0); pm.second.SetPrecision(0);
        return(pm);
    }
	ProgramTypes::MType MeasuredValue() const {
        ProgramTypes::MType toRtn;
        toRtn.SetExplicit(SPTSMeasurement::Measurement::BadMeasurementStr);
        return(toRtn);
    }
	bool Result() const {
        return(false);
    }
	const std::string& TestName() const {
        return(name_);
    }
	const std::string& Units() const {
        return(units_);
    }

private:
    std::string name_;
    std::string units_;
    long err_;
};


#endif // SPTS_TESTSTEP_FACADE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
