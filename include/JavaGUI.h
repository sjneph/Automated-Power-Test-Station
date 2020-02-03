// Macro Guard
#ifndef SPTS_JAVA_GUI_H
#define SPTS_JAVA_GUI_H

// Files included
#include "Functions.h"
#include "GraphicsInterface.h"
#include "NoCopy.h"
#include "jni.h" // from the Sun team
#include "StandardFiles.h"

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

namespace DontUse {
    struct JavaTypes; // forward declaration
}

struct JavaGUI : public GraphicsInterface, private NoCopy {
    JavaGUI();
    virtual ~JavaGUI();

    //=======================================================
    // Start Public Interface - implements GraphicsInterface
    //=======================================================
    void AtTemperature(Temperature temp);
    void Close();
    void DisplayDialogInteractive(const std::string& info);
    void DisplayDialogMessage(const std::string& info);
    void DisplayResults(bool result, const std::string& info);
    std::string GetTestInfo();
    void Initialize(const std::vector<std::string>& stationInfo);
    bool IsDonePrinting();
    bool IsError();
    void Print(const std::string& toPrint);
    void RampingToTemperature(Temperature temp);
    void Reset();
    void DisplayDialogWarning(const std::string& warning);
    void SetSequenceResult(bool result);
    void ShowTestInfo(const std::vector<std::string>& info);
    void SoakingAtTemperature(bool isSoaking);
    bool UserRequestAbort();
    bool UserRequestClosed();
    bool UserRequestTesting();
    std::string WhatError();
    //======================
    // End Public Interface
    //======================

private:
    friend struct DontUse::JavaTypes;

private:
     JNIEnv *env_;
     JavaVM *jvm_;
     jint res_;
     jclass cls_;     
     jstring jstr_;
     jint intType_;
     jobjectArray args_;
     std::string className_;
     std::string name_;
     bool testing_;
};


#endif // SPTS_JAVA_GUI_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
