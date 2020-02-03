// Macro Guard
#ifndef SPTS_EXCEPTIONS_H
#define SPTS_EXCEPTIONS_H

// Files included
#include "StandardFiles.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes (in Main) <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   05/01/05, sjn,
   ==============
     Added DUTCriticalBase to SPTSExceptions namespace.
     Added BadInhibit structure to DUT Exception section.  Removed BadInhibit typedef.

*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes (in Main) <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

// Don't use these directly 
namespace SPTSExceptions {
    struct DUTBase;
    struct DUTCriticalBase;
    struct MajorStationBase;
    struct MinorStationBase;
    struct UserInputBase;
} 

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace ExceptionTypes {
    template <int ErrorNumber, typename BaseType, typename TagType>
    struct SomeException;

    static const int NoErrorNumber = 0;
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

// Don't use these directly 
namespace ExceptionTags { 

    namespace StationExceptionTags {  
        struct BadArgTag;  
        struct BadClockTag;
        struct BadCommandTag;
        struct BadIinShuntValueTag;
        struct BadRtnValueTag;
        struct BlownFuseTag;
        struct BusErrorTag;
        struct ContainerStateTag;
        struct DeltaFileTag;
        struct DivideByZeroTag;
        struct FileErrorTag;
        struct FileFormatTag;
        struct InfiniteLoopTag;
        struct InfiniteRecursionTag;
        struct InhibitCurrentHighTag;
        struct InstrumentErrorTag;
        struct InstrumentSetupTag;
        struct InstrumentTimeoutTag;
        struct IRResetErrorTag;
        struct MeasurementErrorTag;
        struct MeasurementTimeoutTag;
        struct NoArchiveTag;
        struct NoFileInfoTag;
        struct NoFileTag;       
        struct OutOfRangeTag;
        struct ProgramErrorTag;
        struct RescaleErrorTag;
        struct ScopeMeasureTag;
        struct ScopeSetTag;
        struct TemperatureOverflowTag;
        struct TemperatureTimeoutTag;
        struct UndefinedTag;
        struct UnexpectedStateTag;
        struct UnknownTag;
        struct UnsafeConditionTag;
        struct VinToleranceTag;
    } // namespace ExceptionTags::StationExceptionTags

    namespace MinorExceptionTags {
        struct LocalArchiveTag;
        struct NoLimitsFoundTag;
        struct NoVariablesFoundTag;
        struct OrientationTag;
        struct WrongFixtureTag;
        struct WrongRevisionTag;
    } // namespace ExceptionTags::MinorExceptionTags

    namespace DUTExceptionTags {
        struct BadInhibitTag;
        struct HighIinTag;
        struct JumperPullTag;
        struct NoTripPointTag;
        struct Overshoot2BigTag;
        struct SevereOscillationTag;
        struct TestAbortedTag;
        struct UndershootTag;
        struct VoutAtFreqTag;
    } // namespace ExceptionTags::DUTExceptionTags

    namespace UserInterfaceTags {
        struct NoScreenTag;
        struct InterfaceErrorTag;
    } // namespace ExceptionTags::UserInterfaceTags

} // namespace ExceptionTags


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace DUTExceptionTypes {
    using ExceptionTypes::SomeException;
    using namespace ExceptionTags::DUTExceptionTags;

    namespace SE = SPTSExceptions;

    typedef SE::DUTBase BaseException;
    typedef SE::DUTCriticalBase BaseCriticalException;
    typedef BaseCriticalException IsCritical;
    
    template <int Percentage>
    class BadVout {
        template <int Value>
        struct BadVoutTag {
            static std::string Name() {
                return("Bad Vout: " +
                       convert<std::string>(Value) +
                       std::string("% Load")
                      );
            }
        };
    public:
        typedef SomeException< 1, SE::DUTBase, BadVoutTag<Percentage> > BadVoutType;
    }; // BadVout

    template <typename TestType>
    class VoutBadBeforeTest {
        template <typename T>
        struct VoutBadTag {
            static std::string Name() {
                return("Bad Pre-Vout: " + TestType::Name());
            }
        };
    public:
        typedef SomeException< 2, SE::DUTBase, VoutBadTag<TestType> > PreVoutBad;
    }; // VoutBadBeforeTest

    template <int Percentage>
    class BadInhibit {
        template <int Value>
        struct BadInhibitTag {
            static std::string Name() {
                return("Bad Inh Vout: " +
                       convert<std::string>(Value) +
                       std::string("% Load")
                      );
            }
        };
    public:
        typedef SomeException< 3, IsCritical, BadInhibitTag<Percentage> >
                                                                  BadInhibitType;
    }; // BadVout

    typedef SomeException<4,  IsCritical,  HighIinTag>           HighIin;
    typedef SomeException<5,  SE::DUTBase, JumperPullTag>        JumperPull;
    typedef SomeException<6,  IsCritical,  NoTripPointTag>       NoTripPoint;
    typedef SomeException<7,  SE::DUTBase, Overshoot2BigTag>     Overshoot2Big;
    typedef SomeException<8,  SE::DUTBase, SevereOscillationTag> SevereOscillation;
    typedef SomeException<9,  SE::DUTBase, TestAbortedTag>       TestAborted;
    typedef SomeException<10, SE::DUTBase, UndershootTag>        Undershoot;
    typedef SomeException<11, SE::DUTBase, VoutAtFreqTag>        VoutAtFreq;
} // namespace DUTExceptionTypes

/***************************************************************************************/

namespace MinorExceptionTypes {
    using ExceptionTypes::SomeException;
    using namespace ExceptionTags::MinorExceptionTags;

    namespace SE = SPTSExceptions;

    typedef SE::MinorStationBase BaseException;    
    
    typedef SomeException<1, SE::MinorStationBase, LocalArchiveTag>     LocalArchive;
    typedef SomeException<2, SE::MinorStationBase, NoLimitsFoundTag>    NoLimitsFound;;
    typedef SomeException<3, SE::MinorStationBase, NoVariablesFoundTag> NoVariablesFound;
    typedef SomeException<4, SE::MinorStationBase, OrientationTag>      Orientation;
    typedef SomeException<5, SE::MinorStationBase, WrongFixtureTag>     WrongFixture;
    typedef SomeException<6, SE::MinorStationBase, WrongRevisionTag>    WrongRevision;
} // namespace MinorExceptionTypes

/***************************************************************************************/

namespace StationExceptionTypes {
    using ExceptionTypes::SomeException;
    using namespace ExceptionTags::StationExceptionTags;

    namespace SE = SPTSExceptions;

    typedef SE::MajorStationBase BaseException;

    typedef BadIinShuntValueTag BISVT;
    typedef InfiniteRecursionTag IRT;
    typedef InhibitCurrentHighTag ICHT;
    typedef InstrumentTimeoutTag ITT;
    typedef MeasurementErrorTag MET;
    typedef MeasurementTimeoutTag MTO;
    typedef TemperatureOverflowTag TOT;
    typedef TemperatureTimeoutTag TTT;

    typedef SomeException<1,  SE::MajorStationBase,  BadArgTag>                   BadArg;
    typedef SomeException<2,  SE::MajorStationBase,  BadCommandTag>           BadCommand;
    typedef SomeException<3,  SE::MajorStationBase,  BISVT>             BadIinShuntValue;
    typedef SomeException<4,  SE::MajorStationBase,  BadRtnValueTag>         BadRtnValue;
    typedef SomeException<5,  SE::MajorStationBase,  BadClockTag>         BadSystemClock;
    typedef SomeException<6,  SE::MajorStationBase,  BlownFuseTag>             BlownFuse;
    typedef SomeException<7,  SE::MajorStationBase,  BusErrorTag>               BusError;
    typedef SomeException<8,  SE::MajorStationBase,  ContainerStateTag>   ContainerState;
    typedef SomeException<9,  SE::MajorStationBase,  DeltaFileTag>             DeltaFile;
    typedef SomeException<10, SE::MajorStationBase,  DivideByZeroTag>       DivideByZero;
    typedef SomeException<11, SE::MajorStationBase,  FileErrorTag>             FileError;
    typedef SomeException<12, SE::MajorStationBase,  FileFormatTag>      FileFormatError;
    typedef SomeException<13, SE::MajorStationBase,  InfiniteLoopTag>       InfiniteLoop;
    typedef SomeException<14, SE::MajorStationBase,  IRT>              InfiniteRecursion;
    typedef SomeException<15, SE::MajorStationBase,  ICHT>            InhibitCurrentHigh;
    typedef SomeException<16, SE::MajorStationBase,  InstrumentErrorTag> InstrumentError;
    typedef SomeException<17, SE::MajorStationBase,  InstrumentSetupTag> InstrumentSetup;
    typedef SomeException<18, SE::MajorStationBase,  ITT>              InstrumentTimeout;
    typedef SomeException<19, SE::MajorStationBase,  MET>               MeasurementError;
    typedef SomeException<20, SE::MajorStationBase,  MTO>             MeasurementTimeout;
    typedef SomeException<21, SE::MajorStationBase,  NoArchiveTag>             NoArchive;
    typedef SomeException<22, SE::MajorStationBase,  NoFileInfoTag>           NoFileInfo;
    typedef SomeException<23, SE::MajorStationBase,  NoFileTag>              NoFileFound;
    typedef SomeException<24, SE::MajorStationBase,  OutOfRangeTag>           OutOfRange;
    typedef SomeException<25, SE::MajorStationBase,  ProgramErrorTag>       ProgramError;
    typedef SomeException<26, SE::MajorStationBase,  RescaleErrorTag>       RescaleError;
    typedef SomeException<27, SE::MajorStationBase,  IRResetErrorTag>         ResetError;
    typedef SomeException<28, SE::MajorStationBase,  ScopeMeasureTag>       ScopeMeasure;
    typedef SomeException<29, SE::MajorStationBase,  ScopeSetTag>               ScopeSet;
    typedef SomeException<30, SE::MajorStationBase,  TOT>            TemperatureOverflow;
    typedef SomeException<31, SE::MajorStationBase,  TTT>             TemperatureTimeout;
    typedef SomeException<32, SE::MajorStationBase,  UndefinedTag>             Undefined;
    typedef SomeException<33, SE::MajorStationBase,  UnexpectedStateTag> UnexpectedState;
    typedef SomeException<34, SE::MajorStationBase,  UnknownTag>        UnknownException;
    typedef SomeException<35, SE::MajorStationBase,  VinToleranceTag>       VinTolerance;

    // special typedefs
    typedef SomeException<100, SE::MajorStationBase, UnsafeConditionTag> UnsafeCondition;
} // namespace StationExceptionTypes

/***************************************************************************************/

namespace UserInputExceptionTypes {
    using ExceptionTypes::SomeException;    
    using namespace ExceptionTags::UserInterfaceTags;

    namespace SE = SPTSExceptions;

    typedef SE::UserInputBase BaseException;

    typedef SomeException<1, SE::UserInputBase, NoScreenTag>       NoScreen;
    typedef SomeException<2, SE::UserInputBase, InterfaceErrorTag> UserInterfaceError;
} // namespace UserInputExceptionTypes

/***************************************************************************************/

#include "SPTSException.template" // Microsoft 7.0 compiler workaround

#endif // SPTS_EXCEPTIONS_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
