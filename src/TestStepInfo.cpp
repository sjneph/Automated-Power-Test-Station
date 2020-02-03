// Files included
#include "Assertion.h"
#include "ConfigureRelays.h"
#include "Converter.h"
#include "GenericAlgorithms.h"
#include "ProgramTypes.h"
#include "ScaleUnits.h"
#include "SingletonType.h"
#include "SPTSException.h"
#include "StandardStationFiles.h"
#include "TestStepInfo.h"



//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   01/13/10, REB,
   ==============
     Added additional values to convertSymbolicLoad()

   ==============
   11/12/08, REB,
   ==============
     Added to convertSymbolicLoad()for percentages

   ==============
   06/23/05, sjn,
   ==============
     Added convertSymbolicLoad() and convertSymbolicLine() helpers to unnamed namespace
     --> allow common symbolic load/line values to be read in from external file.  This
     helps implement a more robust input data entry system.
     Modified setMembers() to use these new helpers.
     Modified apsPrimary_ and apsSecondary_ to be assigned to UNDEFINEDSETTYPE instead
      of 0V when they are unused.  This is to reflect changes that allow us to specify
      a per-test setting of these parameters even when there exists a sequence-wide
      setpoint.  0V takes on meaning now.

   ==============
   05/05/05, sjn,
   ==============
     Added implementation for TestStepInfo::setErrorCode().  Added eCode_ to pimpl
       implemenation.  Added implementation for TestStepInfo::TestStep::ErrorCode().

   ==============  
   07/28/04, sjn,
   ==============
     Removed any leading/trailing spaces from all test names in setMembers() overload.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    typedef StationExceptionTypes::BadArg    BadArg;
    typedef StationExceptionTypes::FileError FileError;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { // unnamed

    // Typedefs
    typedef ProgramTypes::MType   MType;
    typedef ProgramTypes::SetType SetType;

    //=======================
    // convertSymbolicLoad()
    //=======================
    ProgramTypes::SetType convertSymbolicLoad(ConverterOutput::Output channel,
                                              const std::string& loadValue) {
        Converter* converter = SingletonType<Converter>::Instance();        
        std::string cpy = Uppercase(loadValue);
        ProgramTypes::SetType fullLoad = converter->Iout(channel);
        if ( cpy == "FULL LOAD" )
            return(fullLoad);
        else if ( cpy == "HALF LOAD" )
            return(fullLoad / ProgramTypes::SetType(2));
        else if ( cpy == "NO LOAD" )
            return(0);
        else if ( cpy == "TEN% LOAD" )
            return(fullLoad / ProgramTypes::SetType(10));
        else if ( cpy == "0% LOAD" )
            return(0);
        else if ( cpy == "2% LOAD" )
            return(fullLoad * ProgramTypes::SetType(0.02));
        else if ( cpy == "5% LOAD" )
            return(fullLoad * ProgramTypes::SetType(0.05));
        else if ( cpy == "10% LOAD" )
            return(fullLoad * ProgramTypes::SetType(0.10));
        else if ( cpy == "20% LOAD" )
            return(fullLoad * ProgramTypes::SetType(0.20));
        else if ( cpy == "25% LOAD" )
            return(fullLoad * ProgramTypes::SetType(0.25));
        else if ( cpy == "30% LOAD" )
            return(fullLoad * ProgramTypes::SetType(0.30));
        else if ( cpy == "40% LOAD" )
            return(fullLoad * ProgramTypes::SetType(0.40));
        else if ( cpy == "50% LOAD" )
            return(fullLoad * ProgramTypes::SetType(0.50));
        else if ( cpy == "60% LOAD" )
            return(fullLoad * ProgramTypes::SetType(0.60));
        else if ( cpy == "70% LOAD" )
            return(fullLoad * ProgramTypes::SetType(0.70));
        else if ( cpy == "75% LOAD" )
            return(fullLoad * ProgramTypes::SetType(0.75));
        else if ( cpy == "80% LOAD" )
            return(fullLoad * ProgramTypes::SetType(0.80));
        else if ( cpy == "90% LOAD" )
            return(fullLoad * ProgramTypes::SetType(0.90));
        else if ( cpy == "100% LOAD" )
            return(fullLoad);
        else if ( cpy == "110% LOAD" )
            return(fullLoad * ProgramTypes::SetType(1.10));
        else if ( cpy == "120% LOAD" )
            return(fullLoad * ProgramTypes::SetType(1.20));
        else if ( cpy == "125% LOAD" )
            return(fullLoad * ProgramTypes::SetType(1.25));
        else if ( cpy == "130% LOAD" )
            return(fullLoad * ProgramTypes::SetType(1.30));
        else if ( cpy == "150% LOAD" )
            return(fullLoad * ProgramTypes::SetType(1.50));
        
        std::pair<SetType, ScaleUnits<SetType>::Units> p;
        p = ScaleUnits<SetType>::GetUnits(loadValue);
        return(ScaleUnits<SetType>::ScaleUp(p.first, p.second));
    }

    //=======================
    // convertSymbolicLine()
    //=======================
    ProgramTypes::SetType convertSymbolicLine(const std::string& vin) {
        Converter* converter = SingletonType<Converter>::Instance();        
        std::string cpy = Uppercase(vin);
        if ( cpy == "LOW LINE" )
            return(converter->LowLine());
        else if ( cpy == "NOMINAL LINE" )
            return(converter->NominalLine());
        else if ( cpy == "HIGH LINE" )
            return(converter->HighLine());

        std::pair<SetType, ScaleUnits<SetType>::Units> p;
        p = ScaleUnits<SetType>::GetUnits(vin);
        return(ScaleUnits<SetType>::ScaleUp(p.first, p.second));
    }

    // name()
    std::string name() {
        static std::string name = "TestStepInfo or TestStepInfo::NestedType class";
        return(name);
    }

    // isBoolean<ErrorType> class
    template <typename ErrorType>
    bool isBoolean(const std::string& s) {
        Assert<ErrorType>(! s.empty(), name());
        if ( ("1" == s) || ("TRUE" == Uppercase(s)) )
            return(true);
        if ( ("0" == s) || ("FALSE" == Uppercase(s)) )
            return(false);
        throw(ErrorType(name()));
    }
} // unnamed namespace

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

