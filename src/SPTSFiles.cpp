// Files included
#include "Assertion.h"
#include "DateTime.h"
#include "GenericAlgorithms.h"
#include "SPTSException.h"
#include "SPTSFiles.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
Revision N.01, 10/7/08, MBuck
	Changed Parser in SPTSFiles<LimitsFileTag>::Tests, SPTSFiles<VariablesFileTag>::getTable
	SPTSFiles<VariablesFileTag>::GetVariables() to allow the use of the second, "alpha", dash
	to be used.

Revision ?.??, 10/2/07, MRB
	Tried modifying Load Transient tests to get better readings.

Revision L.01, 8/10/07, MRB
	Added Inhibit Cycling test.

Revision K.03, 8/02/07, MBuck
	Changed algorithm for radiation handling to be more in line with how system normally
	parses dash numbers in SPTSFiles<LimitsFileTag>::Tests, SPTSFiles<VariablesFileTag>::getTable
	SPTSFiles<VariablesFileTag>::GetVariables()
	Also added parsing for Trim Table

    ==============  
	6/19/07, MRB
    ==============  
	Modified SPTSFiles<LimitsFileTag>::Tests 
	Didn't take into account the Limits file parsing below.

    ==============  
   06/06/07, mbuck,
   ==============
   Modified SPTSFiles<VariablesFileTag>::GetVariables() to allow specification of different
    parameters for different radiation level based on the 2nd digit of dash number 'X_X#' (8th 
    element in 'startDash'.  
    There's a bit of a kludge to handle when there isn't a specified set of radiation 
	parameters in the part specification file by setting the 8th element of 'startDash' to 'X'.
	This turns 'startDash' back to -XXX#, actually [START-XXX#]

   ==============
   04/26/05, sjn,
   ==============
     Added function checkPtr() to unnamed namespace.  Needed to avoid performance
      warnings of current compiler.

	==============
   03/09/05, sjn,
   ==============
     Removing Delta Testing Capability from this software: Separate app has been made.
      Removed SPTSFiles<LimitsFileTag>::GetDeltaTestsCompare()
      Removed SPTSFiles<LimitsFileTag>::GetDeltaTestsStore()
      Removed SPTSFiles<LimitsFileTag>::GetDeltaValues()
      Removed SPTSFiles<LimitsFileTag>::SetDeltaValues()
      Removed all pda_ and readWord_ member variable references from
        SPTSFiles<LimitsFileTag> constructors.

   ==============  
   01/10/05, sjn,
   ==============
     Removed SPTSFiles<TestTypeFileTag>::GetTestTypes() and testTypes_ member var.
     Added GetVariableValue() overload to SPTSFiles<InstrumentFileTag>
     Added deviation "shadowing" implementation to both SPTSFiles<LimitsFileTag> and
      SPTSFiles<VariablesFileTag>.
     Added test engineering and engineering "shadowing" implementation to both
      SPTSFiles<LimitsFileTag> and SPTSFiles<VariablesFileTag>.
     Added FileFormatError typedef.
     Added local functions: checkAgainstDate() and findDeviationPath().
     Replaced helper function definitions with declarations.  Moved definitions to
       end of this file.
     Removed 'std::transform(info.begin(), info.end(), info.begin(), Uppercase);' from
       SPTSFiles<LimitsFileTag>::GetDeltaValues() --> making case sensitive.
     Modified SPTSFiles<LimitsFileTag>::SetDeltaValues() --> ensuring LHS = RHS has
       the form [Test_Type] Test_Name = Measurement Value.  The [Test_Type] portion is
       new and is required to support sophisticated delta testing requirements needed
       by some of Interpoint's parts.  The Test_Name portion has been made case
       sensitive.  Made dash number sensitive --> 002 is different from -012.
     Modified SPTSFiles<LimitsFileTag>::GetDeltaValues() --> made dash number sensitive.

*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    typedef StationExceptionTypes::BadArg          BadArg;
    typedef StationExceptionTypes::BadCommand      BadCommand;
    typedef StationExceptionTypes::FileError       FileError;
    typedef StationExceptionTypes::FileFormatError FileFormatError;
    typedef StationExceptionTypes::NoFileFound     NoFileFound;
    typedef StationExceptionTypes::NoFileInfo      NoFileInfo;
    typedef StationExceptionTypes::ProgramError    ProgramError;
    typedef StationExceptionTypes::UnexpectedState UnexpectedState;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { // unnamed --> helper prototypes

    // Function prototypes
    bool checkAgainstDate(const std::string& date);
    template <typename PtrType>
    bool checkPtr(const PtrType& ptr);
    template <typename EType>
    std::string findDeviationPath(const std::string& family, std::ifstream& ifile, 
                                  const std::string& wo, const std::string& dash,
                                  const std::string& whichFile);
    template <typename EType>
    std::string findEngineeringPath(const std::string& family, std::ifstream& ifile,
                                    const std::string& opID,
                                    const std::string& whichFile,
                                    const std::string& stationLoc);
    template <typename R, typename S>
    bool greaterFirst(const std::pair<R,S>& one, const std::pair<R,S>& two);
    bool isEqualFirst(const std::string& s, const std::string& t);
    struct noLeadingDash;
    std::vector< std::pair<std::string, std::string> > 
                     separate(const std::vector<std::string>& v, char delim = '=');
    void substituteFirst(const std::vector< std::pair<std::string, std::string> >& s,
                                                                   std::string& t);
} // end unnamed namespace





/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace FileTypes {

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> File Tag Types <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

struct GoldLimitsFileTag {
    static std::string Name() {
        return("Gold Limits File");
    }
};

struct InstrumentFileTag {
    static std::string Name() { 
        return("Instrument File");
    }
};

struct LimitsFileTag {
    static std::string Name() { 
        return("Limits File");
    }
};

struct PauseFileTag {
    static std::string Name() {
        return("Pause File");
    }
};

struct PDAFileTag {
    static std::string Name() {
        return("PDA File");
    }
};

struct PointerFileTag {
    static std::string Name() { 
        return("Pointer File");
    }
};

struct ScopeSetupFileTag {
    static std::string Name() { 
        return("Scope Setup File");
    }
};

struct StationFileTag {
    static std::string Name() { 
        return("Station File");
    }
};

struct TestFixtureFileTag {
    static std::string Name() { 
        return("Fixture File");
    }
};

struct TestTypeFileTag {
    static std::string Name() {
        return("Test Type File");
    }
};

struct VariablesFileTag {
    static std::string Name() { 
        return("Variables File");
    }
};


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>> SPTSFiles<PointerFileTag> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

template <>
struct SPTSFiles<PointerFileTag> {
    friend struct SPTSFiles<InstrumentFileTag>;
    friend struct SPTSFiles<LimitsFileTag>;    
    friend struct SPTSFiles<PauseFileTag>;
    friend struct SPTSFiles<PDAFileTag>;
    friend struct SPTSFiles<ScopeSetupFileTag>;
    friend struct SPTSFiles<StationFileTag>;
    friend struct SPTSFiles<TestFixtureFileTag>;
    friend struct SPTSFiles<TestTypeFileTag>;    
    friend struct SPTSFiles<VariablesFileTag>;

    //=============
    // Constructor
    //=============
    SPTSFiles() : fn_(0) 
    { /* */ }

    ~SPTSFiles() 
    { /* */ }  

private:
    //===============
    // getFilePath()
    //===============
    template <typename T> 
    std::string getFilePath(const std::string& header) {

        typedef std::vector< std::pair<std::string, std::string> > PairVec;

        if ( ! fn_.get() ) {
            /* Realize that Pointer.txt has a hardcoded path below.  It is
                debatable whether this should be the case.  But, for historical
                reasons, it is currently the case */
            std::ifstream inputPointerFile("C:\\SPTSFiles\\Pointer.txt");
            fn_.reset(new FileNode(inputPointerFile));
            Assert<FileError>(checkPtr(fn_.get()), PointerFileTag::Name());
        }

        // find pointer information for 'header'
        std::pair<std::string, std::string> p = FileNode::HeaderTags();
        std::string tmp = header;
        RemoveFrontBackSpace(tmp);
        FileNode* ptr = fn_->GetFileNode(p.first + tmp + p.second);       
        Assert<FileError>(checkPtr(ptr), PointerFileTag::Name());

        // Grab any "Starter Path" information in pointer file
        FileNode* shortPath = fn_->GetFileNode(p.first + "StarterPaths" + p.second);
        PairVec pv;
        if ( shortPath && shortPath->GetInfo().size() ) 
            pv = separate(shortPath->GetInfo());
        
        // find file path for T()        
        std::vector<std::string> v = ptr->GetInfo();
        Assert<FileError>(!v.empty(), PointerFileTag::Name());
        PairVec w = separate(v);
        PairVec::iterator i = w.begin(), j = w.end();
        std::string name = Uppercase(T::Name());
        while ( i != j ) { 
            if ( Uppercase(i->first) == name ) {
                substituteFirst(pv, i->second); // take care of starter path info
                return(i->second);  
            }
            ++i;          
        }                        
        throw(FileError(PointerFileTag::Name()));
    } 

private:
    std::auto_ptr<FileNode> fn_;
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
     // only instance of this class
     static SPTSFiles<PointerFileTag> pointerFile;
}  

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>> SPTSFiles<InstrumentFileTag> <<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

//==========================================
// SPTSFiles<InstrumentFileTag> Constructor
//==========================================
SPTSFiles<InstrumentFileTag>::SPTSFiles<InstrumentFileTag>() : fn_(0) {
    // Create a FileNode for the file that contains Instrument info     
    std::string instrFilePath 
             = pointerFile.getFilePath<Type>(Type::Name());
    std::ifstream infile(instrFilePath.c_str());
    fn_.reset(new FileNode(infile));
    Assert<NoFileFound>(fn_->Size() > 0, Type::Name());
}

//============
// Destructor
//============
SPTSFiles<InstrumentFileTag>::~SPTSFiles<InstrumentFileTag>() 
{ /* */ }

//================================
// GetVariableValue() - Overload1
//================================
std::string 
   SPTSFiles<InstrumentFileTag>::GetVariableValue(const std::string& instr, 
                                                  const std::string& variable) {

    // Get a FileNode pointer to instr
    std::pair<std::string, std::string> headTags = FileNode::HeaderTags();
    std::string type = headTags.first + instr + headTags.second;
    FileNode* ptr = fn_->GetFileNode(type);
    Assert<FileError>(checkPtr(ptr), Type::Name());

    // Get all info associated with instr
    std::vector<std::string> v = ptr->GetInfo();
    Assert<FileError>(!v.empty(), Type::Name());

    // Return value associated with variable
    typedef std::vector< std::pair<std::string, std::string> > PairVec;
    PairVec w = separate(v);
    PairVec::iterator i = w.begin(), j = w.end();
    std::string name = Uppercase(variable);
    while ( i != j ) {
        if ( Uppercase(i->first) == name )
            return(i->second);
        ++i;
    }
    throw(FileError(Type::Name()));
}

//================================
// GetVariableValue() - Overload2
//================================
std::string
   SPTSFiles<InstrumentFileTag>::GetVariableValue(const std::string& instr,
                                                  const std::string& model,
                                                  const std::string& variable) {
    // Get a FileNode pointer to instr
    std::pair<std::string, std::string> headTags = FileNode::HeaderTags();
    std::string type = headTags.first + instr + headTags.second;
    FileNode* ptr = fn_->GetFileNode(type);
    Assert<FileError>(checkPtr(ptr), Type::Name(), instr);

    // Get a FileNode pointer to model
    type = model;
    std::vector<FileNode*> children = ptr->GetChildNodes();
    std::vector<FileNode*>::iterator a = children.begin(), b = children.end();
    while ( a != b ) {
        std::string h = Uppercase((*a)->GetHeader());
        std::string::size_type st = h.find(headTags.first);
        Assert<FileError>(st != std::string::npos, Type::Name());
        h = h.substr(headTags.first.size() + 1);
        Assert<FileError>(!h.empty(), Type::Name());
        h = h.substr(0, h.find(headTags.second));
        Assert<FileError>(!h.empty(), Type::Name());
        
        if ( isEqualFirst(h, Uppercase(model)) )
            break;        
        ++a;
    } // while
    Assert<FileError>(a != b, Type::Name(), model);
    ptr = *a;

    // Get all info associated with model
    std::vector<std::string> v = ptr->GetInfo();
    Assert<FileError>(!v.empty(), Type::Name());

    // Return value associated with variable
    typedef std::vector< std::pair<std::string, std::string> > PairVec;
    PairVec w = separate(v);
    PairVec::iterator i = w.begin(), j = w.end();
    std::string name = Uppercase(variable);
    while ( i != j ) {
        if ( Uppercase(i->first) == name )
            return(i->second);
        ++i;
    }
    throw(FileError(Type::Name()));
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SPTSFiles<LimitsFileTag> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

//================================================
// SPTSFiles<LimitsFileTag> Constructor Overload1
//================================================
SPTSFiles<LimitsFileTag>::SPTSFiles<LimitsFileTag>(const std::string& familyNumber,
                                                   const std::string& workOrder,
                                                   const std::string& dash,
                                                   const std::string& opID,
                                                   const std::string& stationLoc,
                                                   bool testEngTest,
                                                   bool engTest)
                            : fn_(0), tests_(0), dashNumber_(""), isGold_(false),
                              isDev_(false) {

    //---------------------------------------------------------------------------
    // Create a FileNode for the file that contains Limits info for FamilyNumber
    // Limits are loaded with the following precedence rules:
    //---------------------------------------------------------------------------
    //   (1) Test Engineering Limits (when applicable)
    //   (2) Engineering Limits (when applicable)
    //   (3) Deviation Limits (when applicable)
    //   (4) Normal, Production Limits (always applicable)
    //---------------------------------------------------------------------------    

    std::string whichFile = "LIMFILE";
    std::string limFilePath;
    bool eng = false, dev = false, testEng = false;
    try {
        limFilePath = pointerFile.getFilePath<Type>(familyNumber);
        bool done = false;
        std::string::size_type pos = limFilePath.rfind("\\");
        Assert<FileError>(pos != std::string::npos, Type::Name());
        ++pos;
        std::string finalFile;

        if ( testEngTest ) { // try to load test engineering limits
            std::string tefile = limFilePath.substr(0, pos) + "TestEng.support";
            std::ifstream te(tefile.c_str());
            if ( te ) {
                finalFile = findEngineeringPath<FileFormatError>(familyNumber, 
                                                                 te, opID, 
                                                                 whichFile, stationLoc);
                if ( !finalFile.empty() ) { // found something
                    done = true;
                    testEng = true;
                }
            }            
        }
        if ( engTest && !done ) { // try to load engineering limits
            std::string efile = limFilePath.substr(0, pos) + "Engineering.support";
            std::ifstream engFile(efile.c_str());
            if ( engFile ) {
                finalFile = findEngineeringPath<FileFormatError>(familyNumber, 
                                                                 engFile, opID,
                                                                 whichFile, stationLoc);
                if ( !finalFile.empty() ) { // found something
                    done = true;
                    eng = true;
                }
            }
        }
        if ( !done ) { // try to load deviation limits
            std::string file = limFilePath.substr(0, pos) + "Deviation.support";
            std::ifstream deviations(file.c_str());
            if ( deviations ) {
                finalFile = findDeviationPath<FileFormatError>(familyNumber, 
                                                               deviations,
                                                               workOrder, dash, 
                                                               whichFile);
                dev = true;
                isDev_ = true;
            }
        }
        if ( finalFile.empty() )
            throw(false); // nothing found --> load regular limits

        finalFile = limFilePath.substr(0, pos) + finalFile;
        try {
            std::ifstream infile(finalFile.c_str());
            fn_.reset(new FileNode(infile));
        } catch(...) { // If non-production limits exist, then we must load them
            isDev_ = false;
            std::string fileType = eng ? "engineering limits" : "deviation limits";
            fileType = testEng ? "test eng limits" : fileType;
            throw(FileFormatError("Could not find " + fileType + ": " + finalFile));
        }
    } catch(FileFormatError& ffe) {
        isDev_ = false;
        throw(ffe);
    } catch(...) { // no non-production file exists
        try {
            isDev_ = false;
            std::ifstream infile(limFilePath.c_str());
            fn_.reset(new FileNode(infile));
        } catch(...) { throw(NoFileFound(Type::Name())); }
    }
    Assert<NoFileFound>(fn_->Size() > 0, Type::Name());
    famNumber_ = familyNumber;
}

//================================================
// SPTSFiles<LimitsFileTag> Constructor Overload2
//================================================
SPTSFiles<LimitsFileTag>::SPTSFiles(const std::string& familyNumber, bool) 
                            : fn_(0), tests_(0), dashNumber_(""), isGold_(true),
                              isDev_(false) {
    // Should be called for gold standards only
    std::string limFilePath = pointerFile.getFilePath<GoldType>(familyNumber);
    std::ifstream infile(limFilePath.c_str());
    fn_.reset(new FileNode(infile));
    Assert<NoFileFound>(fn_->Size() > 0, GoldType::Name());
    famNumber_ = familyNumber;
}

//============
// Destructor
//============
SPTSFiles<LimitsFileTag>::~SPTSFiles<LimitsFileTag>() 
{ /* */ }

//==================
// GetATPRevision()
//==================
std::string SPTSFiles<LimitsFileTag>::GetATPRevision() {
    std::pair<std::string, std::string> headTags = FileNode::HeaderTags();
    std::string atp = headTags.first + "ATP" + headTags.second;
    FileNode* fntmp = fn_->GetFileNode(atp);
    Assert<FileError>(checkPtr(fntmp), Type::Name());
    std::vector<std::string> v = fntmp->GetInfo();  
    Assert<FileError>(std::string::size_type(1) == v.size(), Type::Name());
    return(separate(v)[0].second);
}

//================
// GetGoldTests()
//================
SPTSFiles<LimitsFileTag>::Tests 
         SPTSFiles<LimitsFileTag>::GetGoldTests(const std::string& dashNumber,
                                                const std::string& serialNumber) {

    // find tests for specific dashNumber and testType
    Assert<BadCommand>(isGold_, GoldType::Name());
    std::string startDash = dashNumber;    
    RemoveFrontBackSpace(startDash);  
    Assert<BadArg>(!startDash.empty(), GoldType::Name());  
    if ( startDash[0] == '-' ) 
        startDash = startDash.substr(1);
    Assert<BadArg>(! startDash.empty(), GoldType::Name());

    // no alpha-extensions
    bool done = false;
    while ( ! done ) {
        std::string::size_type secondDash = startDash.find_first_of("-");
        if ( secondDash != std::string::npos ) 
            startDash = startDash.substr(0, secondDash); 
        else
            break;        
    }
    std::string newDash = startDash;

    startDash = "-" + RemoveAllWhiteSpace(Uppercase(startDash));
    std::pair<std::string, std::string> headTags = FileNode::HeaderTags();
    startDash = headTags.first + "Gold " + startDash + headTags.second;

    // look for explicit dash number
    FileNode* fntmp = fn_->GetFileNode(startDash);
    Assert<FileError>(checkPtr(fntmp), GoldType::Name()); 

    // grab gold standard information for explicit serial number
    std::string serial = RemoveAllWhiteSpace(Uppercase(serialNumber));
    serial = headTags.first + "SN " + serial + headTags.second;
    fntmp = fntmp->GetFileNode(serial);

    // make sure we found something for serial
    Assert<FileError>(checkPtr(fntmp), GoldType::Name()); 
 
    // grab all tests; make sure there is at least one
    std::vector<std::string> v = fntmp->GetInfo();  
    Assert<FileError>(!v.empty(), GoldType::Name()); 

    // gather all appropriate tests
    tests_.reset(new Tests(separate(v)));
    dashNumber_ = newDash; 

    return(*tests_);   
}
//============
// GetTests()
//============
SPTSFiles<LimitsFileTag>::Tests 
                SPTSFiles<LimitsFileTag>::GetTests(const std::string& dashNumber,
                                                   const std::string& testType) {

    // find tests for specific dashNumber and testType
    Assert<BadCommand>(!isGold_, Type::Name());
    std::string alphaDash = dashNumber;
	std::string baseDash = dashNumber;
    std::string startDash = dashNumber;
    std::string genDash = "X";
    std::string radDash = "X";
    std::string voltageDash = "X";
    std::string startGenDash = "-X";
    std::string genericDash = "-XXX";
	std::string buildDash = "-XXXX";
	std::string newDash;
	std::string::size_type secondDash = 0;
	std::string::size_type thirdDash = 0;
	bool done = false;
	bool noAlphaDash = false;
//    std::string qualDash = "X";
//    std::string packageDash = "X";
    RemoveFrontBackSpace(startDash);  
    Assert<BadArg>(!startDash.empty(), Type::Name());  
    if ( startDash[0] == '-' ) 
        startDash = startDash.substr(1);
    Assert<BadArg>(!startDash.empty(), Type::Name());

    startDash = "-" + RemoveAllWhiteSpace(Uppercase(startDash));
 
	// alpha-extensions
    std::pair<std::string, std::string> headTags = FileNode::HeaderTags();
    startDash = headTags.first + startDash + headTags.second;

    // look for explicit dash number first
    FileNode* fntmp = fn_->GetFileNode(startDash);

	if ( ! fntmp ) { // wasn't an exact match; perform some setup
		// Save alpha dash in alphaDash
        secondDash = alphaDash.find_first_of("-");
        thirdDash  = alphaDash.size();
		if (secondDash > thirdDash) { // no alpha dash
			noAlphaDash = true;
		}

		// strip off alpha-extensions
		while ( !done ) {
			secondDash = baseDash.find_first_of("-");
			if ( secondDash != std::string::npos ) 
				baseDash = baseDash.substr(0, secondDash); 
			else
				break;
		}
		secondDash = baseDash.size();
		if (secondDash == 3) {
			radDash     = baseDash[0];
//			packageDash = baseDash[1];
			voltageDash = baseDash[2];
		}
		else {
//			qualDash    = baseDash[0];
			radDash     = baseDash[1];
//			packageDash = baseDash[2];
			voltageDash = baseDash[3];
		}
	}


	// didn't find exact match; look for voltage only and alpha match
	if ( ! fntmp ) { 
		if ( ! noAlphaDash ) {
	        alphaDash = alphaDash.substr(secondDash, thirdDash); //secondDash); 
			startDash = 
				headTags.first + startGenDash + radDash + genDash + voltageDash + alphaDash  + headTags.second;
			fntmp = fn_->GetFileNode(startDash);
			// Don't look at Radiation value
			if ( ! fntmp ) {
				startDash = 
					headTags.first + genericDash + voltageDash + alphaDash  + headTags.second;
				fntmp = fn_->GetFileNode(startDash);
			}
		}
	}

    // no alpha-extensions
	if ( ! fntmp ) { 
	    done = false;
	    // look for explicit dash number first without an alpha extension
		startDash = 
			headTags.first + "-" + baseDash + headTags.second;
	    fntmp = fn_->GetFileNode(startDash);
	}

	// didn't find exact match; look for rad (3rd to last) and voltage (last) digits
	if ( ! fntmp ) { 
//		buildDash = 
//			headTags.first + startGenDash + radDash + genDash + voltageDash + headTags.second;
		startDash = 
			headTags.first + startGenDash + radDash + genDash + voltageDash + headTags.second;
 	    fntmp = fn_->GetFileNode(startDash);
	    }

	// didn't find specific radiation match; 
	// look for 3 digit unscreened, HX#, KX#, etc. or not a "P" or "R" part
	// parts specification should defined all unscreened parts as "O" ('oh')
	if ( ! fntmp ) { 
		if ( (radDash == "P") || radDash == "R") {  // 3 digit spec with a 'zero' = no radiation 
			// Leave it alone 
		} else {
			radDash = "O";
		}
//		buildDash = startGenDash + radDash + genDash + voltageDash;
		startDash = 
			headTags.first + startGenDash + radDash + genDash + voltageDash + headTags.second;
        fntmp = fn_->GetFileNode(startDash);
	}

	// didn't find any specific definitions; look for generic definition, last digit only, XXX# 
    if ( ! fntmp ) { 
		startDash = headTags.first + genericDash + voltageDash + headTags.second;
        fntmp = fn_->GetFileNode(startDash);
    }

    newDash = startDash;
	
    // make sure we found something for dashNumber
    Assert<FileError>(checkPtr(fntmp), Type::Name()); 

    // grab testType information for found dash number
    std::string testTypeStart = RemoveAllWhiteSpace(Uppercase(testType));
    testTypeStart = headTags.first + testTypeStart + headTags.second;
    fntmp = fntmp->GetFileNode(testTypeStart);

    // make sure we found something for testType
    Assert<FileError>(checkPtr(fntmp), Type::Name()); 
 
    // grab all tests; make sure there is at least one
    std::vector<std::string> v = fntmp->GetInfo();  
    Assert<FileError>(!v.empty(), Type::Name()); 

    // gather all appropriate tests
    tests_.reset(new Tests(separate(v)));
    dashNumber_ = newDash;
    
    return(*tests_);
}

//===================
// IsDeviationTest()
//===================
bool SPTSFiles<LimitsFileTag>::IsDeviationTest() const {
    return(isDev_);
}

//=============
// IsSetGold()
//=============
bool SPTSFiles<LimitsFileTag>::IsSetGold() {
    return(isGold_);
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SPTSFiles<PauseFileTag> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

//=====================================
// SPTSFiles<PauseFileTag> Constructor
//=====================================
SPTSFiles<PauseFileTag>::SPTSFiles<PauseFileTag>(const std::string& familyNumber)
                                                                     : fn_(0) {
    // Create a FileNode for the file that contains pause info for FamilyNumber
    std::string pausePath;
    try {
        pausePath = pointerFile.getFilePath<Type>(familyNumber);
    } catch(FileError&) {
        throw(NoFileFound(Type::Name()));
    }
    std::ifstream infile(pausePath.c_str());
    fn_.reset(new FileNode(infile));
    Assert<NoFileFound>(fn_->Size() > 0, Type::Name());    
}

//============
// Destructor
//============
SPTSFiles<PauseFileTag>::~SPTSFiles<PauseFileTag>() 
{ /* */ }

//============
// GetValue() 
//============
std::string SPTSFiles<PauseFileTag>::GetValue(const std::string& parameter) {
    
    // Grab all info
    std::vector<std::string> v = fn_->GetInfo();
    Assert<FileError>(!v.empty(), Type::Name());
    typedef std::vector< std::pair<std::string, std::string> > PairType;
    PairType w = separate(v);
    PairType::iterator i =
    w.begin(), j = w.end();
    std::string name = Uppercase(parameter);    

    // Find and return value associated with variable
    while ( i != j ) {
        if ( Uppercase(i->first) == name ) // found
            return(i->second);              
        ++i;
    }
    return(""); // not found
    
} 

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>> SPTSFiles<ScopeSetupFileTag> <<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

//==========================================
// SPTSFiles<ScopeSetupFileTag> Constructor
//==========================================
SPTSFiles<ScopeSetupFileTag>::SPTSFiles<ScopeSetupFileTag>
                                    (const std::string& familyNumber) : fn_(0) {
    // Create a FileNode for the file that contains scope setup info for FamilyNumber
    std::string setupFilePath = pointerFile.getFilePath<Type>(familyNumber);
    std::ifstream infile(setupFilePath.c_str());
    fn_.reset(new FileNode(infile));
    Assert<NoFileFound>(fn_->Size() > 0, Type::Name());
}

//============
// Destructor
//============
SPTSFiles<ScopeSetupFileTag>::~SPTSFiles<ScopeSetupFileTag>() 
{ /* */ }

//=================
// GetParameters()
//=================
SPTSFiles<ScopeSetupFileTag>::Parameters 
      SPTSFiles<ScopeSetupFileTag>::GetParameters(const std::string& setUpName) {
    std::pair<std::string, std::string> tags = FileNode::HeaderTags();
    std::string header = tags.first + setUpName + tags.second;
    FileNode* fntmp = fn_->GetFileNode(header);
    Assert<FileError>(checkPtr(fntmp), Type::Name());
    std::vector<std::string> v = fntmp->GetInfo();  
    Assert<FileError>(!v.empty(), Type::Name()); 
    return(separate(v));  
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/********************************gettab
*******************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SPTSFiles<StationFileTag> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

//=======================================
// SPTSFiles<StationFileTag> Constructor
//=======================================
SPTSFiles<StationFileTag>::SPTSFiles<StationFileTag>() : fn_(0) {
    // Create a FileNode for the file that contains Station info
    std::string stationPath = pointerFile.getFilePath<Type>(Type::Name());
    std::ifstream infile(stationPath.c_str());
    fn_.reset(new FileNode(infile));
    Assert<NoFileFound>(fn_->Size() > 0, Type::Name());
}

//============
// Destructor
//============
SPTSFiles<StationFileTag>::~SPTSFiles<StationFileTag>() 
{ /* */ }

//====================
// GetVariableValue()
//====================
std::string SPTSFiles<StationFileTag>::GetVariableValue(const std::string& variable) {
    std::vector<std::string> v = fn_->GetInfo();
    typedef std::vector< std::pair<std::string, std::string> > PairType;
    PairType w = separate(v);
    PairType::iterator i = w.begin(), j = w.end();
    std::string name = Uppercase(variable);

    // Find and return value associated with variable
    while ( i != j ) {
        if ( Uppercase(i->first) == name ) // found
            return(i->second);              
        ++i;
    }
    return(""); // not found
}

//====================
// GetVariableValue()
//====================
std::string SPTSFiles<StationFileTag>::GetVariableValue(const std::string& header,
                                                        const std::string& variable) {
    std::pair<std::string, std::string> p = FileNode::HeaderTags();
    std::string hName = p.first + header + p.second;
    FileNode* ptr = fn_->GetFileNode(hName);
    Assert<FileError>(checkPtr(ptr), Type::Name());

    std::vector<std::string> v = ptr->GetInfo();
    typedef std::vector< std::pair<std::string, std::string> > PairType;
    PairType w = separate(v);
    PairType::iterator i = w.begin(), j = w.end();
    std::string name = Uppercase(variable);

    // Find and return value associated with variable
    while ( i != j ) {
        if ( Uppercase(i->first) == name ) // found
            return(i->second);              
        ++i;
    }
    return(""); // not found
}

//====================
// SetVariableValue()
//====================
bool SPTSFiles<StationFileTag>::SetVariableValue(const std::string& header,
                                                 const std::string& variable,
                                                 const std::string& value) {
    std::pair<std::string, std::string> p = FileNode::HeaderTags();
    std::string hName = p.first + header + p.second;
    FileNode* ptr = fn_->GetFileNode(hName);
    Assert<FileError>(checkPtr(ptr), Type::Name());

    std::vector<std::string> v = ptr->GetInfo();
    typedef std::vector< std::pair<std::string, std::string> > PairType;
    PairType w = separate(v);
    PairType::iterator i = w.begin(), j = w.end();
    std::vector<std::string>::iterator viter = v.begin();
    std::string name = Uppercase(variable);

    while ( i != j ) {
        if ( Uppercase(i->first) == name ) { // found
            *viter = i->first + " = " + value;
            ptr->ReplaceInfo(v);
            break;
        }
        ++i;
        ++viter;
    }
    if ( i == j )
        return(false);

    // Update the actual file with new information and reset fn_
    std::string stationPath = pointerFile.getFilePath<Type>(Type::Name());
    std::ofstream of(stationPath.c_str());
    Assert<FileError>(checkPtr(of), Type::Name());
    of << *fn_;
    std::ifstream infile(stationPath.c_str());
    fn_.reset(new FileNode(infile));
    Assert<FileError>(fn_->Size() > 0, Type::Name());
    return(true);
}
   
/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>> SPTSFiles<TestFixtureFileTag> <<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

//===========================================
// SPTSFiles<TestFixtureFileTag> Constructor
//===========================================
SPTSFiles<TestFixtureFileTag>::SPTSFiles<TestFixtureFileTag>() : fn_(0) {
    // Create a FileNode for the file that contains Test Fixture info
    std::string testFixtPath = 
               pointerFile.getFilePath<Type>(Type::Name());
    std::ifstream infile(testFixtPath.c_str());
    fn_.reset(new FileNode(infile));
    Assert<NoFileFound>(fn_->Size() > 0, Type::Name());
}

//============
// Destructor
//============
SPTSFiles<TestFixtureFileTag>::~SPTSFiles<TestFixtureFileTag>() 
{ /* */ }

//====================
// GetVariableValue()
//====================
std::string 
    SPTSFiles<TestFixtureFileTag>::GetVariableValue(const std::string& fixtureName, 
                                                    const std::string& variable) {
    
    // Find fixtureName FileNode*
    std::pair<std::string, std::string> p = FileNode::HeaderTags();
    std::string fname = p.first + fixtureName + p.second;
    FileNode* ptr = fn_->GetFileNode(fname);
    Assert<FileError>(checkPtr(ptr), Type::Name());

    // Grab all info for fixtureName
    std::vector<std::string> v = ptr->GetInfo();
    Assert<FileError>(!v.empty(), Type::Name());
    typedef std::vector< std::pair<std::string, std::string> > PairType;
    PairType w = separate(v);
    PairType::iterator i = w.begin(), j = w.end();
    std::string name = Uppercase(variable);

    // Find and return value associated with variable
    while ( i != j ) {
        if ( Uppercase(i->first) == name ) // found
            return(i->second);              
        ++i;
    }
    return(""); // not found
}

//====================
// GetVariableValue()
//====================
std::string 
    SPTSFiles<TestFixtureFileTag>::GetVariableValue(const std::string& variable) {
    
    // Grab all info
    std::vector<std::string> v = fn_->GetInfo();
    Assert<FileError>(!v.empty(), Type::Name());
    typedef std::vector< std::pair<std::string, std::string> > PairType;
    PairType w = separate(v);
    PairType::iterator i = w.begin(), j = w.end();
    std::string name = Uppercase(variable);

    // Find and return value associated with variable
    while ( i != j ) {
        if ( Uppercase(i->first) == name ) // found
            return(i->second);              
        ++i;
    }
    return(""); // not found
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>> SPTSFiles<TestTypeFileTag> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

//========================================
// SPTSFiles<TestTypeFileTag> Constructor
//========================================
SPTSFiles<TestTypeFileTag>::SPTSFiles<TestTypeFileTag>() : roomTempTestTypes_(0) {
    // Create a FileNode for the file that contains Test Type info
    std::string testTypePath = 
                  pointerFile.getFilePath<Type>(Type::Name());
    std::ifstream infile(testTypePath.c_str());
    FileNode fn(infile);
    Assert<NoFileFound>(fn.Size() > 0, Type::Name());

    // Grab all Standard Room Temperature Tests
    std::pair<std::string, std::string> ht = FileNode::HeaderTags();
    std::string nodeName = ht.first + "Standard Room Temperature Tests" + ht.second;
    FileNode* ptr = fn.GetFileNode(nodeName);
    Assert<FileError>(checkPtr(ptr), Type::Name());
    roomTempTestTypes_.reset(new TestTypes(ptr->GetInfo()));
}

//============
// Destructor
//============
SPTSFiles<TestTypeFileTag>::~SPTSFiles<TestTypeFileTag>() 
{ /* */ }

//========================
// GetRoomTempTestTypes()
//========================
SPTSFiles<TestTypeFileTag>::TestTypes 
                              SPTSFiles<TestTypeFileTag>::GetRoomTempTestTypes() {
    return(*roomTempTestTypes_);
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>> SPTSFiles<VariablesFileTag> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

//=========================================
// SPTSFiles<VariablesFileTag> Constructor
//=========================================
SPTSFiles<VariablesFileTag>::SPTSFiles<VariablesFileTag>(
                                                   const std::string& familyNumber,
                                                   const std::string& workOrder,
                                                   const std::string& dash,
                                                   const std::string& opID,
                                                   const std::string& stationLoc,
                                                   bool testEngTest,
                                                   bool engTest)
                              : variables_(0), fn_(0), dashNumber_(""), isDev_(false) {

    //-----------------------------------------------------------------------------
    // Create a FileNode for the file that contains Variable info for FamilyNumber
    // Variables are loaded with the following precedence rules:
    //-----------------------------------------------------------------------------
    //   (1) Test Engineering Variables (when applicable)
    //   (2) Engineering Variables (when applicable)
    //   (3) Deviation Variables (when applicable)
    //   (4) Normal, Production Variables (always applicable)
    //--------------------------------------------------------------------------- 
    std::string variablePath;
    std::string whichFile = "VARFILE";
    bool eng = false, dev = false, testEng = false;
    try {
        bool done = false;
        variablePath = pointerFile.getFilePath<Type>(familyNumber);
        std::string::size_type pos = variablePath.rfind("\\");
        Assert<FileError>(pos != std::string::npos, Type::Name());
        ++pos;
        std::string finalFile;

        if ( testEngTest ) { // try to load test engineering variables
            std::string tefile = variablePath.substr(0, pos) + "TestEng.support";
            std::ifstream te(tefile.c_str());
            if ( te ) {
                finalFile = findEngineeringPath<FileFormatError>(familyNumber, 
                                                                 te, opID, 
                                                                 whichFile, stationLoc);
                if ( !finalFile.empty() ) { // found something
                    done = true;
                    testEng = true;
                }
            }
        }
        if ( engTest && !done ) { // try to load engineering variables
            std::string efile = variablePath.substr(0, pos) + "Engineering.support";
            std::ifstream engFile(efile.c_str());
            if ( engFile ) {
                finalFile = findEngineeringPath<FileFormatError>(familyNumber, 
                                                                 engFile, opID,
                                                                 whichFile, stationLoc);
                if ( !finalFile.empty() ) { // found something
                    done = true;
                    eng = true;
                }
            }
        }
        if ( !done ) { // try to load deviation variables
            std::string file = variablePath.substr(0, pos) + "Deviation.support";
            std::ifstream deviations(file.c_str());
            if ( deviations ) {
                finalFile = findDeviationPath<FileFormatError>(familyNumber, 
                                                               deviations,
                                                               workOrder, dash, 
                                                               whichFile);
                dev = true;
                isDev_ = true;
            }
        }
        if ( finalFile.empty() )
            throw(false); // nothing found --> load regular variables
        finalFile = variablePath.substr(0, pos) + finalFile;
        try {
            std::ifstream infile(finalFile.c_str());
            fn_.reset(new FileNode(infile));
        } catch(...) { // If non-production variables exist, then we must load them
            isDev_ = false;
            std::string fileType = eng ? "engineering variables" 
                                       : "deviation variables";
            fileType = testEng ? "test eng variables" : fileType;
            throw(FileFormatError("Could not find " + fileType + " " + finalFile));
        }
    } catch(FileFormatError& ffe) {
        isDev_ = false;
        throw(ffe);
    } catch(...) { // no deviation file exists
        try {
            isDev_ = false;
            std::ifstream infile(variablePath.c_str());
            fn_.reset(new FileNode(infile));
        } catch(...) { throw(NoFileFound(Type::Name())); }
    }
    Assert<NoFileFound>(fn_->Size() > 0, Type::Name());
}

//============
// Destructor
//============
SPTSFiles<VariablesFileTag>::~SPTSFiles<VariablesFileTag>() 
{ /* */ }

//=====================
// GetJumperPullIout()
//=====================
SPTSFiles<VariablesFileTag>::JumperPullTable 
      SPTSFiles<VariablesFileTag>::GetJumperPullIout(const std::string& output) {  

    return(getTable("JUMPERPULLIOUT", output));  
}

//=====================
// GetJumperPullVout()
//=====================
SPTSFiles<VariablesFileTag>::JumperPullTable 
      SPTSFiles<VariablesFileTag>::GetJumperPullVout(const std::string& output) {

    return(getTable("JUMPERPULLVOUT", output));
	  }

//===============
// GetRLLTable()
//===============
SPTSFiles<VariablesFileTag>::RLLTable 
    SPTSFiles<VariablesFileTag>::GetRLLTable(const std::string& output) {
    
    return(getTable("RLOADSOutput", output));
}

//============
// getTable()
//============
SPTSFiles<VariablesFileTag>::Variables 
     SPTSFiles<VariablesFileTag>::getTable(const std::string& toGet, 
                                           const std::string& output) {

    // Ensure dashNumber_ has been set already through GetVariables() call
    Assert<UnexpectedState>(!dashNumber_.empty(), Type::Name());

    std::string startDash = dashNumber_;

	RemoveFrontBackSpace(startDash);  
    Assert<BadArg>(!startDash.empty(), Type::Name());  
    if ( startDash[0] == '-' ) 
        startDash = startDash.substr(1);
    Assert<BadArg>(!startDash.empty(), Type::Name());

    startDash = RemoveAllWhiteSpace(Uppercase(startDash));

	// Jumper Pull info
    std::pair<std::string, std::string> headTags = FileNode::HeaderTags();
	std::string type = headTags.first + toGet + output + headTags.second;
 
	FileNode* fntmp = fn_->GetFileNode(startDash);

    fntmp = fntmp->GetFileNode(type);
    // Ensure we found some jumper pull information
    Assert<FileError>(checkPtr(fntmp), Type::Name());        
    Assert<FileError>(!fntmp->GetInfo().empty(), Type::Name());

    // Return Table
    return(separate(fntmp->GetInfo()));
}

//================
// GetVariables()
//================
void SPTSFiles<VariablesFileTag>::GetVariables(const std::string& dashNumber) {
	//                 SPTSFiles<LimitsFileTag>::GetTests(const std::string& dashNumber,
     //                                              const std::string& testType) {
//strip off alpha-extensions
  // find tests for specific dashNumber and testType
//    Assert<BadCommand>(!isGold_, Type::Name());
    std::string alphaDash = dashNumber;
	std::string baseDash = dashNumber;
    std::string startDash = dashNumber;
    std::string genDash = "X";
    std::string radDash = "X";
    std::string voltageDash = "X";
    std::string startGenDash = "-X";
    std::string genericDash = "-XXX";
	std::string buildDash = "-XXXX";
	std::string newDash;
	std::string::size_type secondDash = 0;
	std::string::size_type thirdDash = 0;
	bool done = false;
	bool noAlphaDash = false;
//    std::string qualDash = "X";
//    std::string packageDash = "X";
    RemoveFrontBackSpace(startDash);  
    Assert<BadArg>(!startDash.empty(), Type::Name());  
    if ( startDash[0] == '-' ) 
        startDash = startDash.substr(1);
    Assert<BadArg>(!startDash.empty(), Type::Name());

    startDash = "-" + RemoveAllWhiteSpace(Uppercase(startDash));
 
	// alpha-extensions
    std::pair<std::string, std::string> headTags = FileNode::HeaderTags();
    startDash = headTags.first + startDash + headTags.second;

    // look for explicit dash number first
    FileNode* fntmp = fn_->GetFileNode(startDash);

	if ( ! fntmp ) { // wasn't an exact match; perform some setup
		// Save alpha dash in alphaDash
        secondDash = alphaDash.find_first_of("-");
        thirdDash  = alphaDash.size();
		if (secondDash > thirdDash) { // no alpha dash
			noAlphaDash = true;
		}

			while ( !done ) {
			secondDash = baseDash.find_first_of("-");
			if ( secondDash != std::string::npos ) 
				baseDash = baseDash.substr(0, secondDash); 
			else
				break;
		}
		secondDash = baseDash.size();
		if (secondDash == 3) {
			radDash     = baseDash[0];
//			packageDash = baseDash[1];
			voltageDash = baseDash[2];
		}
		else {
//			qualDash    = baseDash[0];
			radDash     = baseDash[1];
//			packageDash = baseDash[2];
			voltageDash = baseDash[3];
		}
	}


	// didn't find exact match; look for voltage only and alpha match
	if ( ! fntmp ) { 
		if ( ! noAlphaDash ) {
	        alphaDash = alphaDash.substr(secondDash, thirdDash); //secondDash); 
			startDash = 
				headTags.first + startGenDash + radDash + genDash + voltageDash + alphaDash  + headTags.second;
			fntmp = fn_->GetFileNode(startDash);
			// Don't look at Radiation value
			if ( ! fntmp ) {
				startDash = 
					headTags.first + genericDash + voltageDash + alphaDash  + headTags.second;
				fntmp = fn_->GetFileNode(startDash);
			}
		}
	}

    // no alpha-extensions
	if ( ! fntmp ) { 
	    done = false;
	    // look for explicit dash number first without an alpha extension
		startDash = 
			headTags.first + "-" + baseDash + headTags.second;
	    fntmp = fn_->GetFileNode(startDash);
	}

	// didn't find exact match; look for rad (3rd to last) and voltage (last) digits
	if ( ! fntmp ) { 
//		buildDash = 
//			headTags.first + startGenDash + radDash + genDash + voltageDash + headTags.second;
		startDash = 
			headTags.first + startGenDash + radDash + genDash + voltageDash + headTags.second;
 	    fntmp = fn_->GetFileNode(startDash);
	    }

	// didn't find specific radiation match; 
	// look for 3 digit unscreened, HX#, KX#, etc. or not a "P" or "R" part
	// parts specification should defined all unscreened parts as "O" ('oh')
	if ( ! fntmp ) { 
		if ( (radDash == "P") || radDash == "R") {  // 3 digit spec with a 'zero' = no radiation 
			// Leave it alone 
		} else {
			radDash = "O";
		}
//		buildDash = startGenDash + radDash + genDash + voltageDash;
		startDash = 
			headTags.first + startGenDash + radDash + genDash + voltageDash + headTags.second;
        fntmp = fn_->GetFileNode(startDash);
	}

	// didn't find any specific definitions; look for generic definition, last digit only, XXX# 
    if ( ! fntmp ) { 
		startDash = headTags.first + genericDash + voltageDash + headTags.second;
        fntmp = fn_->GetFileNode(startDash);
    }

    newDash = startDash;
	
    // make sure we found something for dashNumber
    Assert<FileError>(checkPtr(fntmp), Type::Name()); 

    // grab testType information for found dash number
//    std::string testTypeStart = RemoveAllWhiteSpace(Uppercase(testType));
 //   testTypeStart = headTags.first + testTypeStart + headTags.second;
//    fntmp = fntmp->GetFileNode(testTypeStart);

 	// make sure we found something for dashNumber
    Assert<FileError>(checkPtr(fntmp), Type::Name()); 

    // grab all variables; make sure there is at least one
    std::vector<std::string> v = fntmp->GetInfo();  
    Assert<FileError>(! v.empty(), Type::Name()); 

    // gather all appropriate tests
    variables_.reset(new Variables(separate(v)));
    dashNumber_ = newDash;

}

//====================
// GetVariableValue()
//====================
std::string SPTSFiles<VariablesFileTag>::GetVariableValue(const std::string& variable) {

    Variables::const_iterator i = variables_->begin(), j = variables_->end();
    std::string name = Uppercase(variable);
    while ( i != j ) {
        if ( Uppercase(i->first) == name ) // found
            return(i->second);              
        ++i;
    }
    return(""); // not found
}

//===================
// IsDeviationTest()
//===================
bool SPTSFiles<VariablesFileTag>::IsDeviationTest() const {
    return(isDev_);
}

} // namespace FileTypes



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


namespace { // unnamed --> helper definitions

    //====================
    // checkAgainstDate()
    //====================
    bool checkAgainstDate(const std::string& date) {
        enum { yr = 2, month = 0, day = 1 };
        std::vector<std::string> currentDateInfo = SplitString(Date::CurrentDate(), '/');
        std::vector<std::string> fileDate = SplitString(date, '/');
        Assert<FileFormatError>(3 == currentDateInfo.size(), "Could not process date");
        Assert<FileFormatError>(3 == currentDateInfo.size(), "Bad deviation date");
        Assert<FileFormatError>(IsInteger(currentDateInfo[yr]),
                                "Could not process date");
        Assert<FileFormatError>(IsInteger(currentDateInfo[month]),
                                "Could not process date");
        Assert<FileFormatError>(IsInteger(currentDateInfo[day]),
                                "Could not process date");
        Assert<FileFormatError>(IsInteger(fileDate[yr]), "Bad deviation date");
        Assert<FileFormatError>(IsInteger(fileDate[month]), "Bad deviation date");
        Assert<FileFormatError>(IsInteger(fileDate[day]), "Bad deviation date"); 
        Assert<FileFormatError>(4 == currentDateInfo[yr].size(), "Bad deviation date"); 
        Assert<FileFormatError>(4 == fileDate[yr].size(), "Bad deviation date"); 

        // Check year
        long current = convert<long>(currentDateInfo[yr]);
        long file = convert<long>(fileDate[yr]);
        if ( file < current ) return(false);
        if ( file > current ) return(true);

        // Check month
        current = convert<long>(currentDateInfo[month]);
        file = convert<long>(fileDate[month]);
        if ( file < current ) return(false);
        if ( file > current ) return(true);

        // Check day
        current = convert<long>(currentDateInfo[day]);
        file = convert<long>(fileDate[day]);
        if ( file < current ) return(false);
        return(true);
    }

    //============
    // checkPtr()
    //============
    template <typename PtrType>
    bool checkPtr(const PtrType& ptr) {
        return(ptr != 0); // func used to get rid of 'performance warnings'
    }

    //=====================
    // findDeviationPath()
    //=====================
    template <typename EType>
    std::string findDeviationPath(const std::string& family, std::ifstream& ifile, 
                                  const std::string& wo, const std::string& dash, 
                                  const std::string& whichFile) {
        if ( !ifile ) return("");

        // typedefs
        typedef std::vector< std::pair<std::string, std::string> > PairVec;
        typedef std::vector<std::string> StrVec;

        // locals
        std::auto_ptr<FileNode> fileNode(new FileNode(ifile));
        if ( fileNode->Size() == 0 ) return("");
        std::set<std::string> applies;    
        std::size_t sz = fileNode->Size();
        FileNode* multFiles = 0, *child = 0, *fam;
        std::string filePath, lastFilePath;
        std::string date = "DATE", workOrder = "WORKORDER";
        std::string multDev = "MULTIPLE DEVIATIONS", name = "NAME", all = "ALL";
        std::string dashNumbers = "DASHES", familyNumber = "FAMILY";
        std::string devFile = "deviation file";
        noLeadingDash nld;
        std::string dashField = Uppercase(nld(dash));
        if ( dashField.empty() ) return("");

        // Check family number is correct
        std::pair<std::string, std::string> headerTags = FileNode::HeaderTags();
        std::string tmpStr = headerTags.first + familyNumber + headerTags.second;        
        fam = fileNode->GetFileNode(tmpStr);
        Assert<EType>(fam != 0, "No Family Number Found");
        std::vector<std::string> values = fam->GetInfo();
        std::vector<std::string>::iterator iter = values.begin();
        bool foundit = false;
        while ( iter != values.end() ) {
            std::vector<std::string> split = SplitString(*iter, '=');
            Assert<EType>(2 == split.size(), "Expected 'Token = Value'", devFile);
            if ( Uppercase(split[0]) == familyNumber ) {
                RemoveFrontBackSpace(split[1]);
                if ( Uppercase(split[1]) == Uppercase(family) ) {
                    foundit = true;
                    break;
                }
            }
            ++iter;
        }
        std::string errMsg = "Wrong (or no) Family Number Found in deviation";
        errMsg += " support file";
        Assert<EType>(foundit, errMsg);

        // Check each deviation listed for applicability
        long cntr = 0;
        std::string::size_type end = std::string::npos;
        while ( --sz > 0 ) {
            // Grab next deviation
            child = fileNode->GetChildNodes()[cntr++];

            // Make sure the file is in the proper format
            Assert<EType>(1 == child->Size(), "Deviation Support File Incorrect");
            std::vector<std::string> values = child->GetInfo();
            std::vector<std::string>::iterator iter = values.begin();
            errMsg = "All deviation information expected to be of the form: 'A = B'";
            while ( iter != values.end() ) {
                std::vector<std::string> tmpVec = SplitString(*iter, '=');
                Assert<EType>(2 == tmpVec.size(), errMsg);
                ++iter;
            }

            // If multDev or familyNumber nodes, don't count it here
            if ( Uppercase(child->GetHeader()).find(multDev) != end ) {
                multFiles = child;
                continue;
            }
            else if ( Uppercase(child->GetHeader()).find(familyNumber) != end ) {
                // ignore
                continue;
            }            

            // Locals
            StrVec info = child->GetInfo();
            bool check1 = false, check2 = false, check3 = false;
            bool all1 = false, all2 = false;
            std::string deviation = "";
            PairVec w = separate(info);
        
            // Ensure all required fields exist for this deviation
            PairVec::iterator i = w.begin(), j = w.end();
            bool fileCheck = false, dateCheck = false, woCheck = false;
            bool dashCheck = false, nameCheck = false;
            while ( i != j ) {
                std::string s = Uppercase(i->first);
                if ( s == Uppercase(whichFile) )
                    fileCheck = true;
                else if ( s == date )
                    dateCheck = true;
                else if ( s == workOrder )
                    woCheck = true;
                else if ( s == dashNumbers )
                    dashCheck = true;
                else if ( s == name )
                    nameCheck = true;
                ++i;
            }
            std::string errMsg = "Missing field for deviation: ";
            errMsg += child->GetHeader();
            Assert<EType>(woCheck && dashCheck && fileCheck && 
                          dateCheck && nameCheck, errMsg);

            // Check each work order listed, dashes listed and date of expiration
            i = w.begin();
            while ( i != j ) {
                if ( Uppercase(i->first) == workOrder ) { // check work order info
                    if ( Uppercase(i->second) == all ) { // all w/o's affected
                        all1 = true;
                        check1 = true;
                    }
                    else {
                        i->second = Uppercase(i->second);
                        StrVec allWOs;
                        allWOs = SplitString(i->second, ',');
                        std::sort(allWOs.begin(), allWOs.end());
                        StrVec::iterator toFind = std::find(allWOs.begin(),
                                                            allWOs.end(),
                                                            Uppercase(wo));
                        if ( toFind != allWOs.end() )
                            check1 = true; // work order is listed
                        else
                            break; // deviation does not apply to this w/o
                    }
                }             
                else if ( Uppercase(i->first) == date ) { // check within date
                    if ( Uppercase(i->second) == all ) { // no expiration
                        all2 = true;
                        check2 = true;
                    }
                    else if ( checkAgainstDate(i->second) ) // within expiration
                        check2 = true;
                    else
                        break; // deviation expired
                }
                else if ( Uppercase(i->first) == dashNumbers ) { // check dash
                    if ( Uppercase(i->second) == all ) // affects all dashes
                        check3 = true;
                    else {
                        StrVec allDashes;
                        i->second = Uppercase(i->second);
                        allDashes = SplitString(i->second, ',');
                        std::transform(allDashes.begin(), allDashes.end(),
                                       allDashes.begin(), noLeadingDash());
                        std::sort(allDashes.begin(), allDashes.end());
                        StrVec::iterator toFind = std::find(allDashes.begin(),
                                                            allDashes.end(),
                                                            dashField);
                        if ( toFind != allDashes.end() )
                            check3 = true; // dash number is listed
                        else
                            break; // deviation does not apply to this dash
                    }
                }
                else if ( Uppercase(i->first) == Uppercase(whichFile) )
                    lastFilePath = i->second;
                else if ( Uppercase(i->first) == name )
                    deviation = i->second;

                if ( check1 && check2 && check3 ) { // deviation applies
                    if ( !deviation.empty() && !lastFilePath.empty() ) {
                        if ( all1 && all2 ) {
                            std::string s = "Can't have 'ALL' for both ";
                            s += "Date and Work Order effectivity - deviation file";
                            throw(EType(s));
                        }
                        filePath = lastFilePath;
                        applies.insert(deviation);
                        break;
                    }                
                }
                ++i;
            } // while
            lastFilePath = "";
        } // while

        if ( 0 == applies.size() ) return(""); // no applicable deviation
        if ( 1 == applies.size() ) return(filePath); // only 1 deviation applies
        std::string msg = "Found > 1 deviation that applies:  ";
        msg += "need 'Multiple Deviations' section";
        Assert<EType>(checkPtr(multFiles), msg);

        // More than 1 deviation applies
        // Find file that supports the deviations we're after
        StrVec allInfo = multFiles->GetInfo();
        PairVec pv = separate(allInfo, '=');
        PairVec::iterator allBeg = pv.begin();
        std::string err = "Multi-deviations must be of the form: File(Dev1,Dev2,etc)";
        err += " = VariablesFilePathInfo";
        while ( allBeg != pv.end() ) {
            std::string next = Uppercase(allBeg->first);
            std::string::size_type pos = next.find(Uppercase(whichFile)), lParen;
            if ( pos != std::string::npos ) {
                lParen = next.find("("); 
                Assert<EType>(pos != std::string::npos, err);
                Assert<EType>(lParen > pos, err);
                std::string::size_type rParen = next.find(")");
                Assert<EType>(rParen != std::string::npos, err);
                Assert<EType>(rParen > lParen, err);
                StrVec devs;
                devs = SplitString(next.substr(lParen+1, rParen-lParen-1), ',');
                Assert<EType>(devs.size() > 1, err);
                std::sort(devs.begin(), devs.end());
                StrVec diff;
                std::set_intersection(applies.begin(), applies.end(), devs.begin(),
                                      devs.end(), std::back_inserter(diff));
                if ( diff.size() == applies.size() ) // found the path
                    return(allBeg->second);
            }
            ++allBeg;
        } // while
        return("");
    }

    //=======================
    // findEngineeringPath()
    //=======================
    template <typename EType>
    std::string findEngineeringPath(const std::string& family, std::ifstream& ifile,
                                    const std::string& opID, 
                                    const std::string& whichFile,
                                    const std::string& stationLoc) {
        if ( !ifile ) return("");

        // typedefs
        typedef std::vector< std::pair<std::string, std::string> > PairVec;
        typedef std::vector<std::string> StrVec;

        // locals
        std::auto_ptr<FileNode> fileNode(new FileNode(ifile));
        if ( fileNode->Size() == 0 ) return(""); 
        std::size_t sz = fileNode->Size();
        FileNode *child = 0, *fam = 0;
        std::string filePath, lastFilePath;
        std::string date = "DATE";
        std::string all = "ALL";
        std::string identify = "OPIDS";
        std::string familyNumber = "FAMILY";
        std::string stationLocation = "LOCATION";
        std::string engFile = "engineering file";

        // Check family number is correct
        std::pair<std::string, std::string> headerTags = FileNode::HeaderTags();
        std::string tmpStr = headerTags.first + familyNumber + headerTags.second;        
        fam = fileNode->GetFileNode(tmpStr);
        Assert<EType>(fam != 0, "No Family Number Found");
        std::vector<std::string> values = fam->GetInfo();
        std::vector<std::string>::iterator iter = values.begin();
        bool foundit = false;
        while ( iter != values.end() ) {
            std::vector<std::string> split = SplitString(*iter, '=');
            Assert<EType>(2 == split.size(), "Expected 'Token = Value'", engFile);
            if ( Uppercase(split[0]) == familyNumber ) {
                RemoveFrontBackSpace(split[1]);
                if ( Uppercase(split[1]) == Uppercase(family) ) {
                    foundit = true;
                    break;
                }
            }
            ++iter;
        }
        std::string errMsg = "Wrong (or no) Family Number Found in engineering";
        errMsg += " support file";
        Assert<EType>(foundit, errMsg);

        // Get listed file if applicable
        long cntr = 0;
        std::string::size_type end = std::string::npos;
        while ( --sz > 0 ) {
            // Grab next entry
            child = fileNode->GetChildNodes()[cntr++];

            // Make sure the file is in the proper format
            Assert<EType>(1 == child->Size(), "Engineering Support File Incorrect");
            std::vector<std::string> values = child->GetInfo();
            std::vector<std::string>::iterator iter = values.begin();
            errMsg = "All engineering information expected to be of the form: 'A = B'";
            while ( iter != values.end() ) {
                std::vector<std::string> tmpVec = SplitString(*iter, '=');
                Assert<EType>(2 == tmpVec.size(), errMsg);
                ++iter;
            }

            // If familyNumber node, don't count it here
            if ( Uppercase(child->GetHeader()).find(familyNumber) != end ) {
                // ignore
                continue;
            }

            // Locals
            StrVec info = child->GetInfo();
            bool check1 = false, check2 = false, check3 = false;
            bool all1 = false, all2 = false;
            PairVec w = separate(info);
        
            // Ensure all required fields exist
            PairVec::iterator i = w.begin(), j = w.end();
            bool fileCheck = false, dateCheck = false, idCheck = false;
            bool stationCheck = false;
            while ( i != j ) {
                std::string s = Uppercase(i->first);
                if ( s == Uppercase(whichFile) )
                    fileCheck = true;
                else if ( s == date )
                    dateCheck = true;
                else if ( s == identify )
                    idCheck = true;
                else if ( s == stationLocation )
                    stationCheck = true;
                ++i;
            }
            std::string errMsg = "Missing field for engineering file: ";
            errMsg += child->GetHeader();
            Assert<EType>(stationCheck && fileCheck && idCheck && dateCheck, errMsg);

            // Check date of expiration, operator id and station location
            i = w.begin();
            while ( i != j ) {           
                if ( Uppercase(i->first) == date ) { // check within date
                    if ( Uppercase(i->second) == all ) { // no expiration
                        all2 = true;
                        check1 = true;
                    }
                    else if ( checkAgainstDate(i->second) ) // within expiration
                        check1 = true;
                    else
                        break; // date expired
                }
                if ( Uppercase(i->first) == identify ) { // check operator ID
                    if ( Uppercase(i->second) == all ) { // everyone's OK
                        all1 = true;
                        check2 = true;
                    }
                    else {
                        i->second = Uppercase(i->second);
                        StrVec allIDs;
                        allIDs = SplitString(i->second, ',');
                        std::sort(allIDs.begin(), allIDs.end());
                        StrVec::iterator toFind = std::find(allIDs.begin(),
                                                            allIDs.end(),
                                                            Uppercase(opID));
                        if ( toFind != allIDs.end() )
                            check2 = true; // operator ID is listed
                        else
                            break; // not this operator
                    }
                } 
                else if ( Uppercase(i->first) == Uppercase(whichFile) )
                    lastFilePath = i->second;
                else if ( Uppercase(i->first) == stationLocation ) // check station
                    check3 = (Uppercase(i->second) == Uppercase(stationLoc));

                if ( check1 && check2 && check3 ) { // eng file applies
                    if ( !lastFilePath.empty() ) {
                        if ( all1 && all2 ) {
                            std::string s = "Can't have 'ALL' for both ";
                            s += "Date and Operator ID - eng support file";
                            throw(EType(s));
                        }
                        return(lastFilePath); // return from here
                    }                
                }
                ++i;
            } // while
            lastFilePath = "";
        } // while
        return("");
    }

    //================
    // greaterFirst()
    //================
    template <typename R, typename S>
    bool greaterFirst(const std::pair<R,S>& one, const std::pair<R,S>& two) {
        return(one.first > two.first);
    }    

    //================
    // isEqualFirst()
    //================
    bool isEqualFirst(const std::string& s, const std::string& t) {
        if ( t.size() < s.size() ) 
            return(false);
        std::pair<std::string::const_iterator, std::string::const_iterator> p;
        p = std::mismatch(s.begin(), s.end(), t.begin());
        return(p.first == s.end());
    }

    //=================
    // noLeadingDash()
    //=================
    struct noLeadingDash {
        std::string operator()(const std::string& str) {
            std::string toRtn = str;
            if ( toRtn.empty() ) return(toRtn);
            while ( toRtn[0] == '-' || toRtn[0] == ' ' ) {
                toRtn = toRtn.substr(1);
                if ( toRtn.empty() ) break;
            }
            return(toRtn);
        }
    };

    //============
    // separate()
    //============
    std::vector< std::pair<std::string, std::string> > 
               separate(const std::vector<std::string>& v, char delim) {

        std::vector< std::pair<std::string, std::string> > toRtn;
        for ( std::string::size_type idx = 0; idx < v.size(); ++idx ) {
            bool found = false;
            std::string::size_type tmp = v[idx].size()-1;
            for ( std::string::size_type i = 0; i < v[idx].size(); ++i ) {
                char next = v[idx].at(i);
                if ( next == delim ) {
                    Assert<FileError>(i != tmp, "SPTSFiles::separate()");
                    toRtn.push_back(
                        std::make_pair(v[idx].substr(0, i), v[idx].substr(i+1))
                    );
                    found = true;
                    break;
                } // if 
            } // for
            Assert<FileError>(found, "SPTSFiles::separate()");
        } // for
        for ( std::string::size_type idx = 0; idx < toRtn.size(); ++idx ) {
            RemoveFrontBackSpace(toRtn[idx].first); 
            RemoveFrontBackSpace(toRtn[idx].second);
            RemoveTabs(toRtn[idx].first);
            RemoveTabs(toRtn[idx].second);
        }
        return(toRtn);
    }

    //===================
    // substituteFirst()
    //===================
    void substituteFirst(const std::vector< std::pair<std::string, std::string> >& s,
                                                                   std::string& t) {
    
        typedef std::vector< std::pair<std::string, std::string> > WS;
        WS::const_iterator k = s.begin(), l = s.end();
        std::string tmp = Uppercase(t);
        while ( k != l ) {
            if ( isEqualFirst(Uppercase(k->first), tmp) ) {
                tmp = t.substr(k->first.size());
                RemoveFrontBackSpace(tmp);
                t = k->second + tmp;                
                break;
            }
            ++k;
        }
    }
} // end unnamed




/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/
    
/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
