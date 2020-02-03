// Macro Guard
#ifndef STRINGALGORITHMS_H
#define STRINGALGORITHMS_H

// Files included
#include "StandardFiles.h"

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//=====================
// GetNumericInteger()
//=====================
extern std::string GetNumericInteger(const std::string& str);

//======================
// GetNumericFloating()
//======================
extern std::string GetNumericFloating(const std::string& str);

//=============
// IsInteger()
//=============
extern bool IsInteger(const std::string& num);

//==============
// IsFloating()
//==============
extern bool IsFloating(const std::string& num);

//=====================
// RemoveAllNewlines()
//=====================
extern void RemoveNewlines(std::string& s);

//=======================
// RemoveAllWhiteSpace() 
//=======================
extern std::string RemoveAllWhiteSpace(const std::string& str);

//===================
// RemoveBackSpace()
//===================
extern void RemoveBackSpace(std::string& s);

//========================
// RemoveFrontBackSpace()
//========================
extern void RemoveFrontBackSpace(std::string& s);

//====================
// RemoveFrontSpace()
//====================
extern void RemoveFrontSpace(std::string& s);

//==============
// RemoveTabs()
//==============
extern void RemoveTabs(std::string& s);

//=========================
// SplitString() Overload1
//=========================
  // space allowed as delim
extern std::vector<std::string> SplitString(const std::string& s, char delim);

//=========================
// SplitString() Overload2
//=========================
  // no spaces allowed in delim
extern std::vector<std::string> 
                  SplitString(const std::string& s, const std::string& delim);

//=============
// Uppercase()
//=============
extern std::string Uppercase(const std::string& str);


#endif // STRINGALGORITHMS_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