const ProgramTypes::SetType TestStepInfo::UNDEFINEDSETTYPE = -1;
const ProgramTypes::MType   TestStepInfo::UNDEFINEDMTYPE   = -1;

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//--------------------------------> TestStepInfoImpl <---------------------------------//
//=====================================================================================//


struct TestStepInfo::TestStepInfoImpl {
    //----------------------------------------------------
	// Private Nested Class --> only TestStepInfo's and
    //  and friends have access to this --> PIMPL pattern                                      
    //----------------------------------------------------
	friend struct TestStepInfo;
    TestStepInfoImpl(); // unimplemented
	TestStepInfoImpl(LimitsFile* lf);
	TestStepInfoImpl(const TestStepInfoImpl& source);
	TestStepInfoImpl& operator=(const TestStepInfoImpl& source);
	~TestStepInfoImpl();  // Only a TestStepInfo object is allowed to destroy me
    void setMembers(const TestStepInfoImpl& src); 
    void setMembers(LimitsFile* lf);

	// Member Variables
	SetType acqCount_;
    SetType apsPrimary_;
    SetType apsSecondary_;
	FilterSelects::FilterType bandwidth_;
	TestStepInfo::Conditions* conditions_;
	ConverterOutput::Output currentChannel_;
    long eCode_;
	SetType freq_;
	SetTypeContainer iouts_;
	MType maxLimit_;
	MType measured_;
    std::set<ControlMatrixTraits::RelayTypes::MiscRelay> midMisc_;
	MType minLimit_;
    std::vector<SwitchMatrixTraits::RelayTypes::DCRelay> miscDMM_;
    MType miscOhms_;
	SetTypeContainer nextIouts_;
	SetType nextVin_;
    std::set<ControlMatrixTraits::RelayTypes::MiscRelay> pretestMisc_;
	bool priInhibited_;
    SetType refValue_;
	bool result_;
    bool secInhibited_;
	std::vector<ConverterOutput::Output> shortedChannels_;
    bool speedup_;
	std::string swTestname_;
	std::string testname_;
	TestStepInfo::TestStep* testStep_;
	std::string units_;
	SetType vin_;
    bool vRamp_;
};

//=============
// Constructor
//=============
TestStepInfo::TestStepInfoImpl::TestStepInfoImpl(LimitsFile* lf) {
    conditions_ = new TestStepInfo::Conditions(this);
    testStep_   = new TestStepInfo::TestStep(this);
    setMembers(lf);
}

