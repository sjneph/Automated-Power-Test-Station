// Macro Guard
#ifndef SPTS_FACTORY_H
#define SPTS_FACTORY_H

// Files included
#include "Assertion.h"
#include "SPTSException.h"
#include "StandardFiles.h"

//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============
   12/30/04, sjn,
   ==============
     Added GetAllRegistered()
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//



//=============================================================
// Factory design idea "borrowed" from 'Modern C++ Design'
// by Andrei Alexandrescu and modified slightly from his 
// implementation in the Loki Library.
//=============================================================


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


template 
<
    typename AbstractProduct,
    typename IdentifierType,
    typename ProductCreator = AbstractProduct* (*)()
>
struct Factory {
    std::set<IdentifierType> GetAllRegistered() const {
        std::set<IdentifierType> toRtn;
        AssocMap::const_iterator i = associations_.begin(), j = associations_.end();
        while ( i != j ) {
            toRtn.insert(i->first);
            ++i;
        } // while
        return(toRtn);
    }

    bool IsRegistered(const IdentifierType& id) {
        return(associations_.find(id) != associations_.end());
    }

    bool Register(const IdentifierType& id, ProductCreator creator) {
        return(associations_.insert(typename AssocMap::value_type(id, creator)).second);
    }

    bool Unregister(const IdentifierType& id) {
        return(associations_.erase(id) == 1);
    }

    AbstractProduct* CreateObject(const IdentifierType& id) {
        typename AssocMap::const_iterator i = associations_.find(id);
        Assert<BadArg>(i != associations_.end(), "Factory Class");
        return((i->second)());  // Create an instance and return
    }
private:
    typedef StationExceptionTypes::BadArg BadArg;
    typedef std::map<IdentifierType, ProductCreator> AssocMap;
    AssocMap associations_;
};

#endif // SPTS_FACTORY_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
