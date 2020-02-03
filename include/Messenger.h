// Macro Guard
#ifndef SPTS_MESSENGER_H
#define SPTS_MESSENGER_H

// Files included
#include "StandardFiles.h"

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct Messenger { 
    //========================
    // Start Public Interface
    //========================
    Messenger() : toOutput_(new std::stringstream) { /* */ }
    ~Messenger() { /* */ }

    void Clear() {
        toOutput_.reset(new std::stringstream);        
    }

    bool IsEmpty() {
        return(toOutput_->str().empty());
    }

    template <typename Type>
    Messenger& operator<<(const Type& s) {
        (*toOutput_) << s << std::endl;
        return(*this);
    }

    friend std::ostream& operator<<(std::ostream& os, const Messenger& message) {
        os << message.toOutput_->str();
        return(os);
    }
    //======================
    // End Public Interface
    //======================    
private:
    std::auto_ptr<std::stringstream> toOutput_;
};

#endif // SPTS_MESSENGER_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu 
//---------------------------------------------------------*/