//============
// Destructor 
//============
TestStepInfo::TestStepInfoImpl::~TestStepInfoImpl() { 
    if ( conditions_ ) delete(conditions_);
    if ( testStep_ )   delete(testStep_);
    conditions_ = 0;
    testStep_   = 0;
}

//==================
// Copy constructor
//==================
TestStepInfo::TestStepInfoImpl::TestStepInfoImpl(const TestStepInfoImpl& source) {
    conditions_ = new TestStepInfo::Conditions(this); 
    testStep_   = new TestStepInfo::TestStep(this); 

    setMembers(source);
}

//=====================
// Assignment operator
//=====================
TestStepInfo::TestStepInfoImpl& 
  TestStepInfo::TestStepInfoImpl::operator=(
                                    const TestStepInfo::TestStepInfoImpl& source) {
	if ( this != &source ) {
        if ( conditions_ ) delete(conditions_);
        if ( testStep_ )   delete(testStep_);
		conditions_ = new TestStepInfo::Conditions(this);
		testStep_   = new TestStepInfo::TestStep(this);
        setMembers(source);
	}
	return(*this);
}

//========================
// setMembers() Overload1
//========================
void TestStepInfo::TestStepInfoImpl::setMembers(const TestStepInfoImpl& src) {
    // Everything except testStep_ and conditions_
    acqCount_        = src.acqCount_;
	apsPrimary_      = src.apsPrimary_;
    apsSecondary_    = src.apsSecondary_;
	bandwidth_       = src.bandwidth_;	
	currentChannel_  = src.currentChannel_;
    eCode_           = src.eCode_;
	freq_            = src.freq_;
	iouts_           = src.iouts_;
	maxLimit_        = src.maxLimit_;
	measured_        = src.measured_;
    midMisc_         = src.midMisc_;
	minLimit_        = src.minLimit_;
    miscDMM_         = src.miscDMM_;
    miscOhms_        = src.miscOhms_;
	nextIouts_       = src.nextIouts_;
	nextVin_         = src.nextVin_;
    pretestMisc_     = src.pretestMisc_;
	priInhibited_    = src.priInhibited_;    
    refValue_        = src.refValue_;
	result_          = src.result_;
    secInhibited_    = src.secInhibited_;
	shortedChannels_ = src.shortedChannels_;
    speedup_         = src.speedup_;
	swTestname_      = src.swTestname_;
	testname_        = src.testname_;
	units_           = src.units_;
	vin_             = src.vin_;
    vRamp_           = src.vRamp_;
}

