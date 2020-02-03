// Files included
#include "Assertion.h"
#include "FileNode.h"
#include "GenericAlgorithms.h"
#include "SPTSException.h"



//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============  
   07/27/04, sjn,
   ==============
     Added check that ifstream argument is valid to public constructor
     Added implementation of AddFileNode() and private constructor FileNode(string).
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//




/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

namespace {
    // Exception Types
    typedef StationExceptionTypes::FileFormatError FileFormatError;
    typedef StationExceptionTypes::NoFileFound     NoFileFound;

    //================
    // isEqualFirst()
    //================
    bool isEqualFirst(const std::string& s, std::string& t) {
        if ( t.size() < s.size() ) 
            return(false);
        std::pair<std::string::const_iterator, std::string::const_iterator> p;
        p = std::mismatch(s.begin(), s.end(), t.begin());
        return(p.first == s.end());
    }

    //=======================
    // matchedHeaderFooter()
    //=======================
    bool matchedHeaderFooter(const std::string& start, std::string& end) {
        std::pair<std::string::const_iterator, std::string::const_iterator> p, r;
        std::pair<std::string, std::string> headTags = FileNode::HeaderTags();
        std::pair<std::string, std::string> footTags = FileNode::FooterTags();
        
        if ( start.empty() || end.empty() ) 
            return(false);
        p = std::mismatch(headTags.first.begin(), headTags.first.end(), start.begin());       
        r = std::mismatch(footTags.first.begin(), footTags.first.end(), end.begin());
        return(std::equal(p.second, start.end(), r.second));       
    }
} // end unnamed namespace

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

// Static constant definitions
//  if file tag types change in the future, update this only
const std::string FileNode::startHeader = "[START";
const std::string FileNode::stopHeader  = "[END";
const std::string FileNode::endOfHeader = "]";

//=======================
// Constructor Overload1
//========================
FileNode::FileNode(std::ifstream& ifile) {
    bool first = true;
    bool done  = false;
    if ( !ifile )
        throw(NoFileFound(name()));
    while ( ifile ) {
        std::string buffer;
        std::getline(ifile, buffer);
        RemoveFrontBackSpace(buffer);
        RemoveTabs(buffer);
        if ( buffer.empty() ) continue;
        if ( isEqualFirst(startHeader, buffer) ) {
            if ( first ) {
                header_ = Uppercase(buffer);
                first = false;
            }
            else 
                children_.push_back(new FileNode(ifile, buffer));            
        }
        else if ( isEqualFirst(stopHeader, buffer) ) {
            footer_ = Uppercase(buffer);
            done = true;
            break;
        }     
        else {
            if ( first ) {
                ifile.close();
                throw(FileFormatError(name()));
            }
            info_.push_back(buffer);        
        } // if-else
    } // while
    ifile.close();
    if ( (!done) && first )
        throw(NoFileFound(name()));
    if ( !done )
        throw(FileFormatError(name()));
    if ( !matchedHeaderFooter(header_, footer_) )
        throw(FileFormatError(name()));       
}

//=================================
// Constructor Overload2 - private
//=================================
FileNode::FileNode(std::ifstream& ifile, const std::string& head) {
    bool done = false;
    header_ = Uppercase(head);
    while ( ifile ) {
        std::string buffer;
        std::getline(ifile, buffer);
        RemoveFrontBackSpace(buffer);
        RemoveTabs(buffer);
        if ( buffer.empty() ) continue;
        if ( isEqualFirst(stopHeader, buffer) ) {
            footer_ = Uppercase(buffer);
            done = true;
            break;
        }
        else if ( isEqualFirst(startHeader, buffer) ) 
            children_.push_back(new FileNode(ifile, buffer));
        else 
            info_.push_back(buffer);        
    } // while
    if ( !done )
        throw(FileFormatError(name()));
    if ( !matchedHeaderFooter(header_, footer_) )
        throw(FileFormatError(name()));     
}

//=================================
// Constructor Overload3 - private
//=================================
FileNode::FileNode(const std::string& head) {
    header_ = startHeader + Uppercase(head) + endOfHeader;
    footer_ = stopHeader + Uppercase(head) + endOfHeader;
}

//============
// Destructor
//============
FileNode::~FileNode() {
    for ( std::size_t idx = 0; idx < children_.size(); ++idx ) {
        if ( children_[idx] ) 
            delete(children_[idx]);
        children_[idx] = 0;
    }        
}

//===============
// AddFileNode()
//===============
FileNode* FileNode::AddFileNode(const std::string& header) {
    children_.push_back(new FileNode(header));
    return(children_.back());
}

//==============
// FooterTags()
//==============
std::pair<std::string, std::string> FileNode::FooterTags() {
    return(std::make_pair(stopHeader, endOfHeader));

}

//=================
// GetChildNodes()
//=================
std::vector<FileNode*>& FileNode::GetChildNodes() {
    return(children_);
}

//===============
// GetFileNode()
//===============
FileNode* FileNode::GetFileNode(const std::string& header) {
    std::string h = Uppercase(RemoveAllWhiteSpace(header));
    if ( Uppercase(RemoveAllWhiteSpace(header_)) == h )
        return(this);
    for ( std::size_t idx = 0; idx < children_.size(); ++idx ) {
        if ( Uppercase(RemoveAllWhiteSpace(children_[idx]->header_)) == h )
            return(children_[idx]);
        else {
            FileNode* fn = children_[idx]->GetFileNode(h);
            if ( fn ) 
                return(fn);
        }
    }
    return(0);
}

//=============
// GetFooter()
//=============
std::string FileNode::GetFooter() const {
    return(footer_);
}

//=============
// GetHeader()
//=============
std::string FileNode::GetHeader() const {
    return(header_);
}

//===========
// GetInfo()
//===========
const std::vector<std::string>& FileNode::GetInfo() {
    return(info_);
}

//==============
// HeaderTags()
//==============
std::pair<std::string, std::string> FileNode::HeaderTags() {
    return(std::make_pair(startHeader, endOfHeader));

}

//========
// name()
//========
std::string FileNode::name() {
    return("FileNode Class");
}

//===============
// ReplaceInfo()
//===============
void FileNode::ReplaceInfo(const std::vector<std::string>& newInfo) {
    info_ = newInfo;
}

//========
// Size()
//========
std::size_t FileNode::Size() const {
    std::size_t size = 1;
    if ( header_.empty() )
        --size;
    return(size + children_.size());        
}

//=====================
// extraction operator
//=====================
std::ostream& operator<<(std::ostream& os, const FileNode& fn) {
    static std::string space  = "";
    static const std::string spacer = "    ";

    space += spacer;
    os << fn.header_ << std::endl;
    for ( std::size_t idx = 0; idx < fn.info_.size(); ++idx )
        os << space << fn.info_[idx] << std::endl;
    for ( std::size_t idx = 0; idx < fn.children_.size(); ++idx )
        os << space << *(fn.children_[idx]);        
    space = space.substr(0, space.size() - spacer.size());
    os << space << fn.footer_ << std::endl;    
    return(os);
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

