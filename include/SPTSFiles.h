#ifndef SPTS_FILES_H
#define SPTS_FILES_H

// Files included
#include "FileNode.h"
#include "NoCopy.h"
#include "SingletonType.h"
#include "StandardFiles.h"



//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   03/09/05, sjn,
   ==============
    Removing Delta Testing Capability from this software: Separate app has been made.
      Removed SPTSFiles<LimitsFileTag>::GetDeltaTestsCompare()
      Removed SPTSFiles<LimitsFileTag>::GetDeltaTestsStore()
      Removed SPTSFiles<LimitsFileTag>::GetDeltaValues()
      Removed SPTSFiles<LimitsFileTag>::SetDeltaValues()
      Removed DELTATESTTYPESTART and DELTATESTTYPESTOP static constant chars from 
       SPTSFiles<LimitsFileTag>.
      Removed pda_ member variable from SPTSFiles<LimitsFileTag>
      Removed readWord_ member variable from SPTSFiles<LimitsFileTag>

   ==============  
   01/10/05, sjn,
   ==============
    Removed SPTSFiles<TestTypeFileTag>::GetTestTypes() and testTypes_ member var.
    Added GetVariableValue() overload to SPTSFiles<InstrumentFileTag> --> allows user
       to specify model# in addition to instrument type.
    Added deviation "shadowing" support to SPTSFiles<LimitsFileTag> --> added
       IsDeviationTest() and private variable.  Added formal parameters to one 
       constructor overload (dash number, work order, station location, operator
       ID, test engineering test and engineering test).
    Made the same changes to SPTSFiles<VariablesFileTag> to support deviations affecting
       variables files.
    Added DELTATESTTYPESTART and DELTATESTTYPESTOP static constant chars to 
      SPTSFiles<LimitsFileTag>.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//




namespace FileTypes {

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> File Tag Types <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

struct GoldLimitsFileTag;
struct InstrumentFileTag;
struct LimitsFileTag;
struct PauseFileTag;
struct PointerFileTag;
struct ScopeSetupFileTag;
struct StationFileTag;
struct TestFixtureFileTag;
struct TestTypeFileTag;
struct VariablesFileTag;

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

template <typename FileType>
struct SPTSFiles; // undefined

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>> SPTSFiles<InstrumentFileTag> <<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

template <>
struct SPTSFiles<InstrumentFileTag> : private NoCopy {
    // Public Interface
    SPTSFiles();    
    std::string GetVariableValue(const std::string& instr, const std::string& variable);
    std::string GetVariableValue(const std::string& instr, const std::string& model,
                                 const std::string& variable);
    ~SPTSFiles();

private:
    typedef InstrumentFileTag Type;

private:
    std::auto_ptr<FileNode> fn_;
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SPTSFiles<LimitsFileTag> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

template <>
struct SPTSFiles<LimitsFileTag> : private NoCopy {
    // Public Typedefs
    typedef std::string TestName;
    typedef std::string TestInfo;
    typedef std::vector< std::pair<TestName, TestInfo> > Tests;

    // Public Interface
    SPTSFiles(const std::string&, const std::string&, const std::string&, 
              const std::string&, const std::string&, bool, bool);
    SPTSFiles(const std::string& familyNumber, bool goldTest);
    std::string GetATPRevision();
    Tests GetGoldTests(const std::string& dashNumber, const std::string& serialNumber);
    Tests GetTests(const std::string& dashNumber, const std::string& testType);
    bool IsDeviationTest() const;
    bool IsSetGold();
    ~SPTSFiles();

private:
    typedef LimitsFileTag Type;
    typedef GoldLimitsFileTag GoldType;

private:
    std::auto_ptr<Tests> tests_;
    std::string dashNumber_;
    std::auto_ptr<FileNode> fn_;
    std::string famNumber_;
    bool isDev_;
    bool isGold_;
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SPTSFiles<PauseFileTag> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

template<>
struct SPTSFiles<PauseFileTag> : private NoCopy { 
    // Public Interface
    explicit SPTSFiles(const std::string& familyNumber);
    std::string GetValue(const std::string& parameter);
    ~SPTSFiles();

private:
    typedef PauseFileTag Type;

private:
    std::auto_ptr<FileNode> fn_;        
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>> SPTSFiles<ScopeSetupFileTag> <<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

template <>
struct SPTSFiles<ScopeSetupFileTag> : private NoCopy {
    // Public Typedefs
    typedef std::string ParmName;
    typedef std::string ParmValue;
    typedef std::vector< std::pair<ParmName, ParmValue> > Parameters;
 
