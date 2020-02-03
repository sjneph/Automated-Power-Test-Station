// Macro Guard
#ifndef SPTS_LOAD_N3300A_LANGUAGE
#define SPTS_LOAD_N3300A_LANGUAGE

// Files included
#include "ProgramTypes.h"
#include "SCPI_ElectronicLoad.h"
	

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct AgilentN3300ALanguage : public SCPI<ElectronicLoadTag> {
	static std::string SetTransient(long channel, const SetType& fromVal, 
		                            const SetType& toVal, const std::string& rate) {

            /*
              The N3300A has trouble meeting the calculated slew rate when the
              transition time is < 25us.  If < 25us transition time, then we ask for
              the max slew rate allowable, which actually results in a closer-to-
              specification transition time than if we use the more accurate, 
              calculated slew rate.  Note that this empirically measured result is the
              reason why this struct was created (and inherits from SCPI<ELoad>).
            */
            SetType diff = fromVal - toVal;
            if ( diff < SetType(0) )
                diff *= -1;

            std::string newRate = rate;
            if ( newRate != MaxSlew() ) {
                if ( SetType(newRate) >= SetType(diff / SetType(25e-6)) )
                    newRate = MaxSlew();
            }
            return(SCPI<ElectronicLoadTag>::SetTransient(channel, fromVal,
                                                         toVal, newRate));
		}

protected:
	~AgilentN3300ALanguage() { /* */ }  
};

#endif // SPTS_LOAD_N3300A_LANGUAGE

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