//========================
// setMembers() Overload2
//========================
void TestStepInfo::TestStepInfoImpl::setMembers(LimitsFile* lf) {
    Converter* dut = SingletonType<Converter>::Instance();

    // Grab information in string format
    std::string pName   = lf->GetTestStepParameter(LimitsFile::PRINTEDTESTNAME);
    std::string vin     = lf->GetTestStepParameter(LimitsFile::VIN);
    std::string iout1   = lf->GetTestStepParameter(LimitsFile::IOUT1);
    std::string iout2   = lf->GetTestStepParameter(LimitsFile::IOUT2);
    std::string iout3   = lf->GetTestStepParameter(LimitsFile::IOUT3);
    std::string iout4   = lf->GetTestStepParameter(LimitsFile::IOUT4);
    std::string iout5   = lf->GetTestStepParameter(LimitsFile::IOUT5);
    std::string min     = lf->GetTestStepParameter(LimitsFile::MINLIMIT);
    std::string max     = lf->GetTestStepParameter(LimitsFile::MAXLIMIT);
    std::string units   = lf->GetTestStepParameter(LimitsFile::UNITS);
    std::string nextVin = lf->GetTestStepParameter(LimitsFile::NEXTVIN);
    std::string niout1  = lf->GetTestStepParameter(LimitsFile::NEXTIOUT1);
    std::string niout2  = lf->GetTestStepParameter(LimitsFile::NEXTIOUT2);
    std::string niout3  = lf->GetTestStepParameter(LimitsFile::NEXTIOUT3);
    std::string niout4  = lf->GetTestStepParameter(LimitsFile::NEXTIOUT4);
    std::string niout5  = lf->GetTestStepParameter(LimitsFile::NEXTIOUT5);
    std::string ref     = lf->GetTestStepParameter(LimitsFile::REFVALUE);
    std::string bw      = lf->GetTestStepParameter(LimitsFile::BANDWIDTH);
    std::string isPInh  = lf->GetTestStepParameter(LimitsFile::ISINHIBITEDPRIMARY);
    std::string isSInh  = lf->GetTestStepParameter(LimitsFile::ISINHIBITEDSECONDARY); 
    std::string syncIn  = lf->GetTestStepParameter(LimitsFile::SYNCINVALUE);
    std::string isSync  = lf->GetTestStepParameter(LimitsFile::ISSYNCHRONIZED);
    std::string short1  = lf->GetTestStepParameter(LimitsFile::SHORTCHANNEL1);
    std::string short2  = lf->GetTestStepParameter(LimitsFile::SHORTCHANNEL2);
    std::string short3  = lf->GetTestStepParameter(LimitsFile::SHORTCHANNEL3);
    std::string short4  = lf->GetTestStepParameter(LimitsFile::SHORTCHANNEL4);
    std::string short5  = lf->GetTestStepParameter(LimitsFile::SHORTCHANNEL5);
    std::string miscDMM = lf->GetTestStepParameter(LimitsFile::MISCDMM);
    std::string miscOhm = lf->GetTestStepParameter(LimitsFile::MISCOHM);
    std::string apsPri  = lf->GetTestStepParameter(LimitsFile::APSPRIMARY);
    std::string apsSec  = lf->GetTestStepParameter(LimitsFile::APSSECONDARY);
    std::string oscAvg  = lf->GetTestStepParameter(LimitsFile::ACQCOUNT);
    std::string speedUp = lf->GetTestStepParameter(LimitsFile::SPEEDUP);
    std::string sName   = lf->GetTestStepParameter(LimitsFile::SOFTWARETESTNAME);
    sName = Uppercase(sName);

    std::pair<SetType, ScaleUnits<SetType>::Units> p;
    std::pair<MType, ScaleUnits<MType>::Units> q;
    std::vector<std::string> vtemp, split;
    std::vector<std::string>::iterator i, j;

    eCode_ = TestStepInfo::TestStep::NODUTERROR;

    // Convert string information to something useful
    try {

		// oscAvg
		if ( !IsInteger(oscAvg) ) {
			acqCount_ = UNDEFINEDSETTYPE;
		}
		else
			acqCount_ = SetType(oscAvg);

        // apsPri
        if ( !apsPri.empty() ) {
            p = ScaleUnits<SetType>::GetUnits(apsPri);
            apsPrimary_ = ScaleUnits<SetType>::ScaleUp(p.first, p.second);
        }
        else
            apsPrimary_ = UNDEFINEDSETTYPE;        
        
        // apsSec
        if ( !apsSec.empty() ) {
            p = ScaleUnits<SetType>::GetUnits(apsSec);
            apsSecondary_ = ScaleUnits<SetType>::ScaleUp(p.first, p.second);
        }
        else
            apsSecondary_ = UNDEFINEDSETTYPE;

        // bandwidth_
        bandwidth_ = bw.empty() ? FilterSelects::PASSTHRU : ConvertToFilterType(bw);

        // currentChannel_ and swTestName_
        if ( ! sName.empty() ) {
            std::string toConvert;
            toConvert += sName[sName.size()-1];
            if ( toConvert.find_first_of("12345") != std::string::npos ) {
                long val = convert<long>(toConvert);
                currentChannel_ = static_cast<ConverterOutput::Output>(val);
                swTestname_ = sName.substr(0, sName.size()-1);
            }
            else {
                currentChannel_ = ConverterOutput::ALL; 
                swTestname_ = sName;
            }
        }
        else {            
            Assert<FileError>(!swTestname_.empty(), name());
            currentChannel_ = ConverterOutput::ALL;
        }

        // freq_
        if ( isBoolean<FileError>(isSync) ) {
            p = ScaleUnits<SetType>::GetUnits(syncIn);
            freq_ = ScaleUnits<SetType>::ScaleUp(p.first, p.second);
        }
        else {
            Assert<FileError>(syncIn.empty(), name());
            freq_ = UNDEFINEDSETTYPE;
        }

        // iouts_
        long number = static_cast<long>(dut->NumberOutputs());
        Assert<FileError>(!iout1.empty(), name());
        iouts_.push_back(convertSymbolicLoad(ConverterOutput::ONE, iout1));

        if ( iout2.empty() ) 
            Assert<FileError>(1 == number, name());           
        else {
            iouts_.push_back(convertSymbolicLoad(ConverterOutput::TWO, iout2));
            if ( iout3.empty() )
                Assert<FileError>(2 == number, name());
            else {
                iouts_.push_back(convertSymbolicLoad(ConverterOutput::THREE, iout3));
                if ( iout4.empty() ) 
                    Assert<FileError>(3 == number, name());
                else {
                    iouts_.push_back(convertSymbolicLoad(ConverterOutput::FOUR, iout4));
                    if ( iout5.empty() )
                        Assert<FileError>(4 == number, name());                    
                    else {
                        Assert<FileError>(5 == number, name());
                        iouts_.push_back(convertSymbolicLoad(ConverterOutput::FIVE,
                                                             iout5));
                    } // if-else
                } // if-else
            } // if-else
        } // if-else

        // maxLimit_
        Assert<FileError>(IsFloating(max), name());
        split = SplitString(max, '.');
        maxLimit_ = convert<MType>(max);
        if ( 2 == split.size() ) // Limit with precision > 0
            maxLimit_.SetPrecision(static_cast<long>(split[1].size()));
        else if ( 1 == split.size() )
            maxLimit_.SetPrecision(0); // Limit with precision == 0
        else
            throw(FileError(name()));    
        
        // measured_
        measured_ = UNDEFINEDMTYPE;

        // midMisc_
        vtemp = lf->GetMidtestMisc();
        i = vtemp.begin(), j = vtemp.end();
        while ( i != j ) {
            midMisc_.insert(ConvertToMisc(*i));
           ++i;
        }        

        // minLimit_
        Assert<FileError>(IsFloating(min), name());
        split = SplitString(min, '.');
        minLimit_ = convert<MType>(min);
        if ( 2 == split.size() ) // Limit with precision > 0
            minLimit_.SetPrecision(static_cast<long>(split[1].size()));
        else if ( 1 == split.size() ) // Limit with precision == 0
            minLimit_.SetPrecision(0);
        else
            throw(FileError(name()));       

        // miscDMM_ 
        vtemp = lf->GetMiscDMM();
        i = vtemp.begin(), j = vtemp.end();
        std::transform(i, j, std::back_inserter(miscDMM_), ConvertToMiscDMM);
        if ( ! miscDMM_.empty() )
            Assert<FileError>(1 == miscDMM_.size(), name());

        // miscOhms_        
        if ( !miscOhm.empty() ) {
            q = ScaleUnits<MType>::GetUnits(miscOhm);
            miscOhms_ = ScaleUnits<MType>::ScaleUp(q.first, q.second);
        }     
        else
            miscOhms_ = UNDEFINEDMTYPE;

        // nextIouts_;               
        if ( niout1.empty() ) {
            Assert<FileError>(niout2.empty() && niout3.empty() && 
                              niout4.empty() && niout5.empty(), name());
        }
        else {
            nextIouts_.push_back(convertSymbolicLoad(ConverterOutput::ONE, niout1));     
            if ( niout2.empty() ) 
                Assert<FileError>(1 == number, name());
            else {
                nextIouts_.push_back(convertSymbolicLoad(ConverterOutput::TWO, niout2));
                if ( niout3.empty() ) 
                    Assert<FileError>(2 == number, name());
                else {
                    nextIouts_.push_back(convertSymbolicLoad(ConverterOutput::THREE,
                                                             niout3));
                    if ( niout4.empty() ) 
                        Assert<FileError>(3 == number, name());
                    else {
                        nextIouts_.push_back(convertSymbolicLoad(ConverterOutput::FOUR,
                                                                 niout4));
                        if ( niout5.empty() )
                            Assert<FileError>(4 == number, name());                    
                        else {
                            Assert<FileError>(5 == number, name());
                            nextIouts_.push_back(
                                convertSymbolicLoad(ConverterOutput::FIVE, niout5)
                                                );
                        } // if-else
                    } // if-else
                } // if-else
            } // if-else                   
        }
        
        // nextVin_
	    if ( !nextVin.empty() )
            nextVin_ = convertSymbolicLine(nextVin);
        else
            nextVin_ = UNDEFINEDSETTYPE;

        // pretestMisc_
        vtemp = lf->GetPretestMisc();
        i = vtemp.begin(), j = vtemp.end();
        while ( i != j ) { 
            pretestMisc_.insert(ConvertToMisc(*i));
            ++i;
        }

        // priInhibited_
        priInhibited_ = isBoolean<FileError>(isPInh);        

        // refValue_
        refValue_ = UNDEFINEDSETTYPE;
        if ( ! ref.empty() ) {
            p = ScaleUnits<SetType>::GetUnits(ref);
            refValue_ = ScaleUnits<SetType>::ScaleUp(p.first, p.second);
            refValue_ = absolute(refValue_);
        }   

        // result_ 
        result_ = false;

        // secInhibited_
        secInhibited_ = isBoolean<FileError>(isSInh);

        // shortedChannels_
        if ( !short1.empty() && isBoolean<FileError>(short1) )
            shortedChannels_.push_back(ConverterOutput::ONE);
        if ( !short2.empty() && isBoolean<FileError>(short2) ) {
            Assert<FileError>(number > 1, name());
            shortedChannels_.push_back(ConverterOutput::TWO);
        }
        if ( !short3.empty() && isBoolean<FileError>(short3) ) {
            Assert<FileError>(number > 2, name());
            shortedChannels_.push_back(ConverterOutput::THREE);
        }
        if ( !short4.empty() && isBoolean<FileError>(short4) ) {
            Assert<FileError>(number > 3, name());
            shortedChannels_.push_back(ConverterOutput::FOUR);
        }
        if ( !short5.empty() && isBoolean<FileError>(short5) ) {
            Assert<FileError>(number > 4, name());
            shortedChannels_.push_back(ConverterOutput::FIVE);
        }

        // speedup_
        speedup_ = isBoolean<FileError>(speedUp);

        // testname_
        testname_ = pName;
        RemoveFrontBackSpace(testname_);
        Assert<FileError>(! testname_.empty(), name());              
        
        // units_
        Assert<FileError>(ScaleUnits<MType>::IsUnits(units), name());
        units_ = units;

	    // vin_
        vin_ = convertSymbolicLine(vin);

        // vRamp_
        vRamp_ = (nextVin_ != UNDEFINEDSETTYPE);

    } catch(BadArg b) {  // type thrown by ScaleUnits class members on bad input
        throw(FileError(LimitsFile::Name()));
    }     
}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//=====================================================================================//
//----------------------------------> TestStepInfo <-----------------------------------//
//=====================================================================================//

