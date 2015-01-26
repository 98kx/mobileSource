/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#ifndef _XML_DOCUMENT_H
#define _XML_DOCUMENT_H

#include <memory>
#include <string>
#include "NonCopyable.h"
#include "XMLNode.h"


namespace XML
{
    class DocumentImpl;

    class Document : private NonCopyable
    {
    public:
        explicit Document(const std::wstring &xmlFile);
        Document(const std::wstring &xmlVersion,
                 const std::wstring &encoding,
                 const std::wstring &rootNodeName);
        ~Document();

    public:
        void Load(const std::wstring &xmlFile);
        void Save(const std::wstring &xmlFile);
        void Validate(const std::wstring &schemaFile);
        Node GetRootNode();
        void Beautify();

    private:
        void DoBeautify(Node &node, unsigned int depth);

    private:
        std::auto_ptr<DocumentImpl> m_Impl;
    };
}

#endif // _XML_DOCUMENT_H
