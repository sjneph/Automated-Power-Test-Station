// Macro Guard
#ifndef SPTS_DIALOG_H
#define SPTS_DIALOG_H

// Files included
#include "GenericAlgorithms.h"
#include "NoCopy.h"
#include "OperatorInterface.h"
#include "SingletonType.h"
#include "StandardFiles.h"


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

//===========
// DialogBox
//===========
struct DialogBox {
    // Public enumeration
    enum Flag { endl = 0 };
    
    // Overloaded insertion operator
    template <typename Type>
    DialogBox& operator<<(const Type& type) {
        if ( 0 == dos_.get() )
            dos_.reset(new std::stringstream);
        *dos_ << convert<std::string>(type);
        return(*this);
    }

    // Overloaded insertion operator
    DialogBox& operator<<(const std::string& str) {
        if ( 0 == dos_.get() )
            dos_.reset(new std::stringstream);
        *dos_ << str;
        return(*this);
    }

    // Overloaded insertion operator
    DialogBox& operator<<(Flag f) {
        if ( 0 == dos_.get() )
            dos_.reset(new std::stringstream);
        if ( f == endl )
            *dos_ << "\n";
        return(*this);
    }

    // DisplayInfo()
    void DisplayInfo() {
        OperatorInterface* oi = SingletonType<OperatorInterface>::Instance();
        oi->showDialogMessage(dos_->str()); 
        dos_.reset(new std::stringstream);
    }

    // DisplayInteractive()
    void DisplayInteractive() {
        OperatorInterface* oi = SingletonType<OperatorInterface>::Instance();
        oi->showDialogInteractive(dos_->str()); 
        dos_.reset(new std::stringstream);
    }
    
    // DisplayWarning
    void DisplayWarning() {
        OperatorInterface* oi = SingletonType<OperatorInterface>::Instance();
        oi->showDialogWarning(dos_->str());
        dos_.reset(new std::stringstream);
    }

private:
    friend class SingletonType<DialogBox>;
    DialogBox() : dos_(0) { /* */ }
    ~DialogBox() { /* */ }

private:
    std::auto_ptr<std::stringstream> dos_;
};
//===========
// DialogBox
//===========

#endif // SPTS_DIALOG_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