//=============
// Constructor
//=============
TestStepInfo::TestStepInfo(LimitsFile* lf) : tsiImpl_(new TestStepInfoImpl(lf))
{ /* */ }

//============
// Destructor
//============
TestStepInfo::~TestStepInfo() 
{ /* */ }

//==================
// Copy constructor
//==================
TestStepInfo::TestStepInfo(const TestStepInfo& source) 
  : tsiImpl_(new TestStepInfoImpl(*(source.tsiImpl_)))
{ /* */ }

//=====================
// Assignment operator
//=====================
TestStepInfo& TestStepInfo::operator=(const TestStepInfo& source) {
	if ( this != &source ) 
        tsiImpl_.reset(new TestStepInfoImpl(*(source.tsiImpl_)));	
	return(*this);
}

//=============
// Operator ==
//=============
bool operator==(const TestStepInfo& first, const TestStepInfo& second) {
    return(first.tsiImpl_->swTestname_ == second.tsiImpl_->swTestname_);
}

//=============
// Operator !=
//=============
bool operator!=(const TestStepInfo& first, const TestStepInfo& second) {
    return(first.tsiImpl_->swTestname_ != second.tsiImpl_->swTestname_);
}

//============
// Operator >
//============
bool operator>(const TestStepInfo& first, const TestStepInfo& second) {
    return(first.tsiImpl_->swTestname_ > second.tsiImpl_->swTestname_);
}

