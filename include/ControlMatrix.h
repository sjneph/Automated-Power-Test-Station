// Macro Guard
#ifndef SPTS_CONTROL_MATRIX_H
#define SPTS_CONTROL_MATRIX_H

// Files Included
#include "ControlMatrixTraits.h"
#include "Instrument.h"
#include "InstrumentTags.h"
#include "NoCopy.h"
#include "StandardFiles.h"
#include "Switch.h"


namespace SPTSInstrument {

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct ControlMatrixInheritance 
                   : protected Instrument<ControlMatrixTraits::ModelType::BusType> {
    typedef ControlMatrixTraits::RelayTypes RelayTypes;
protected:
    typedef ControlMatrixTraits::ModelType MatrixType;
    typedef MatrixType::BusType BT;
    typedef MatrixType::Language Language;
    ~ControlMatrixInheritance() { /* */ }
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


struct ControlMatrix : public ControlMatrixInheritance {

	//==================
    // Public Interface 
    //==================
	bool Initialize(); 
    std::string Name() const;

public:
	struct RLL;
	struct InputRelayControl;
	struct OutputRelayControl;
	struct Misc;
	
private:
	friend struct RLL;
	friend struct InputRelayControl;
	friend struct OutputRelayControl;
	friend struct Misc;

public:
	//==================
    // Start nested RLL 
    //==================
	struct RLL : public ControlMatrixInheritance, private NoCopy {		
		// typedefs
		typedef RelayTypes::ResistiveLoadLines RLLs;
		typedef std::set<RLLs> RLLContainer;
		typedef std::map<RLLs, Switch> RLLMap;

		// Constructor
		explicit RLL(const RLLContainer& container);

		// Public Interface
        bool ChangedState();
		void Concatenate(Switch state);
		Switch CurrentState(RLLs line);
		bool CustomReset();
        void ImmediateMode();
		void Initialize();
		bool IsConcatenate() const;
		bool IsError();
		static std::string Name();
        bool OpsComplete();
		bool Reset();
		void SetCustomReset(const RLLMap& relays);
		bool TurnOff(RLLs line);
		bool TurnOff(const RLLContainer& lines);
		bool TurnOn(RLLs line);
		bool TurnOn(const RLLContainer& lines);
		ControlMatrix* Underlying();
		std::string WhatError() const;

	private:
        bool command();
		bool command(const std::string& str);
		bool stateChange(RLLs line, Switch state);
		bool stateChange(const RLLContainer& lines, Switch state);
		void updateStatus(const RLLMap& relays);
		void updateStatus(RLLs relay, Switch sw);

     private:
		RLLMap knownStates_;
		RLLMap customReset_;
		bool locked_;
		bool concatenated_;
        std::string syntax_;
        std::string name_;
		long address_;
	};
	//================
    // End nested RLL 
    //================

	//================================
    // Start nested InputRelayControl 
    //================================
	struct InputRelayControl : public ControlMatrixInheritance, private NoCopy {
		// typedefs 
		typedef RelayTypes::InputRelay InputRelay;
		typedef std::vector<InputRelay> RelayContainer;
		typedef std::map<InputRelay, Switch> RelayStateMap;

		// Constructor
		explicit InputRelayControl(const RelayContainer& container);

		// Public Interface
        bool ChangedState(const RelayContainer& relays);
		Switch CurrentState(InputRelay line);
		bool CustomReset();
		void Initialize();
		bool IsError();
		static std::string Name();
        bool OpsComplete();
		bool Reset();
		void SetCustomReset(const RelayStateMap& relays);
		bool TurnOff(InputRelay relay);
		bool TurnOff(const RelayContainer& relays);
		bool TurnOn(InputRelay relay);
		bool TurnOn(const RelayContainer& relays);
		ControlMatrix* Underlying();
		std::string WhatError() const;

	private: // helpers
		bool command(const std::string& str);
		bool stateChange(InputRelay line, Switch state);
		bool stateChange(const RelayContainer& relay, Switch state);
		void updateStatus(const RelayStateMap& relays);
		void updateStatus(InputRelay relay, Switch sw);

	private:
        RelayStateMap knownStates_;
		RelayStateMap customReset_;
		bool locked_;
        std::string name_;
		long address_;
	};
	//==============================
    // End nested InputRelayControl 
    //==============================

	//=================================
    // Start nested OutputRelayControl 
    //=================================
	struct OutputRelayControl : public ControlMatrixInheritance, private NoCopy {
		// typedefs 
		typedef RelayTypes::OutputRelay OutputRelay;
		typedef std::vector<OutputRelay> RelayContainer;
		typedef std::map<OutputRelay, Switch> RelayStateMap;

		// Constructor
		explicit OutputRelayControl(const RelayContainer& container);

		// Public Interface
        bool ChangedState();
		Switch CurrentState(OutputRelay line);        
		bool CustomReset();
		void Initialize(); 
		bool IsError();
		static std::string Name();
        bool OpsComplete();
		bool Reset();
		void SetCustomReset(const RelayStateMap& relays);
		bool TurnOff(OutputRelay relay);
		bool TurnOff(const RelayContainer& relays);
		bool TurnOn(OutputRelay relay);
		bool TurnOn(const RelayContainer& relays);
		ControlMatrix* Underlying();
		std::string WhatError() const;

	private: // helpers
		bool command(const std::string& str);
		bool stateChange(OutputRelay line, Switch state);
		bool stateChange(const RelayContainer& relay, Switch state);
		void updateStatus(const RelayStateMap& relays);
		void updateStatus(OutputRelay relay, Switch sw);

	private:
        RelayStateMap knownStates_;
		RelayStateMap customReset_;
		bool locked_;
        std::string name_;
		long address_;
	};
	//===============================
    // End nested OutputRelayControl 
    //===============================

	//===================
    // Start nested Misc 
    //===================
	struct Misc : public ControlMatrixInheritance, private NoCopy {
		// typedefs
		typedef RelayTypes::MiscRelay MiscLine;
		typedef std::vector<MiscLine> MiscContainer;
		typedef std::map<MiscLine, Switch> MiscMap;

		// Constructor
		explicit Misc(const MiscContainer& container);

		// Public Interface
        bool ChangedState();
		Switch CurrentState(MiscLine line);
		bool CustomReset();
        MiscMap GetKnownLines();
		void Initialize();
		bool IsError();
		static std::string Name();
        bool OpsComplete();
		bool Reset();
		void SetCustomReset(const MiscMap& lines);
        std::size_t Size();
		bool TurnOff(MiscLine line);
		bool TurnOff(const MiscContainer& lines);
		bool TurnOn(MiscLine line);
		bool TurnOn(const MiscContainer& lines);
		ControlMatrix* Underlying();
		std::string WhatError() const;

	private:
		bool command(const std::string& str);
		bool stateChange(MiscLine line, Switch state);
		bool stateChange(const MiscContainer& lines, Switch state);
		void updateStatus(const MiscMap& lines);
		void updateStatus(MiscLine line, Switch sw);
		MiscMap knownStates_;
		MiscMap customReset_;
		bool locked_;
        std::string name_;
		long address_;
	};
	//=================
    // End nested Misc 
    //=================

private:
	bool bitprocess(const std::string& eString, Instrument<BT>::Register toCheck);
	bool command(const std::string& str, long address);
	std::string query(const std::string& str, long address);
    void setAddress(long address);

    private:
    std::set<long> addresses_;

private:
	static ControlMatrix& instance() {
        static ControlMatrix toRtn;
        return(toRtn);
    }
    ControlMatrix() { /* */ }
};

} // namespace SPTSInstrument

#endif // SPTS_CONTROL_MATRIX_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