    // Public Interface
    explicit SPTSFiles(const std::string& familyNumber);
    Parameters GetParameters(const std::string& testTypeName);
    ~SPTSFiles();

private:
    typedef ScopeSetupFileTag Type;

private:
    std::auto_ptr<FileNode> fn_;        
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>> SPTSFiles<StationFileTag> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

template <>
struct SPTSFiles<StationFileTag> : private NoCopy {

    // Public Interface
    SPTSFiles();
    std::string GetVariableValue(const std::string& variable);
    std::string GetVariableValue(const std::string& header, const std::string& variable);
    bool SetVariableValue(const std::string& header, const std::string& variable,
                          const std::string& value);
    ~SPTSFiles();

private:
    typedef StationFileTag Type;
    
private:
    std::auto_ptr<FileNode> fn_;
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>> SPTSFiles<TestFixtureFileTag> <<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

template <>
struct SPTSFiles<TestFixtureFileTag> : private NoCopy {
    // Public Interface
    SPTSFiles();
    std::string GetVariableValue(const std::string& fixtureName, 
                                 const std::string& variable);
    std::string GetVariableValue(const std::string& variable);
    ~SPTSFiles();

private:
    typedef TestFixtureFileTag Type;

private:
    std::auto_ptr<FileNode> fn_;
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>> SPTSFiles<TestTypeFileTag> <<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

template <>
struct SPTSFiles<TestTypeFileTag> : private NoCopy {
    // Public Typedefs
    typedef std::vector<std::string> TestTypes;

    // Public Interface
    SPTSFiles();
    TestTypes GetRoomTempTestTypes();
    ~SPTSFiles();

private:
    typedef TestTypeFileTag Type;

private:
    std::auto_ptr<TestTypes> roomTempTestTypes_;
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>> SPTSFiles<VariablesFileTag> <<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//

template <>
struct SPTSFiles<VariablesFileTag> : private NoCopy {
    // Public Typedefs
    typedef std::string VariableName;
    typedef std::string VariableValue;
    typedef std::vector< std::pair<VariableName, VariableValue> > Variables;
    typedef Variables JumperPullTable;
    typedef Variables RLLTable;

    // Public Interface
    SPTSFiles(const std::string&, const std::string&, const std::string&, 
              const std::string&, const std::string&, bool, bool);
    JumperPullTable GetJumperPullIout(const std::string& output);
    JumperPullTable GetJumperPullVout(const std::string& output);     
    RLLTable GetRLLTable(const std::string& output);
    void GetVariables(const std::string& dashNumber);
    std::string GetVariableValue(const std::string& variable);
    bool IsDeviationTest() const;
    ~SPTSFiles();

private:
    typedef VariablesFileTag Type;

private:
    Variables getTable(const std::string& toGet, const std::string& output); 

private:
    std::string dashNumber_;
    std::auto_ptr<Variables> variables_;
    std::auto_ptr<FileNode> fn_;
    bool isDev_;
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

    // Typedefs to be used by other files
    typedef SPTSFiles<LimitsFileTag>      LimitsFileType;
    typedef SPTSFiles<InstrumentFileTag>  InstrumentFileType;
    typedef SPTSFiles<PauseFileTag>       PauseFileType;
    typedef SPTSFiles<ScopeSetupFileTag>  ScopeSetupFileType;
    typedef SPTSFiles<StationFileTag>     StationFileType;
    typedef SPTSFiles<TestFixtureFileTag> TestFixtureFileType;
    typedef SPTSFiles<TestTypeFileTag>    TestTypesFileType;
    typedef SPTSFiles<VariablesFileTag>   VariablesFileType;

} // namespace FileTypes

#endif // SPTS_FILES_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