//=============
// Operator >=
//=============
bool operator>=(const TestStepInfo& first, const TestStepInfo& second) {
    return(first.tsiImpl_->swTestname_ >= second.tsiImpl_->swTestname_);
}

//============
// Operator <
//============
bool operator<(const TestStepInfo& first, const TestStepInfo& second) {
    return(first.tsiImpl_->swTestname_ < second.tsiImpl_->swTestname_);    
}

//=============
// Operator <=
//=============
bool operator<=(const TestStepInfo& first, const TestStepInfo& second) {
    return(first.tsiImpl_->swTestname_ <= second.tsiImpl_->swTestname_);   
}

//=====================
// Conversion function 
//=====================
TestStepInfo::operator TestStepInfo::CondPtr() const { 
    return(tsiImpl_->conditions_); 
}

//=====================
// Conversion function
//=====================
TestStepInfo::operator TestStepInfo::TSPtr() const {
    return(tsiImpl_->testStep_);
}

//================
// setErrorCode()
//================
void TestStepInfo::setErrorCode(long code) {
    tsiImpl_->eCode_ = code;
}

//====================
// setMeasuredValue()
//====================
void TestStepInfo::setMeasuredValue(const ProgramTypes::MType& value) {
    tsiImpl_->measured_ = value;
}

