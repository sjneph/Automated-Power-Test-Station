// Macro Guard
#ifndef SPTS_TESTSTEPINFO_H
#define SPTS_TESTSTEPINFO_H


// Files included
#include "ControlMatrixTraits.h"
#include "ConverterOutput.h"
#include "FilterSelects.h"
#include "NoCopy.h"
#include "ProgramTypes.h"
#include "StandardFiles.h"
#include "SwitchMatrixTraits.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============  
   05/05/05, sjn,
   ==============
     Added private setErrorCode() to TestStepInfo structure.  Added enumeration value
       NODUTERROR and ErrorCode() to TestStepInfo::TestStep's definition.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

// Forward declarations
struct LimitsFile;
struct TestSequence;

struct TestStepInfo {

    // Static constants
    static const ProgramTypes::SetType UNDEFINEDSETTYPE;
    static const ProgramTypes::MType   UNDEFINEDMTYPE;

	// Typedefs
	typedef ProgramTypes::SetTypeContainer SetTypeContainer;
	typedef ProgramTypes::PairMType PairMType;
    enum ScaleDirection { UP, DOWN };

	// Constructor, Value Semantics and Destructor Declarations
	explicit TestStepInfo(LimitsFile* lf);
	TestStepInfo(const TestStepInfo& source);
	TestStepInfo& operator=(const TestStepInfo& source);
	virtual ~TestStepInfo();

    // Operator overloads
    friend bool operator==(const TestStepInfo& first, const TestStepInfo& second);
    friend bool operator!=(const TestStepInfo& first, const TestStepInfo& second);
    friend bool operator>(const TestStepInfo& first, const TestStepInfo& second);
    friend bool operator>=(const TestStepInfo& first, const TestStepInfo& second);
    friend bool operator<(const TestStepInfo& first, const TestStepInfo& second);
    friend bool operator<=(const TestStepInfo& first, const TestStepInfo& second);

	// Forward Declarations
	struct Conditions;
	struct TestStep;

	// More typedefs
	typedef const Conditions* CondPtr;
	typedef const TestStep* TSPtr;

	// Conversion operators
	operator CondPtr() const; 
	operator TSPtr() const;   

private:
	// Friends
	friend struct Conditions;
	friend struct TestStep;
    friend struct TestSequence;

private:
    void setErrorCode(long code);
    void setMeasuredValue(const ProgramTypes::MType& value);
    void setMeasuredValueExplicit(const std::string& value);
    void setName(const std::string& name);
    void setTestName(const std::string& name);
    void setResult(bool result);

private:
    // Forward Declaration 
	struct TestStepInfoImpl; 

private:
    // unimplemented
    TestStepInfo();

private:
	std::auto_ptr<TestStepInfoImpl> tsiImpl_;
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct TestStepInfo::Conditions : private NoCopy {

    //========================
	// Start Public Interface
    //========================
    ProgramTypes::SetType APSPrimary() const;
    ProgramTypes::SetType APSSecondary() const;
	FilterSelects::FilterType BW() const;
	ConverterOutput::Output Channel() const;
	ProgramTypes::SetType Freq() const;
	const SetTypeContainer& Iouts() const;
	const SetTypeContainer& IoutsNext() const;
    bool IsInhibited() const;
    const std::set<ControlMatrixTraits::RelayTypes::MiscRelay>& MidtestMisc() const;
	std::vector<SwitchMatrixTraits::RelayTypes::DCRelay> MiscDMM() const;
    ProgramTypes::MType MiscOhms() const;
    const std::set<ControlMatrixTraits::RelayTypes::MiscRelay>& PretestMisc() const;
    bool PrimaryInhibited() const;
	ProgramTypes::SetType RefVal() const;
    bool SecondaryInhibited() const;
	const std::vector<ConverterOutput::Output>& Shorted() const;
    bool Speedup() const;
	bool SyncIn() const;
	ProgramTypes::SetType Vin() const;
	ProgramTypes::SetType VinNext() const;
    bool VinRamp() const;

    friend bool operator==(const Conditions& first, const Conditions& second);
    //======================
    // End Public Interface
    //======================

private:
	// Implemented
	explicit Conditions(const TestStepInfoImpl* tsi);
	~Conditions();

private:
	// Non-implemented
	Conditions();

private:
    friend struct TestStepInfo::TestStepInfoImpl;
	const TestStepInfo::TestStepInfoImpl* tsi_; 
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct TestStepInfo::TestStep : private NoCopy {

    enum { NODUTERROR = 0 };

    //========================
	// Start Public Interface
    //========================
    long ErrorCode() const;
	PairMType Limits() const;
	ProgramTypes::MType MeasuredValue() const;
	bool Result() const;
    PairMType ScaledLimits(ScaleDirection dir = UP) const;
	std::string SoftwareTestName() const;
	const std::string& TestName() const;
	const std::string& Units() const;    
    //======================
    // End Public Interface
    //======================

private:
	// Implemented
	explicit TestStep(const TestStepInfoImpl* tsi);  
	~TestStep();

private:
	// Non-implemented
	TestStep();

private:
    friend struct TestStepInfo::TestStepInfoImpl;
	const TestStepInfo::TestStepInfoImpl* tsi_; 
};

#endif // SPTS_TESTSTEPINFO_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
