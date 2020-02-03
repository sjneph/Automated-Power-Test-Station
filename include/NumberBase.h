// Macro Guard
#ifndef SPTS_NUMBERBASE_CONVERSION_H
#define SPTS_NUMBERBASE_CONVERSION_H


// Files included
#include "Assertion.h"
#include "SPTSException.h"
#include "StandardFiles.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


struct NumberBase {
    typedef StationExceptionTypes::BadArg BadArg;

	enum Base { BINARY, OCTAL, DECIMAL, HEXIDECIMAL };

	explicit NumberBase(long value) : currentBase_(DECIMAL), currentValue_(value) 
	{ /* */ } // NumberBase(Overload1)

	explicit NumberBase(const std::string& value, Base type = DECIMAL) 
		: currentBase_(type) {

		if ( value.empty() ) {
			currentValue_ = 0;
			return;
		}
		std::string okString;
		std::stringstream convert(value);
		switch(type) {
			case BINARY:
				okString = "01";
				currentValue_ = convertdecimal(convert.str());
				break;
			case OCTAL:
				okString = "01234567";
				convert >> std::oct >> currentValue_;
				break;
			case DECIMAL:
				okString = "0123456789";
				convert >> std::dec >> currentValue_;
				break;
			case HEXIDECIMAL:
			    okString = "0123456789abcdefABCDEF";
				convert >> std::hex >> currentValue_;
		}; // Switch       
		Assert<BadArg>(value.find_first_not_of(okString) == std::string::npos, name());
	} // NumberBase(Overload2)

	NumberBase& operator=(long value) {
		currentBase_  = DECIMAL;
		currentValue_ = value;
		return(*this);
	} // operator=()

	void ConvertTo(Base type) {
		currentBase_ = type;
	} // ConvertTo()

	Base GetType() const {
		return(currentBase_);
	} // GetType()

	std::string Value(Base type) {
		ConvertTo(type);
		return(Value());
	} // Value(Overload1)

	std::string Value() {
		std::stringstream tmp2;

		switch(currentBase_) {
			case BINARY:
				tmp2 << convertbinary();
				break;
			case OCTAL:
				tmp2 << std::oct << currentValue_;
				break;
			case DECIMAL:
				tmp2 << std::dec << currentValue_;
				break;
			case HEXIDECIMAL:
				tmp2 << std::hex << currentValue_;
		}; // Switch
		return(tmp2.str());
	} // Value(Overload2)

private:
	std::string convertbinary() {
		long value = currentValue_;
		
		std::string binary("");
		long remainder = value % 2;
		long quotient  = value / 2;
		
		while ( quotient ) {
			binary += remainder ? "1" : "0";
			remainder = quotient % 2;
			quotient /= 2;
		}
		binary += remainder ? "1" : "0";
		std::reverse(binary.begin(), binary.end());
        return(binary);
	} // convertbinary()

	long convertdecimal(const std::string& str) {
		long tortn = 0, size = static_cast<long>(str.size());
        double two = 2.;
		for ( long idx = size; idx > 0; --idx ) {
			if ( str.at(idx - 1) == '1' )
				tortn += static_cast<long>(std::pow(two, size - idx));
		} // for-loop
		return(tortn);
	} // convertdecimal

    std::string name() {
        return("Number Base Class");
    }

private:
	Base currentBase_;
	long currentValue_;
}; 


#endif // SPTS_NUMBERBASE_CONVERSION_H


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