//============================
// setMeasuredValueExplicit()
//============================
void TestStepInfo::setMeasuredValueExplicit(const std::string& value) {
    tsiImpl_->measured_.SetExplicit(value);
}

//===========
// setName()
//===========
void TestStepInfo::setName(const std::string& name) {
    tsiImpl_->swTestname_ = name;
}

//=============
// setResult()
//=============
void TestStepInfo::setResult(bool result) {
    tsiImpl_->result_ = result;
}

//===============
// setTestName()
//===============
void TestStepInfo::setTestName(const std::string& name) {
    tsiImpl_->testname_ = name;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//=====================================================================================//
//----------------------------> TestStepInfo::Conditions <-----------------------------//
//=====================================================================================//

//=============
// Constructor
//=============
TestStepInfo::Conditions::Conditions(const TestStepInfoImpl* tsi) : tsi_(tsi)
{ /* */ }  

//============
// Destructor
//============
TestStepInfo::Conditions::~Conditions()
{ /* don't delete tsi_ */ }

//==============
// APSPrimary()
//==============
SetType TestStepInfo::Conditions::APSPrimary() const {
    return(tsi_->apsPrimary_);
}

//================
// APSSecondary()
//================
SetType TestStepInfo::Conditions::APSSecondary() const {
    return(tsi_->apsSecondary_);
}

//======
// BW()
//======
FilterSelects::FilterType TestStepInfo::Conditions::BW() const {
    return(tsi_->bandwidth_); 
}

//===========
// Channel()
//===========
ConverterOutput::Output TestStepInfo::Conditions::Channel() const { 
    return(tsi_->currentChannel_); 
}

//========
// Freq()
//========
SetType TestStepInfo::Conditions::Freq() const { 
    return(tsi_->freq_); 
}

//=========
// Iouts()
//=========
const TestStepInfo::SetTypeContainer& TestStepInfo::Conditions::Iouts() const { 
    return(tsi_->iouts_); 
}

//=============
// IoutsNext()
//=============
const TestStepInfo::SetTypeContainer& TestStepInfo::Conditions::IoutsNext() const { 
    return(tsi_->nextIouts_); 
}

//===============
// IsInhibited()
//===============
bool TestStepInfo::Conditions::IsInhibited() const {
    return(PrimaryInhibited() || SecondaryInhibited());
}

//===============
// MidtestMisc()
//===============
const std::set<ControlMatrixTraits::RelayTypes::MiscRelay>& 
                                      TestStepInfo::Conditions::MidtestMisc() const {
    return(tsi_->midMisc_); 
}

//===========
// MiscDMM()
//===========
std::vector<SwitchMatrixTraits::RelayTypes::DCRelay>
                               TestStepInfo::Conditions::MiscDMM() const { 
    return(tsi_->miscDMM_); 
}

//============
// MiscOhms()
//============
ProgramTypes::MType TestStepInfo::Conditions::MiscOhms() const {
    return(tsi_->miscOhms_);
}

//===============
// PretestMisc()
//===============
const std::set<ControlMatrixTraits::RelayTypes::MiscRelay>& 
                                       TestStepInfo::Conditions::PretestMisc() const {
    return(tsi_->pretestMisc_);
}

//====================
// PrimaryInhibited()
//====================
bool TestStepInfo::Conditions::PrimaryInhibited() const { 
    return(tsi_->priInhibited_);
}

//==========
// RefVal()
//==========
SetType TestStepInfo::Conditions::RefVal() const { 
    return(tsi_->refValue_);
}

//======================
// SecondaryInhibited()
//======================
bool TestStepInfo::Conditions::SecondaryInhibited() const {
    return(tsi_->secInhibited_);
}

//===========
// Shorted()
//===========
const std::vector<ConverterOutput::Output>& TestStepInfo::Conditions::Shorted() const {
    return(tsi_->shortedChannels_);
}

//===========
// Speedup()
//===========
bool TestStepInfo::Conditions::Speedup() const {
    return(tsi_->speedup_);
}

//==========
// SyncIn()
//==========
bool TestStepInfo::Conditions::SyncIn() const { 
    return(tsi_->freq_ != UNDEFINEDSETTYPE);
}

//=======
// Vin()
//=======
SetType TestStepInfo::Conditions::Vin() const { 
    return(tsi_->vin_);
}

//===========
// VinNext()
//===========
SetType TestStepInfo::Conditions::VinNext() const			       
	{ return(tsi_->nextVin_); }

//===========
// VinRamp()
//===========
bool TestStepInfo::Conditions::VinRamp() const 
    { return(tsi_->vRamp_); }

//============
// operator==
//============
bool operator==(const TestStepInfo::Conditions& first, 
                const TestStepInfo::Conditions& second) {
    return(
              first.APSPrimary()         == second.APSPrimary()         &&
              first.APSSecondary()       == second.APSSecondary()       &&
              first.BW()                 == second.BW()                 &&
              first.Channel()            == second.Channel()            &&
              first.Freq()               == second.Freq()               &&
              first.Iouts()              == second.Iouts()              &&
              first.IoutsNext()          == second.IoutsNext()          &&
              first.IsInhibited()        == second.IsInhibited()        &&
              first.MidtestMisc()        == second.MidtestMisc()        &&
              first.MiscDMM()            == second.MiscDMM()            &&
              first.MiscOhms()           == second.MiscOhms()           &&
              first.PretestMisc()        == second.PretestMisc()        &&
              first.PrimaryInhibited()   == second.PrimaryInhibited()   &&
              first.RefVal()             == second.RefVal()             &&
              first.SecondaryInhibited() == second.SecondaryInhibited() &&
              first.Shorted()            == second.Shorted()            &&
              first.Speedup()            == second.Speedup()            &&
              first.SyncIn()             == second.SyncIn()             &&
              first.Vin()                == second.Vin()                &&
              first.VinNext()            == second.VinNext()
          );
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


//=====================================================================================//
//-----------------------------> TestStepInfo::TestStep <------------------------------//
//=====================================================================================//

//=============
// Constructor
//=============
TestStepInfo::TestStep::TestStep(const TestStepInfoImpl* tsi) : tsi_(tsi)
{ /* */ }  

//============
// Destructor
//============
TestStepInfo::TestStep::~TestStep() 
{ /* don't delete tsi_ */ }

//=============
// ErrorCode()
//=============
long TestStepInfo::TestStep::ErrorCode() const {
    return(tsi_->eCode_);
}

//==========
// Limits()
//==========
TestStepInfo::PairMType TestStepInfo::TestStep::Limits() const { 
    return(std::make_pair(tsi_->minLimit_, tsi_->maxLimit_));
}

//=================
// MeasuredValue()
//=================
MType TestStepInfo::TestStep::MeasuredValue() const { 
    return(tsi_->measured_);
}

//==========
// Result()
//==========
bool TestStepInfo::TestStep::Result() const { 
    return(tsi_->result_);
}

//================
// ScaledLimits()
//================
TestStepInfo::PairMType TestStepInfo::TestStep::ScaledLimits(ScaleDirection dir) const {
    PairMType p;
    switch(dir) {
        case DOWN:
            p.first = ScaleUnits<MType>::ScaleDown(Limits().first, 
                                              ScaleUnits<MType>::MakeUnits(Units())); 
            p.second = ScaleUnits<MType>::ScaleDown(Limits().second,
                                              ScaleUnits<MType>::MakeUnits(Units()));
            break;
        default: // UP
            p.first = ScaleUnits<MType>::ScaleUp(Limits().first, 
                                            ScaleUnits<MType>::MakeUnits(Units())); 
            p.second = ScaleUnits<MType>::ScaleUp(Limits().second,
                                            ScaleUnits<MType>::MakeUnits(Units()));
    };
    return(p);
}

//====================
// SoftwareTestName()
//====================
std::string TestStepInfo::TestStep::SoftwareTestName() const {
    return(tsi_->swTestname_);
}

//============
// TestName()
//============
const std::string& TestStepInfo::TestStep::TestName() const {
    return(tsi_->testname_);
}

//=========
// Units()
//=========
const std::string& TestStepInfo::TestStep::Units() const {
    return(tsi_->units_);
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
