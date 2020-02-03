// Macro Guard
#ifndef SPTS_FILENODE_H
#define SPTS_FILENODE_H

// Files included
#include "StandardFiles.h"


//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//
/* 
   ==============  
   07/27/04, sjn,
   ==============
   Added AddFileNode() and private constructor explicit FileNode(const std::string&);
*/
//=====================================================================================//
//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Changes <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<//
//=====================================================================================//


/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/

struct FileNode {
   // Static Member Functions
    static std::pair<std::string, std::string> HeaderTags();
    static std::pair<std::string, std::string> FooterTags();

    // Constructor and Destructor
    explicit FileNode(std::ifstream& ifile);
    ~FileNode();

    // Public Interface
    FileNode* AddFileNode(const std::string& header);
    std::vector<FileNode*>& GetChildNodes();   
    FileNode* GetFileNode(const std::string& header);
    std::string GetFooter() const;
    std::string GetHeader() const;
    const std::vector<std::string>& GetInfo();
    void ReplaceInfo(const std::vector<std::string>& newInfo);
    std::size_t Size() const;  
    friend std::ostream& operator<<(std::ostream& os, const FileNode& fn);

private:
    explicit FileNode(const std::string& head);
    FileNode(std::ifstream& ifile, const std::string& head);
    std::string name();

private:
    std::vector<FileNode*> children_;
    std::string header_;
    std::string footer_;
    std::vector<std::string> info_;
    static const std::string startHeader;
    static const std::string stopHeader;
    static const std::string endOfHeader;
};

#endif // SPTS_FILENODE_H

/***************************************************************************************/
////////////////////////////////////CRANE INTERPOINT/////////////////////////////////////
//////////////////////////////////SPACE-POWER DIVISION///////////////////////////////////
/***************************************************************************************/


/*---------------------------------------------------------//
       "Hardcoded types are to generic code what magic 
        constants are to regular code" 
                                 - Andrei Alexandrescu
//---------------------------------------------------------*/
