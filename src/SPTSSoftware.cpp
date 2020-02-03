// Files included
#include "SPTSSoftware.h"



/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/***************************************************************************************/
// PROJECT NOTES (Project/File dependencies):
//  08/25/03, sjn,
//  (1)  When starting a new project in MS VC++ 7.x and using the Java-based GUI:
//       (a)  Add jvm.lib to the project - currently found in C:\j2sdk1.4.2\lib\
//       (b)  Under Project->Properties, choose Debugging under Configuration 
//              Properties.  Add to 'working directory' the path to jvm.dll.
//              This is currently at C:\j2sdk1.4.2\jre\bin\client\
//       This is necessary when using the Java Native Interface.  This is currently
//          the case since we are using a Java-built GUI
//
//  (2)  All java class files used for the Java GUI must exist in the same directory 
//          as the jvm.dll file listed in #1 above.
//
//  (3)  In regards to the Java GUI, the final executable still depends on jvm.dll listed 
//          in #1 above.  Therefore, the final executable must be placed in the same
//          directory as that for jvm.dll.  A shortcut to this executable should be
//          placed on the desktop.
//
//  (4)  Again in regards to the Java GUI, two C++ header files developed by the Sun
//          company are used when building the project.  One is jni.h.  The other,
//          for MS Windows only, is jni_md.h.  These are currently found in 
//          C:\j2sdk1.4.2\include and C:\j2sdk1.4.2\include\win32 respectively.  If 
//          using MS VC++ 7.x, the easiest way to include these is simply to add
//          them to the project.
//
//  (5)  When using National Instruments GPIB communication, the following object file
//          must be linked into the project:  Gpib-32.obj
//          This object file may change during future releases of NI.  If using MS
//          VC++ 7.0, the easiest way to include this is simply add the file to the 
//          project.
//
//  (6)  When using the aardvark USB-2-I2C adapter, the aardvark.dll must go in the
//          same directory as the jvm.dll (see (1) above).
//
//  (7)  No other external file dependencies currently exist for this software package.  
//          Everything else was developed at Interpoint and is standard C++.  You must
//          include all of these C++ files in your project to compile/link.
//
//  * Runtime support text files */
//  (1)  See template <> struct SPTSFiles<PointerFileTag> in SPTSFiles.cpp and the
//          ErrorLogger class in ErrorLogger.h for two hardcoded file dependencies.
//          The former of these may be able to be non-hardcoded when SPTS configuration
//          files are moved out to a network location (if moved to the current working
//          directory with certain tradeoffs).  The one associated with ErrorLogger.h
//          should remain on the local drive in my opinion - see ErrorLogger.h for more
//          details.
/***************************************************************************************/

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

// Static definitions
const std::string Software::name_     = "50000-601";
const std::string Software::revision_ = "R.01";

