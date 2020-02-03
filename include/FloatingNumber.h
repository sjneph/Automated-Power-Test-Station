// Macro Guard
#ifndef FLOAT_NUMERIC_H
#define FLOAT_NUMERIC_H

// Files included
#include "StandardFiles.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

// Post-increment and Post-decrement operators undefined on purpose
//  use Pre-increment and Pre-decrement instead.

//========================
// FloatingNumber<> Class
//========================
template <
          typename Type, 
          bool ToCheck = false, 
          int MinValue = 0, 
          int MaxValue = 0,
          typename RangeException = bool
         >
class FloatingNumber {
    typedef 
    enum { DEFAULT = -1 };
public:
    typedef Type ValueType;
    FloatingNumber() : number_(0), precision_(DEFAULT), explicitSet_(false)
    { /* */ }
    explicit FloatingNumber(const std::string& number) : precision_(DEFAULT), 
                                                         explicitSet_(false) {
        std::stringstream toConvert(number.c_str());
        ValueType toCompare;
        toConvert >> toCompare;   
        number_ = toCompare;
        checkRange();  
    }
    FloatingNumber(ValueType number) : precision_(DEFAULT), explicitSet_(false), 
                                       number_(number) {
        checkRange();
    }
    FloatingNumber(const FloatingNumber& src) 
                           : precision_(src.precision_), number_(src.number_), 
                             explicitSet_(src.explicitSet_), 
                             explicitValue_(src.explicitValue_) {
        checkRange();
    }
    FloatingNumber operator=(const FloatingNumber& src) {
        if ( this != &src ) {
            precision_ = src.precision_;
            number_ = src.number_;
            explicitSet_ = src.explicitSet_;
            explicitValue_ = src.explicitValue_;
            checkRange();            
        }
        return(*this);
    }
    operator ValueType() {        
        return(number_);
    }
    long GetPrecision() const {
        return(precision_);
    }
    int Max() {
        return(MaxValue);
    }
    int Min() {
        return(MinValue);
    }
    void SetExplicit(const std::string& explicitValue) {
        explicitValue_ = explicitValue;
        explicitSet_ = true;
        number_ = FloatingNumber(explicitValue_);
        precision_ = DEFAULT;        
    }
    void SetPrecision(long precision) {
        if ( precision < 0 )
            precision_ = DEFAULT;
        precision_ = precision;
    }
    ValueType Value() const {
        return(number_);
    }
    std::string ValueStr() const {
        if ( explicitSet_ ) // user defined explicitly
            return(explicitValue_);

        std::stringstream s;
        if ( precision_ != DEFAULT )
            s.precision(precision_);
        s.setf(std::ios_base::fixed, std::ios_base::floatfield);
        s << number_;
        std::string toRtn = s.str();
        if ( (number_ < 0) && (!toRtn.empty()) &&
             (toRtn[0] == '-') &&
             (toRtn.find_first_not_of("-.0") == std::string::npos)
           ) { // of form  -0.00...0 --> make 0.00...0
            return(toRtn.substr(1));
        }            
        return(toRtn);
    }
    FloatingNumber& operator++() {
        ++number_;
        checkRange();
        return(*this);
    }    
    FloatingNumber& operator+=(const FloatingNumber& fn) {
        number_ += fn.number_;
        checkRange();
        return(*this);
    }
    FloatingNumber& operator--() {
        --number_;
        checkRange();
        return(*this);
    }
    FloatingNumber& operator-=(const FloatingNumber& fn) {
        number_ -= fn.number_;
        checkRange();
        return(*this);
    }
    FloatingNumber& operator*=(const FloatingNumber& fn) {
        number_ *= fn.number_;
        checkRange();
        return(*this);
    }
    FloatingNumber& operator/=(const FloatingNumber& fn) {
        number_ /= fn.number_;
        checkRange();
        return(*this);
    }
    bool operator<(const FloatingNumber& fn) const {
        return(number_ < fn.number_);
    }
    bool operator>(const FloatingNumber& fn) const {
        return(number_ > fn.number_);
    }
    bool operator==(const FloatingNumber& fn) const {
        return(number_ == fn.number_);
    }
    bool operator!=(const FloatingNumber& fn) const {
        return(number_ != fn.number_);
    }
    bool operator>=(const FloatingNumber& fn) const {
        return(number_ >= fn.number_);
    }
    bool operator<=(const FloatingNumber& fn) const {
        return(number_ <= fn.number_);
    }
    friend FloatingNumber absolute(const FloatingNumber& fn) {
        if ( fn.Value() < 0 ) {
            FloatingNumber toCheck = fn.number_ * -1;
            toCheck.checkRange();                  
            return(toCheck);
        }
        return(fn);
    }
    friend FloatingNumber operator+(const FloatingNumber& f1, const FloatingNumber& f2) {
        FloatingNumber toCheck = f1.number_ + f2.number_;            
        toCheck.checkRange();
        return(toCheck);
    }
    friend FloatingNumber operator-(const FloatingNumber& f1, const FloatingNumber& f2) {        
        FloatingNumber toCheck = f1.number_ - f2.number_;
        toCheck.checkRange();
        return(toCheck);
    }    
    friend FloatingNumber operator*(const FloatingNumber& f1, const FloatingNumber& f2) {
        FloatingNumber toCheck = f1.number_ * f2.number_;
        toCheck.checkRange();
        return(toCheck);        
    }
    friend FloatingNumber operator/(const FloatingNumber& f1, const FloatingNumber& f2) {
        FloatingNumber toCheck = f1.number_ / f2.number_;
        toCheck.checkRange();
        return(toCheck);        
    }
    friend std::ostream& operator<<(std::ostream& os, const FloatingNumber& f) { 
        os << f.number_;
        return(os);
    }
    friend std::istream& operator>>(std::istream& is, FloatingNumber& f) {
        // temp variable needed to prevent infinite recursion
        FloatingNumber::ValueType r;
        is >> r;
        f = FloatingNumber(r); // check range of r
        return(is);
    }
private:
    FloatingNumber& operator++(int); // don't use FloatingNumber++
    FloatingNumber& operator--(int); // don't use FloatingNumber--
private:
    template <bool ToType>
    struct NestedTypify {
        enum { val = ToType };
    };
    void checkRange() {
        checkRange(NestedTypify<ToCheck>());
    }
    void checkRange(const NestedTypify<true>&) {
        if ( number_ < static_cast<ValueType>(MinValue) ||
                      number_ > static_cast<ValueType>(MaxValue) )
            throw(RangeException());    
    }
    void checkRange(const NestedTypify<false>&) { /* nada */ }
    std::string name() { 
        return("Floating Number Class");
    }
private:
    ValueType number_;
    std::string explicitValue_;
    bool explicitSet_;
    long precision_;
};

#endif // FLOAT_NUMERIC_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
