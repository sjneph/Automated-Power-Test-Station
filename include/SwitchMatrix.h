// Macro Guard
#ifndef SPTS_SwitchMatrix_H
#define SPTS_SwitchMatrix_H

// Files Included
#include "Instrument.h"
#include "NoCopy.h"
#include "StandardFiles.h"
#include "SwitchMatrixTraits.h"
#include "Switch.h"


namespace SPTSInstrument {

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

class SwitchMatrixInheritance 
      : protected Instrument<SwitchMatrixTraits::ModelType::BusType>, protected NoCopy {
protected:
    typedef SwitchMatrixTraits::RelayTypes RelayTypes;
    typedef SwitchMatrixTraits::ModelType  MatrixType;
    typedef MatrixType::Language Language;
    typedef MatrixType::BusType BT;
    ~SwitchMatrixInheritance() { /* */ }

public:
    typedef RelayTypes::RFRelay           RFRelay;
    typedef RelayTypes::FilterRelay       FilterRelay;
    typedef RelayTypes::DCRelay           DCRelay;
    typedef std::vector<RFRelay>          RFRelayContainer;	 
    typedef std::vector<FilterRelay>      RFFilterContainer;
	typedef std::vector<DCRelay>          DCRelayContainer;
    typedef std::map<RFRelay, Switch>     RFRelayMap;
    typedef std::map<FilterRelay, Switch> FilterRelayMap;
    typedef std::map<DCRelay, Switch>     DCRelayMap;
    enum RelayType { RF, DC, FILTER };        
};

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct SwitchMatrix : public SwitchMatrixInheritance {
    //========================
    // Constructor/Destructor
    //========================
	SwitchMatrix(const RFRelayContainer& containerRF, 
                 const DCRelayContainer& containerDC,
                 const RFFilterContainer& containerFilter);
	~SwitchMatrix();

    //========================
    // Start Public Interface
    //========================
    bool ChangedState(RelayType type);
	bool Close(RFRelay relay);
	bool Close(const RFRelayContainer& relays);
	bool Close(DCRelay relay);
	bool Close(const DCRelayContainer& relays);
    bool Close(FilterRelay relay);
    bool Close(const RFFilterContainer& relays);
    bool CustomReset(RelayType type);
	bool Initialize();
	bool IsError(RelayType r);
    std::string Name();
	bool Open(RFRelay relay);
	bool Open(const RFRelayContainer& relays);
	bool Open(DCRelay relay);
	bool Open(const DCRelayContainer& relays);
    bool Open(FilterRelay relay);
    bool Open(const RFFilterContainer& relays);
    bool OpsComplete(RelayType r);
	bool Reset();
    void SetCustomReset(const RFRelayMap& relays);
    void SetCustomReset(const DCRelayMap& relays);
    void SetCustomReset(const FilterRelayMap& relays);
    std::string WhatError(RelayType r);
    //======================
    // End Public Interface
    //======================

private:
    bool bitprocess(const std::string& eString, Instrument<BT>::Register toCheck);
	bool command(const std::string& cmd, RelayType type);
    std::string nameDC() const;
    std::string nameFilter() const;
    std::string nameRF() const;
	std::string query(const std::string& str, RelayType r);

private:
	long addressRF_;
    long addressDC_;
    long addressFilt_;
    bool locked_;
    std::auto_ptr<RFRelayMap> rf_;
    std::auto_ptr<DCRelayMap> dc_;
    std::auto_ptr<FilterRelayMap> filter_;
    std::auto_ptr<DCRelayMap> customResetDC_;
    std::auto_ptr<RFRelayMap> customResetRF_;
    std::auto_ptr<FilterRelayMap> customResetFilt_;  
};

} // namespace SPTSInstrument 

#endif // SPTS_SwitchMatrix_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
