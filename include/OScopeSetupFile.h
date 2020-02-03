// Macro Guard
#ifndef OSCOPE_SETUP_FILE_H
#define OSCOPE_SETUP_FILE_H

// Files included
#include "GenericAlgorithms.h"
#include "NoCopy.h"
#include "OScopeParameters.h"
#include "ProgramTypes.h"
#include "SingletonType.h"
#include "SPTSFiles.h"
#include "StandardFiles.h"
#include "Switch.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
  ==============
  10/11/04, sjn,
  ==============
      Changed name of public member function NewFamilyNumber() to Reload().
      Added TRIGVERTSCALE and TRIGCHANCOUPLING to Parameters enumeration type, and added
        getValue() overloads for these new values.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    typedef StationExceptionTypes::FileError       FileError;
    typedef StationExceptionTypes::Undefined       Undefined;
    typedef StationExceptionTypes::UnexpectedState UnexpectedState;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct OScopeSetupFile : public OScopeParameters, private NoCopy {
    //==============
    // Public Enums
    //==============
    enum Parameters {
        BANDWIDTH = 0,
        COUPLING,
        HORMEMORYSETTING,
        HORSCALE,
        OFFSET,
        TRIGCHANCOUPLING,
        TRIGCOUPLING,
        TRIGLEVEL,
        TRIGMODE,
        TRIGSLOPE,
        TRIGSOURCE,
        TRIGVERTSCALE,
        VERTSCALE
    };

    //==================
    // Public Interface
    //==================
    template <typename RtnType, int parameter> 
    RtnType GetValue() {
        Assert<UnexpectedState>(!locked_, name());
        value_ = Uppercase(((*(params_))[parameter].second));
        masterValue_ = Uppercase(((*(masterParams_))[parameter].second));
        if ( masterValue_ == "SOFTWARE DEFINED" ) {
            Assert<FileError>(value_ == "UNDEFINED", name());
            throw(Undefined());
        }
        masterSet_ = (masterValue_ != "UNDEFINED");
        valueSet_ = (value_ != "UNDEFINED");
        Assert<FileError>(masterSet_ != valueSet_, name());
        return(getValue(Int2Type<parameter>()));
    }
    std::string Name();
    void Reload();
    void SetTestName(const std::string& testName);

private:
    friend class SingletonType<OScopeSetupFile>;
    OScopeSetupFile();
    ~OScopeSetupFile();
    bool operator=(const OScopeSetupFile&);

private:
    // Overloaded, private getValue() functions
    Switch getValue(Int2Type<BANDWIDTH>);
    CouplingType getValue(Int2Type<COUPLING>);
    ProgramTypes::SetType getValue(Int2Type<HORSCALE>);
    ProgramTypes::SetType getValue(Int2Type<OFFSET>);
    MemorySetting getValue(Int2Type<HORMEMORYSETTING>);
    CouplingType getValue(Int2Type<TRIGCHANCOUPLING>);
    TriggerCouplingType getValue(Int2Type<TRIGCOUPLING>);
    ProgramTypes::SetType getValue(Int2Type<TRIGLEVEL>);
    TriggerMode getValue(Int2Type<TRIGMODE>);
    SlopeType getValue(Int2Type<TRIGSLOPE>);
    OScopeChannels::Channel getValue(Int2Type<TRIGSOURCE>);
    ProgramTypes::SetType getValue(Int2Type<TRIGVERTSCALE>);
    ProgramTypes::SetType getValue(Int2Type<VERTSCALE>);

private:
    std::string name();

private:
    typedef FileTypes::ScopeSetupFileType SF;
    const long number_;
    std::string value_, masterValue_;
    std::auto_ptr<SF> sf_, masterFile_;
    std::auto_ptr<SF::Parameters> params_, masterParams_;
    bool locked_;
    bool valueSet_, masterSet_;
};

#endif // OSCOPE_SETUP_FILE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
