#include "Assertion.h"
#include "JavaGUI.h"
#include "SPTSException.h"


#define PATH_SEPARATOR ';' /* define it to be ':' on Solaris */
#define USER_CLASSPATH "." /* where JavaGUI.class is */

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   11/20/05, sjn,
   ==============
     Modified Print() to take a string argument.  No longer going through a file for
      printing --> send directly to GUI software.

   ==============  
   03/26/05, sjn,
   ==============
     Added void SoakingAtTemperature(bool isSoaking) to support GEN-029 changes.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { // unnamed
    typedef StationExceptionTypes::BadArg     BadArg;
    typedef StationExceptionTypes::BadCommand BadCommand;

    struct JavaExceptionTag {
        static std::string Name() {
            return("Java Exception");
        }
    };

    typedef ExceptionTypes::SomeException<ExceptionTypes::NoErrorNumber, 
                                          UserInputExceptionTypes::BaseException, 
                                          JavaExceptionTag>  JavaException;

    struct JavaCodes {
        //================
        // AddArguments()
        //================
        static std::string AddArguments(const std::string& first, 
                                        const std::string& second) {            
            return(first + second);
        }

        //=============
        // MakeArray()
        //=============
        static std::string MakeArray(const std::string& arg) {
            return(std::string("[") + arg);
        }

        //============
        // MakeBool()
        //============
        static const std::string& MakeBool() {
            static std::string toRtn = "Z";
            return(toRtn);
        }

        //============================
        // MakeCallable() - overload1
        //============================
        static std::string MakeCallable(const std::string& argList, 
                                        const std::string& rtnType) {
            typedef std::string S;
            return(S("(") + argList + S(")") + rtnType);
        }

        //============================
        // MakeCallable() - overload2
        //============================
        static std::string MakeCallable(const std::string& rtn) {
            typedef std::string S;
            return(S(S("()") + rtn));
        }

        //===========
        // MakeInt()
        //===========
        static const std::string& MakeInt() {
            static std::string toRtn = "I";
            return(toRtn);
        }

        //==============
        // MakeString()
        //==============
        static const std::string& MakeString() {
            static std::string toRtn = "Ljava/lang/String;";
            return(toRtn);
        }

        //============
        // MakeVoid()
        //============
        static const std::string& MakeVoid() {
            static std::string toRtn = "V";
            return(toRtn);
        }
    };


}


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace DontUse { // only JavaGUI should use this
    struct JavaTypes {
        //==========================
        // MakeObject() - overload1
        //==========================
        static jboolean MakeObject(bool val) {
            jboolean toRtn = val;
            return(toRtn);
        }

        //==========================
        // MakeObject() - overload2
        //==========================
        static jint MakeObject(int val) {
            jint toRtn = val;
            return(toRtn);
        }

        //==========================
        // MakeObject() - overload3
        //==========================
        static void MakeObject(const std::string& str, jstring& j, const JavaGUI& gui) {
            j = str.empty() ? 0 : gui.env_->NewStringUTF(str.c_str());
        } 
    };
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=============
// Constructor
//=============
JavaGUI::JavaGUI() : env_(0), jvm_(0), className_("Shell"), 
                     name_("Java GUI class"), testing_(false) {
    JavaVMInitArgs vm_args;
    JavaVMOption options[1];
    options[0].optionString =
        "-Djava.class.path=" USER_CLASSPATH;
    vm_args.version = 0x00010002;
    vm_args.options = options;
    vm_args.nOptions = 1;
    vm_args.ignoreUnrecognized = JNI_TRUE;         

    jint r = JNI_CreateJavaVM(&jvm_, (void**)&env_, &vm_args);
    Assert<JavaException>(r >= 0, name_);
    Assert<JavaException>(env_ != 0, name_);
    Assert<JavaException>(!env_->ExceptionOccurred(), name_);        
    cls_ = env_->FindClass(className_.c_str());
    Assert<JavaException>(cls_ != 0, name_);
}

//============
// Destructor
//============
JavaGUI::~JavaGUI() {
    try {
        if ( jvm_ )
            jvm_->DestroyJavaVM();
    } catch(...) { /* can't do much */ }
}

//=================
// AtTemperature()
//=================
void JavaGUI::AtTemperature(Temperature temp) {
    static const char* funcName = "goToTemp";
    static const char* argType  = JavaCodes::MakeString().c_str();
    static const char* rtnType  = JavaCodes::MakeVoid().c_str();
    static const std::string fs = JavaCodes::MakeCallable(argType, rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool first           = true;

    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }

    std::string toPass = "Ready";
    toPass += GraphicsInterface::Delimiter;
    switch(temp) {
        case ROOM: toPass += "Room"; break;
        case COLD: toPass += "Cold"; break;
        case HOT:  toPass += "Hot";  break;
        default:   throw(BadArg(name_));    
    };    

    jstring j;
    DontUse::JavaTypes::MakeObject(toPass, j, *this);
    try {     
        env_->CallStaticObjectMethod(cls_, mid, j);        
    } catch(...) {
        env_->ReleaseStringUTFChars(j, 0);
        throw;
    }
    env_->ReleaseStringUTFChars(j, 0);    
}

//=========
// Close()
//=========
void JavaGUI::Close() {
    static const char* funcName = "closeMe";
    static const char* rtnType  = JavaCodes::MakeVoid().c_str();
    static const std::string fs = JavaCodes::MakeCallable(rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    Assert<JavaException>(mid != 0, name_);
    env_->CallStaticVoidMethod(cls_, mid);
}

//============================
// DisplayDialogInteractive()
//============================
void JavaGUI::DisplayDialogInteractive(const std::string& info) {
    if ( info.empty() ) // nothing to say?
        return;       
    static const char* funcName = "showDialogMidTest";
    static const char* argType  = JavaCodes::MakeString().c_str();
    static const char* rtnType  = JavaCodes::MakeVoid().c_str();
    static const std::string fs = JavaCodes::MakeCallable(argType, rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool first           = true;
    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }

    jstring j;
    DontUse::JavaTypes::MakeObject(info, j, *this);
    try {     
        env_->CallStaticObjectMethod(cls_, mid, j);        
    } catch(...) {
        env_->ReleaseStringUTFChars(j, 0);
        throw;
    }
    env_->ReleaseStringUTFChars(j, 0);
}

//========================
// DisplayDialogMessage()
//========================
void JavaGUI::DisplayDialogMessage(const std::string& info) {
    if ( info.empty() ) // nothing to say?
        return;       
    static const char* funcName = "showDialogInfo";
    static const char* argType  = JavaCodes::MakeString().c_str();
    static const char* rtnType  = JavaCodes::MakeVoid().c_str();
    static const std::string fs = JavaCodes::MakeCallable(argType, rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool first           = true;
    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }

    jstring j;
    DontUse::JavaTypes::MakeObject(info, j, *this);
    try {     
        env_->CallStaticObjectMethod(cls_, mid, j);        
    } catch(...) {
        env_->ReleaseStringUTFChars(j, 0);
        throw;
    }
    env_->ReleaseStringUTFChars(j, 0);    
}

//========================
// DisplayDialogWarning()
//========================
void JavaGUI::DisplayDialogWarning(const std::string& warning) {
    if ( warning.empty() ) // nothing to say?
        return;       
    static const char* funcName = "showDialogWarning";
    static const char* argType  = JavaCodes::MakeString().c_str();
    static const char* rtnType  = JavaCodes::MakeVoid().c_str();
    static const std::string fs = JavaCodes::MakeCallable(argType, rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool first           = true;
    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }

    jstring j;
    DontUse::JavaTypes::MakeObject(warning, j, *this);
    try {     
        env_->CallStaticObjectMethod(cls_, mid, j);        
    } catch(...) {
        env_->ReleaseStringUTFChars(j, 0);
        throw;
    }
    env_->ReleaseStringUTFChars(j, 0);    
}

//==================
// DisplayResults()
//==================
void JavaGUI::DisplayResults(bool result, const std::string& info) {
    Assert<BadArg>(! info.empty(), name_);        
    static const char* funcName = "displayInfo";
    static std::string argType1 = JavaCodes::MakeBool();
    static std::string argType2 = JavaCodes::MakeString();
    static std::string argType  = JavaCodes::AddArguments(argType1, argType2).c_str();
    static const char* rtnType  = JavaCodes::MakeVoid().c_str();
    static const std::string fs = JavaCodes::MakeCallable(argType.c_str(), rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool first           = true;
    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }

    jboolean b = result;
    jstring j;
    DontUse::JavaTypes::MakeObject(info, j, *this);
    try {     
        env_->CallStaticObjectMethod(cls_, mid, b, j);        
    } catch(...) {
        env_->ReleaseStringUTFChars(j, 0);
        throw;
    }
    env_->ReleaseStringUTFChars(j, 0);
}

//===============
// GetTestInfo()
//===============
std::string JavaGUI::GetTestInfo() {
    Assert<BadCommand>(testing_, name_);
    static const char* funcName = "testinfo";
    static const char* rtnType  = JavaCodes::MakeString().c_str();
    static const std::string fs = JavaCodes::MakeCallable(rtnType);
    static const char* funcSig  = fs.c_str();    
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool first           = true;    
    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }
    jstring j = static_cast<jstring>(env_->CallStaticObjectMethod(cls_, mid)); 
    try {
        const char* str = env_->GetStringUTFChars(j, 0);
        Assert<JavaException>(str != 0, name_); 
        env_->ReleaseStringUTFChars(j, 0);  
        return(static_cast<std::string>(str));         
    } catch(...) {
        env_->ReleaseStringUTFChars(j, 0);
        throw;
    }
}

//==============
// Initialize()
//==============
void JavaGUI::Initialize(const std::vector<std::string>& stationInfo) {
    Assert<BadArg>(!(stationInfo.empty()), name_);           
    std::vector<std::string>::const_iterator i = stationInfo.begin();
    std::string toPass = *i;
    while ( ++i != stationInfo.end() ) {
        toPass += GraphicsInterface::Delimiter;
        toPass += *i;
    }
    static const char* funcName = "initialize";    
    static const char* argType  = JavaCodes::MakeString().c_str();
    static const char* rtnType  = JavaCodes::MakeVoid().c_str();
    static const std::string fs = JavaCodes::MakeCallable(argType, rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool first           = true;
    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }

    jstring j;
    DontUse::JavaTypes::MakeObject(toPass, j, *this);
    try {     
        env_->CallStaticObjectMethod(cls_, mid, j);
        Assert<JavaException>(!IsError(), name_);       
    } catch(JavaException&) {
        env_->ReleaseStringUTFChars(j, 0);
        throw(JavaException(name_ + std::string(": ") + WhatError()));
    } catch(...) {
        testing_ = false;
        env_->ReleaseStringUTFChars(j, 0);
        throw;
    }
    env_->ReleaseStringUTFChars(j, 0);
    testing_ = false;
}

//==================
// IsDonePrinting()
//==================
bool JavaGUI::IsDonePrinting() {
    static const char* funcName = "isDonePrinting";
    static const char* rtnType  = JavaCodes::MakeBool().c_str();
    static const std::string fs = JavaCodes::MakeCallable(rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool  first          = true;
    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }
    return(env_->CallStaticBooleanMethod(cls_, mid) != 0);    
}

//===========
// IsError()
//===========
bool JavaGUI::IsError() {
    static const char* funcName = "isError";
    static const char* rtnType  = JavaCodes::MakeBool().c_str();
    static const std::string fs = JavaCodes::MakeCallable(rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool  first          = true;
    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }
    return(env_->CallStaticBooleanMethod(cls_, mid) != 0);
}

//=========
// Print()
//=========
void JavaGUI::Print(const std::string& info) {
    if ( info.empty() ) // nothing to print?
        return;
    static const char* funcName = "printResults";
    static const char* argType  = JavaCodes::MakeString().c_str();
    static const char* rtnType  = JavaCodes::MakeVoid().c_str();
    static const std::string fs = JavaCodes::MakeCallable(argType, rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool first           = true;
    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }

    jstring j;
    DontUse::JavaTypes::MakeObject(info, j, *this);
    try {     
        env_->CallStaticObjectMethod(cls_, mid, j);        
    } catch(...) {
        env_->ReleaseStringUTFChars(j, 0);
        throw;
    }
    env_->ReleaseStringUTFChars(j, 0);
}

//========================
// RampingToTemperature()
//========================
void JavaGUI::RampingToTemperature(Temperature temp) {
    static const char* funcName = "goToTemp";
    static const char* argType  = JavaCodes::MakeString().c_str();
    static const char* rtnType  = JavaCodes::MakeVoid().c_str();
    static const std::string fs = JavaCodes::MakeCallable(argType, rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool first           = true;

    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }

    std::string toPass = "Ramp";
    toPass += GraphicsInterface::Delimiter;
    switch(temp) {
        case ROOM: toPass += "Room"; break;
        case COLD: toPass += "Cold"; break;
        case HOT:  toPass += "Hot";  break;
        default:   throw(BadArg(name_));    
    };    

    jstring j;
    DontUse::JavaTypes::MakeObject(toPass, j, *this);
    try {     
        env_->CallStaticObjectMethod(cls_, mid, j);        
    } catch(...) {
        env_->ReleaseStringUTFChars(j, 0);
        throw;
    }
    env_->ReleaseStringUTFChars(j, 0);    
}

//=========
// Reset()
//=========
void JavaGUI::Reset() {
    static const char* funcName = "reset";
    static const char* rtnType  = JavaCodes::MakeVoid().c_str();
    static const std::string fs = JavaCodes::MakeCallable(rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool first           = true;
    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }
    testing_ = false;
    env_->CallStaticVoidMethod(cls_, mid);        
}

//=====================
// SetSequenceResult()
//=====================
void JavaGUI::SetSequenceResult(bool result) {
    static const char* funcName = "setStatus";
    static const char* argType  = JavaCodes::MakeBool().c_str();
    static const char* rtnType  = JavaCodes::MakeVoid().c_str();
    static const std::string fs = JavaCodes::MakeCallable(argType, rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool first           = true;

    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }
    env_->CallStaticVoidMethod(cls_, mid, result);
}

//================
// ShowTestInfo()
//================
void JavaGUI::ShowTestInfo(const std::vector<std::string>& info) {
    Assert<BadArg>(!info.empty(), name_);        
    std::vector<std::string>::const_iterator i = info.begin();
    std::string toPass = *i;
    while ( ++i != info.end() ) {
        toPass += GraphicsInterface::Delimiter;
        toPass += *i;        
    }
    static const char* funcName = "showTestSequence";
    static const char* argType  = JavaCodes::MakeString().c_str();
    static const char* rtnType  = JavaCodes::MakeVoid().c_str();
    static const std::string fs = JavaCodes::MakeCallable(argType, rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool first           = true;
    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }

    jstring j;
    DontUse::JavaTypes::MakeObject(toPass, j, *this);
    try {     
        env_->CallStaticObjectMethod(cls_, mid, j);        
    } catch(...) {
        env_->ReleaseStringUTFChars(j, 0);
        throw;
    }
    env_->ReleaseStringUTFChars(j, 0);
}

//========================
// SoakingAtTemperature()
//========================
void JavaGUI::SoakingAtTemperature(bool isSoaking) {
    static const char* funcName = "goToTemp";
    static const char* argType  = JavaCodes::MakeString().c_str();
    static const char* rtnType  = JavaCodes::MakeVoid().c_str();
    static const std::string fs = JavaCodes::MakeCallable(argType, rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool first           = true;

    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }

    std::string toPass = (isSoaking ? "Soak" : "Done Soak");
    toPass += GraphicsInterface::Delimiter; // not used, but necessary
    toPass += "Room";
    jstring j;
    DontUse::JavaTypes::MakeObject(toPass, j, *this);
    try {     
        env_->CallStaticObjectMethod(cls_, mid, j);        
    } catch(...) {
        env_->ReleaseStringUTFChars(j, 0);
        throw;
    }
    env_->ReleaseStringUTFChars(j, 0);  
}

//====================
// UserRequestAbort()
//====================
bool JavaGUI::UserRequestAbort() {
    static const char* funcName = "didAbort";
    static const char* rtnType  = JavaCodes::MakeBool().c_str();
    static const std::string fs = JavaCodes::MakeCallable(rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool  first          = true;
    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }
    return(env_->CallStaticBooleanMethod(cls_, mid) != 0);
}

//=====================
// UserRequestClosed()
//=====================
bool JavaGUI::UserRequestClosed() {
    static const char* funcName = "didClose";
    static const char* rtnType  = JavaCodes::MakeBool().c_str();
    static const std::string fs = JavaCodes::MakeCallable(rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool first           = true;
    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }
    return(env_->CallStaticBooleanMethod(cls_, mid) != 0);
}

//======================
// UserRequestTesting()
//======================
bool JavaGUI::UserRequestTesting() {
    static const char* funcName = "pressedTestButton";
    static const char* rtnType  = JavaCodes::MakeBool().c_str();
    static const std::string fs = JavaCodes::MakeCallable(rtnType);
    static const char* funcSig  = fs.c_str();
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool first           = true;
    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }
    testing_ = (env_->CallStaticBooleanMethod(cls_, mid) != 0);       
    return(testing_);
}

//=============
// WhatError()
//=============
std::string JavaGUI::WhatError() {
    static const char* funcName = "whatError";
    static const char* rtnType  = JavaCodes::MakeString().c_str();
    static const std::string fs = JavaCodes::MakeCallable(rtnType);
    static const char* funcSig  = fs.c_str();    
    static const jmethodID mid  = env_->GetStaticMethodID(cls_, funcName, funcSig);
    static bool first           = true;    
    if ( first ) {
        Assert<JavaException>(mid != 0, name_);
        first = false;
    }
    jstring j = static_cast<jstring>(env_->CallStaticObjectMethod(cls_, mid));
    try {
        const char* str = env_->GetStringUTFChars(j, 0);
        Assert<JavaException>(str != 0, name_);
        env_->ReleaseStringUTFChars(j, 0);   
        return(static_cast<std::string>(str));
    } catch(...) {
        env_->ReleaseStringUTFChars(j, 0);
        throw;
    }
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
