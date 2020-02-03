// Macro Guard
#ifndef SPTS_SCALE_UNITS_H
#define SPTS_SCALE_UNITS_H

// Files included
#include "Assertion.h"
#include "ProgramTypes.h"
#include "SPTSException.h"
#include <string>

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/*
   ==============
   05/05/05, sjn,
   ==============
   Added GetUnits() Overload2.  Take Units argument and convert to std::string.
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

template <typename NumberType>
struct ScaleUnits {

    //====================
    // Public Enumeration
    //====================
    enum Units { 
                 ns,
                 us,
                 ms, mV, mVpk, mVpp, 
                 mA, mApk, mApp, mOhm, 
                 V, A, s, Ohm, Percent, W, oC, NONE, Vpk, Vpp,
                 kHz, kOhm,
                 MHz, MOhm, Cycles
               };

    //======================
    // GetUnits() Overload1
    //======================
    static std::pair<NumberType, Units> GetUnits(const std::string& s) {
        static const std::string name = "ScaleUnits::GetUnits()";
        Assert<BadArg>(! s.empty(), name);
        std::string numbers("-0123456789.");
        std::size_t loc = s.find_first_not_of(numbers);
        std::string units, tmp;
        NumberType number;
        if ( loc != std::string::npos ) {
            units = RemoveAllWhiteSpace(Uppercase(s.substr(loc)));
            tmp   = s.substr(0, loc);
            Assert<BadArg>(IsFloating(tmp), name); 
            number = convert<NumberType>(tmp);                       
        }
        else
            return(std::make_pair(s, ScaleUnits::NONE));
        
        ScaleUnits::Units u;
        if ( units == "NS"          ) u = ScaleUnits::ns;
        else if ( units == "US"     ) u = ScaleUnits::us;
        else if ( units == "MV"     ) u = ScaleUnits::mV;
        else if ( units == "MVPK"   ) u = ScaleUnits::mVpk;
        else if ( units == "MVPP"   ) u = ScaleUnits::mVpp;
        else if ( units == "MS"     ) u = ScaleUnits::ms;
        else if ( units == "MA"     ) u = ScaleUnits::mA;
        else if ( units == "MAPK"   ) u = ScaleUnits::mApk;
        else if ( units == "MAPP"   ) u = ScaleUnits::mApp;
        else if ( units == "MOHM"   ) u = ScaleUnits::mOhm;
        else if ( units == "V"      ) u = ScaleUnits::V;
        else if ( units == "S"      ) u = ScaleUnits::s;
        else if ( units == "A"      ) u = ScaleUnits::A;
        else if ( units == "OHM"    ) u = ScaleUnits::Ohm;
        else if ( units == "%"      ) u = ScaleUnits::Percent;
        else if ( units == "W"      ) u = ScaleUnits::W;
        else if ( units == "OC"     ) u = ScaleUnits::oC;
        else if ( units == "VPK"    ) u = ScaleUnits::Vpk;
        else if ( units == "VPP"    ) u = ScaleUnits::Vpp;
        else if ( units == "KHZ"    ) u = ScaleUnits::kHz;
        else if ( units == "KOHM"   ) u = ScaleUnits::kOhm;
        else if ( units == "MHZ"    ) u = ScaleUnits::MHz;
        else if ( units == "MOHM"   ) u = ScaleUnits::MOhm;
		else if ( units == "CYCLES" ) u = ScaleUnits::Cycles;
        else throw(BadArg("ScaleUnits::GetUnits()"));

        return(std::make_pair(number, u));
    }

    //======================
    // GetUnits() Overload2
    //======================
    static std::string GetUnits(Units units) {
        switch(units) {
            case ScaleUnits::ns:      return("ns");
            case ScaleUnits::us:      return("us");
            case ScaleUnits::mV:      return("mv");
            case ScaleUnits::mVpk:    return("mVpk");
            case ScaleUnits::mVpp:    return("mVpp");
            case ScaleUnits::mA:      return("mA");
            case ScaleUnits::mApk:    return("mApk");
            case ScaleUnits::mApp:    return("mApp");
            case ScaleUnits::mOhm:    return("mOhm");
            case ScaleUnits::ms:      return("ms");
            case ScaleUnits::V:       return("V");
            case ScaleUnits::A:       return("A");
            case ScaleUnits::Ohm:     return("Ohm");
            case ScaleUnits::s:       return("s");
            case ScaleUnits::Percent: return("%");
            case ScaleUnits::W:       return("W");
            case ScaleUnits::oC:      return("oC");
            case ScaleUnits::Vpk:     return("Vpk");
            case ScaleUnits::Vpp:     return("Vpp");
            case ScaleUnits::kHz:     return("kHz");
            case ScaleUnits::kOhm:    return("kOhm");
            case ScaleUnits::MHz:     return("MHz");
            case ScaleUnits::MOhm:    return("MOhm");
			case ScaleUnits::Cycles:  return("Cycles");
            default:
                throw(BadArg("ScaleUnits::GetUnits()"));
        }; // switch
    }

    //===========
    // IsUnits()
    //===========
    static bool IsUnits(const std::string& units) {
        if ( units.empty() ) 
            return(false);

        std::string tmp = "1" + units;
        try {
            GetUnits(tmp);
        } catch(BadArg b) { 
            return(false);
        }
        return(true);
    }

    //=============
    // MakeUnits()
    //=============
    static Units MakeUnits(const std::string& units) {
        if ( units.empty() ) 
            throw(BadArg("ScaleUnits::MakeUnits()"));

        std::string tmp = "1" + units;
        std::pair<NumberType, ScaleUnits::Units> p = GetUnits(tmp);
        return(p.second);        
    }

    //=============
    // ScaleDown()
    //=============
    static NumberType ScaleDown(const NumberType& number, Units units) {
        return(number / scaled(units));
    }

    //===========
    // ScaleUp()
    //===========
    static NumberType ScaleUp(const NumberType& number, Units units) {
        return(number * scaled(units));
    }

private:
    typedef StationExceptionTypes::BadArg BadArg;

    static NumberType scaled(Units units) {
        switch(units) {
            case ScaleUnits::ns:
                return(1E-9);
            case ScaleUnits::us:
                return(1E-6);
            case ScaleUnits::mV:   case ScaleUnits::mVpk: 
            case ScaleUnits::mVpp: case ScaleUnits::mA: 
            case ScaleUnits::mApk: case ScaleUnits::mApp: 
            case ScaleUnits::mOhm: case ScaleUnits::ms:
                return(1E-3);
            case ScaleUnits::V:  case ScaleUnits::A:       case ScaleUnits::Ohm:
            case ScaleUnits::s:  case ScaleUnits::Percent: case ScaleUnits::W:
            case ScaleUnits::oC: case ScaleUnits::NONE:    case ScaleUnits::Vpk:
			case ScaleUnits::Vpp: case ScaleUnits::Cycles:
                return(1);
            case ScaleUnits::kHz: case ScaleUnits::kOhm:
                return(1E3);
            case ScaleUnits::MHz: case ScaleUnits::MOhm:
                return(1E6);
            default:
                throw(BadArg("ScaleUnits::scaled()"));
        }; // switch
    }
};

#endif // SPTS_SCALE_UNITS_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
