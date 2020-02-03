// Files included
#include "StringAlgorithms.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============  
   07/26/04, sjn,
   ==============
     SplitString() Overload1 had a local variable 'i' declared in for-loop scope and
       being used outside the loop.  MS VC++ 7.1 fails to catch this error.
     RemoveAllNewlines() had an unsigned integral, 'idx', initialized to -1.
     Replaced all std::size_t types with std::string::size_type
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace { // unnamed
    //===========
    // getType()
    //===========
    std::string getType(const std::string& str, const std::string& type) {
        long size = static_cast<long>(str.size());
        std::string toRtn;
        for ( long idx = 0; idx < size; ++idx ) {
            if ( type.find(str[idx]) != std::string::npos )
                toRtn += str[idx];
        }
        return(toRtn);
    }
} // end unnamed namespace

//=====================
// GetNumericInteger()
//=====================
std::string GetNumericInteger(const std::string& str) {
    return(getType(str, "-0123456789"));
}

//======================
// GetNumericFloating()
//======================
std::string GetNumericFloating(const std::string& str) {
    return(getType(str, "-0123456789.Ee"));
}

//=============
// IsInteger()
//=============
bool IsInteger(const std::string& num) {
    if ( num.empty() ) return(false);
    return(num.find_first_not_of("-0123456789") == std::string::npos);
}

//==============
// IsFloating()
//==============
bool IsFloating(const std::string& num) {
    if ( num.empty() ) return(false);
    return(num.find_first_not_of("-0123456789.Ee") == std::string::npos);
}

//=====================
// RemoveAllNewlines()
//=====================
void RemoveAllNewlines(std::string& s) {    
    std::string::size_type idx = 0;
    while ( idx < s.size() ) {        
        if ( s.empty() ) break;
        if ( s[idx] == '\n' ) { 
            s = s.substr(0, idx) + s.substr(idx + 1);
            --idx;
        }
        ++idx;
    }
}

//=================================
// RemoveAllWhiteSpace() Overload1
//=================================
std::string RemoveAllWhiteSpace(const std::string& str) {
    long size = static_cast<long>(str.size());
    std::string toRtn = "";
    for ( long idx = 0; idx < size; ++idx ) {
        if ( !isspace(str[idx]) )
            toRtn += str[idx];
    }
    return(toRtn);
}

//===================
// RemoveBackSpace()
//===================
void RemoveBackSpace(std::string& s) {
    bool done = false;
    while ( ! done ) {
        if ( s.empty() ) break;
        if ( s[s.size()-1] == ' ' ) {
            if ( s.size() > 1 )
                s = s.substr(0, s.size()-1);
            else
                s = "";
        }
        else 
            break;
    } // while
}

//====================
// RemoveFrontSpace()
//====================
void RemoveFrontSpace(std::string& s) {
    bool done = false;
    while ( ! done ) {
        if ( s.empty() ) 
            break;
        if ( s[0] == ' ' ) 
            s = s.substr(1);
        else 
            break;
    } // while
}

//========================
// RemoveFrontBackSpace()
//========================
void RemoveFrontBackSpace(std::string& s) {
    RemoveBackSpace(s);
    RemoveFrontSpace(s);
}

//==============
// RemoveTabs()
//==============
void RemoveTabs(std::string& s) {
    bool done = false;
    std::string::size_type idx = 0;
    while ( ! done ) {
        if ( s.empty() ) break;
        if ( s[idx] == '\t' ) {
            s = s.substr(0, idx) + s.substr(idx+1);
            idx = 0;
        }
        if ( ++idx == s.size() ) break;
    } // while
}

//=========================
// SplitString() Overload1
//=========================
std::vector<std::string> SplitString(const std::string& s, char delim) {
    // Split string around each delim instance
    std::vector<std::string> toRtn;
    if ( s.empty() )
        return(toRtn);    
    std::string::size_type j = 0, i = 0;
    for ( ; i < s.size(); ) {
        if ( s[i] == delim ) {
            if ( 0 != i ) 
                toRtn.push_back(s.substr(j, i-j));
            j = ++i;         
        }
        else
            ++i;
    } // for
    if ( j != i )
        toRtn.push_back(s.substr(j));
 
    // Remove leading/trailing spaces, any tabs and any newlines
    std::vector<std::string>::iterator k = toRtn.begin();
    while ( k != toRtn.end() ) {
        RemoveTabs(*k);
        RemoveFrontBackSpace(*k);
        RemoveAllNewlines(*k);
        if ( ! k->empty() )
            ++k;
        else {
            toRtn.erase(k);
            k = toRtn.begin();
        }
    } // while
    return(toRtn);
}

//=========================
// SplitString() Overload2
//=========================
std::vector<std::string> SplitString(const std::string& s, const std::string& delim) {    
    std::vector<std::string> toRtn;
    if ( s.empty() || delim.empty() )
        return(toRtn);

    // Split string around each delim instance
    std::string::size_type found = s.find(delim), last = 0;
    if ( found == std::string::npos ) {
        std::string tmp = s;
        tmp = RemoveAllWhiteSpace(tmp);
        std::vector<std::string> v;
        if ( ! tmp.empty() )
            v.push_back(tmp);
        return(v);
    }
    bool done = false;
    while ( ! done ) {
        std::string tmp = s.substr(last, found-last);
        found += delim.size();
        RemoveTabs(tmp);
        RemoveFrontBackSpace(tmp);
        if ( ! tmp.empty() ) 
            toRtn.push_back(tmp);
        last = found;
        if ( last >= s.size() )
            break;
        tmp = s.substr(last);
        if ( tmp.empty() ) 
            break;
        std::string::size_type next = tmp.find(delim);
        if ( next == std::string::npos )
            found = s.size();
        else
            found = last + next;
    }
    if ( last != found ) {
        std::string tmp = s.substr(last);
        RemoveTabs(tmp);
        RemoveFrontBackSpace(tmp);
        RemoveAllNewlines(tmp);
        if ( ! tmp.empty() )
            toRtn.push_back(tmp);
    }
    return(toRtn);
}

//=============
// Uppercase()
//=============
std::string Uppercase(const std::string& str) {
    long size = static_cast<long>(str.size());
    std::string toRtn;
    for ( long idx = 0; idx < size; ++idx )
        toRtn += static_cast<char>(toupper(str[idx]));
    return(toRtn);
}

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
