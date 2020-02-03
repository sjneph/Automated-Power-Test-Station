// Macro Guard
#ifndef SPTS_CUSTOM_TEST_HANDLER_H
#define SPTS_CUSTOM_TEST_HANDLER_H

// Files included
#include "Assertion.h"
#include "StandardFiles.h"
#include "StringAlgorithms.h"

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/
/*
  Purpose is to deal with test types of the form CUSTOM TEST(x) = Arbitrary Test Name,
    for integer x.  fullType_ maintains the entire expression, shortType_ is equal to
    Arbitrary Test Name.  If a non-custom test or if of the form: CUSTOM TEST(x), then
    shortType_ equals fullType_.
*/
/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/
struct CustomTestHandler {
    typedef StationExceptionTypes::BadArg BadArg;

    //========================
    // Start Public Interface
    //========================
    explicit CustomTestHandler(const std::string& fullTestType)
                          : fullType_(fullTestType), shortType_(fullTestType),
                            custName_(""), ct_("CUSTOM TEST") {
        std::string cpy = Uppercase(fullType_);
        if ( cpy.size() >= ct_.size() ) {
            std::pair<std::string::iterator, std::string::iterator> p;
            p = std::mismatch(ct_.begin(), ct_.end(), cpy.begin());
            if ( p.first == ct_.end() ) { // a custom test
                std::string::size_type pos = fullType_.find("=");
                if ( pos != std::string::npos ) {
                    shortType_ = fullType_.substr(pos+1);
                    custName_ = fullType_.substr(0, pos-1);
                    RemoveFrontBackSpace(shortType_);
                }
                else
                    custName_ = fullType_;
                RemoveFrontBackSpace(custName_);
            }
        }        
    }

    std::string FindTemperature(const std::string& val) const {
        if ( Uppercase(val).find(Uppercase(shortType_)) == std::string::npos )
            return(val);
        // If you get this far, then val is of form:
        //    Custom Test(x) = [Arbitrary Test Name] Temperature
        // Example:  Custom Test(1) = [My Sequence Name] 35oC
        std::string::size_type pos1 = val.find('['), pos2 = val.find(']');
        Assert<StationExceptionTypes::FileError>(pos1 != std::string::npos, name());
        Assert<StationExceptionTypes::FileError>(pos2 != std::string::npos, name());
        std::string toRtn = val.substr(pos2+1);
        RemoveFrontBackSpace(toRtn);
        return(toRtn);
    }

    std::string GetArbitraryTestName() const {
        return(shortType_); // defaults to fullType_ if not arbitrary test name
    }

    std::string GetCustomTestName() const {
        return(custName_);
    }

    std::string GetFullTestName() const {
        return(fullType_);
    }

    bool IsArbitraryTestName() const {
        return(fullType_ != shortType_);
    }

    bool IsCustomTest() const {
        return(!custName_.empty());
    }
    //======================
    // End Public Interface
    //======================

private:
    std::string name() const {
        return("CustomTestHandler");
    }

private:
    std::string fullType_, shortType_, custName_, ct_;
};


#endif // SPTS_CUSTOM_TEST_HANDLER_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