/***************************************************************************************/
//===================================== Comments ======================================//
/***************************************************************************************/
/*
Revision T.01, 02/15/12, MRB
	Created Agilent6034ALanguage.h

Revision R.01, 01/13/10, REB
	Added additional percentages in TestStepINfo.cpp
Revision P.01, 10/15/09, REB, MRB
	Added function to choose which method would be used for measuring Vout.
Revision N.03, 12/15/08, REB
	Added new Cross Regulation Test (preliminary)
Revision N.02, 12/15/08, REB
	Added additional percentages in TestStepINfo.cpp
Revision N.02, 11/12/08, REB
	Added Symbolic values to load field for percentages in TestStepInfo.cpp

Revision N.01, 10/7/08, MBuck
	Revised parser in SPTSFiles.cpp (GetLimits, GetVariables, GetTable) to look at the 
	whole part number or various parts of it.  (Include qualifying LIM/VAR files with 
	second dash.
	Revised VariableFile.cpp (Reload) to send entire part number; not just base part number.

Revision M.02, 04/02/08, MRB
	Revised Load transient test to slowly zoom in to get best reading.

Revision M.01, 10/2/07, MRB
	Added third attempt to read Vout Load Trans. Test.

Revision L.01, 8/10/07, MRB
	Added Inhibit Cycle Test.

Revision K.03, 8/02/07, MBuck
	Changed algorithm for radiation handling to be more in line with how system normally
	parses dash numbers
	Also added parsing for Trim Table

Revision K.02, 6/19/07, MRB
	Didn't take into account the Limits file parsing

Revision K.01, 6/12/07, MBuck
	Added ability to handle different radiation level specifications in part files.
	SPTSFiles<VariablesFileTag>::GetVariables() in SPTSFiles.cpp.

Revision J.01, 05/02/07,	MRB,
	Combined two versions of H.01 that had been used on stations.

Revision H.01, 08/11/06,	MRB, HQP
	Changed RampVin to accept wider range of limits for Input Current

Revision G.03, 08/04/06,	MRB
	Added the Capability of overriding the Frequency measurements to strictly measure off of the outputs.

Revision G.02, 06/30/06,	MRB
	Changed the Error Handling functions to allow for any Dash and Workorder Number to be put in.

Revision G.01, 05/15/06,	MRB
	Prepared to modify the System to choose which method would be used for measuring Iout.
	Modified VariablesFile.cpp, VariablesFile.h, Converter.cpp, Converter.h, and StationAlgorithms.cpp

Revision F.01, 03/24/06 , FAC, HQP
    Modified TestSequence.cpp, MeasurementFunctions.cpp, and SPTS.cpp
	
Revision E.03, 12/01/05 - 12/02/05, sjn,
    Modified StationAlgorithms.cpp, TestSequence.cpp.

Revision E.02, 11/14/05 - 11/20/05, sjn,
    Modified SPTS.h, SPTS.cpp, StationAlgorithms.cpp, StationAlgorithms.h,
    VariablesFile.h, VariablesFile.cpp, MainSupply.h, MainSupply.cpp, Instrument.h,
    Instrument.cpp, PauseStates.h, PauseStates.cpp, TestFixtureFile.cpp,
    TestFixtureFile.h, TestSequence.h, TestSequence.cpp, DataArchive.cpp,
    TestStepFacade.h, DataArchive.cpp, OperatorInterface.h, OperatorInterface.cpp,
    JavaGUI.h, JavaGUI.cpp, StationFile.h, StationFile.cpp and Main.cpp.

    Added TestStepDiagnostic.h

Revision E.01, 06/23/05 - 10/26/05 sjn,
    Modified TestStepInfo.cpp, Measurement.cpp, FunctionGenerator.cpp, SPTS.cpp,
    SPTS.h, FunctionGeneratorTraits.h, SCPI_FunctionGenerator.h, FunctionGenerator.h,
    HP33120A.h, InstrumentFile.h, InstrumentFile.cpp, StationFile.cpp, StationFile.h,
    InstrumentFile.cpp, StationAlgorithms.cpp, MainSupply.cpp, MainSupply.h and
    MeasurementFunctions.cpp.

    Added FunctionGeneratorInterface.h, Agilent33220A.h, Agilent33220A.cpp and
    HP33120A.cpp.
    Added AgilentN5772A.h and Agilent5772A.cpp.

Revision D.01, 03/02/05 - 05/23/05 sjn,
    Modified Project Notes (above):

    Modified CurrentProbe.h, CurrentProbe.cpp, CurrentProbeTraits.h, InstrumentFile.h,
    InstrumentFile.cpp, InstrumentTypes.h, SPTS.cpp, Agilent3499AExternalRelays.h,
    Agilent3499AInternalRelays.h, MeasurementFunctions.cpp, GPIB.h, GPIB.cpp, 
    Instrument.h, Instrument.template, StationAlgorithms.cpp, StationFiles.h,
    StationFiles.cpp, TestSequence.h, TestSequence.cpp, Main.cpp, SPTSFiles.cpp,
    ErrorLogger.h, Measurement.h, Measurement.cpp, MeasurementFunctions.h,
    SPTSException.h, SPTSException.template, StationAlgorithms.cpp,
    StationAlgorithms.h, StationAlgorithms.template, ScaleUnits.h,
    OperatorInterface.cpp, SupplyInterface.h, MainSupply.cpp, SCPI.h, ControlMatrix.cpp,
    SPCI_PowerSupply.h, SwitchMatrix.cpp, ElectronicLoad.cpp, DMM.cpp,
    FunctionGenerator.cpp, OscopeInterface.h, Oscilloscope.cpp, Agilent54624ALanguage.h,
    LecroyLT224Language.h and SPTS.h.

    Added aardvark.h, aardvark.c.  Added aardvark.dll dependency (all 3rd party).
    Added I2C.h and I2C.cpp.
    Added TekTCPA300.h, TekTCPA300Language.h.
    Added HPE4356A.h and HPE4356A.cpp (additional Agilent supplies)
    Added Agilent6034A.h and Agilent6034A.cpp
    Added TestStepFacade.h
    Removed TektronixTM5003.h and TektronixTM5003Language.h (incompatible with TCPA).
    Removed NoCreation.h.  When compiler warnings are set to highest level, inheriting
      from this class causes issues.  It is not an essential class and was meant to
      keep one from creating classes with nothing more than static members (such as
      tags).  But, it does no harm to create a class of this type.  Many files were
      updated to remove NoCreation as an inherited source.

    With Microsoft Visual C++ 7.1's highest warnings, we receive zero warnings in this
      project other than those created in aardvark.c.  That code is from a 3rd party
      vendor.  Any/all code developed at Interpoint should generate no warnings when
      the compiler is set to the highest warning level.

Revision C.01, 03/17/05-03/26/05, sjn,
    Modified StationAlgorithms.cpp, JavaGUI.h, JavaGUI.cpp, GraphicsInterface.h,
             OperatorInterface.h, OperatorInterface.cpp and StationAlgorithms.h

Revision B.02, 02/18/05, sjn,
    Modified SPTS.cpp.

Revision B.01, 07/01/04 - 01/31/05, sjn,
    Replaced all Andrei Alexandrescu sayings with a non-tabbed version (all files).

    Modified Oscilloscope.cpp, LecroyLT224Language.h, LecroyLT224.h, Main.cpp,
             InstrumentFile.cpp, OScopeParameters.h, Oscilloscope.h, OScopeInterface.h,
             CurrentProbe.cpp, SPTS.h, SPTS.cpp, StationFile.cpp, 
             MeasurementFunctions.cpp, VariablesFile.cpp, TestSequence.cpp,
             DataArchive.cpp, LimitsFile.h, LimitsFile.cpp, SPTSFiles.h, SPTSFiles.cpp,
             FileNode.cpp, Functions.h, VariablesFile.h, TemperatureController.h, 
             TemperatureController.cpp, OScopeSetupFile.h, OScopeSetupFile.cpp, 
             Shutdown.cpp, FileNode.h, FileNode.cpp, TestStepInfo.cpp,
             MeasurementFunctions.h, FilterSelects.h, Agilent3499AInternalRelays.h,
             ConfigureRelays.cpp, SCPI_PowerSupply.h, Measurement and AgilentN3300A.h.

    Added Agilent54624A.h, Agilent54624ALanguage.h, Agilent54624A.cpp, 
          AgilentN3300ALanguage and CustomTestHandler.h.

Revision A.01, 11/05/03, sjn,
    Release per ECO# 03-0146.  
    Modified TestSequence.cpp.

Revision PR.01, 08/25/03, sjn,
    Initial Write
*/
/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
