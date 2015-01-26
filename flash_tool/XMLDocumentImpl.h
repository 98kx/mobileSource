/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#ifndef _XML_DOCUMENT_IMPL_H
#define _XML_DOCUMENT_IMPL_H

#include <string>
#include <MSXML2_TLB.h>
#include "NonCopyable.h"


namespace XML
{
    class NodeImpl;

    class DocumentImpl : private NonCopyable
    {
    public:
        explicit DocumentImpl(const std::wstring &xmlFile);
        DocumentImpl(const std::wstring &xmlVersion,
                     const std::wstring &encoding,
                     const std::wstring &rootNodeName);
        ~DocumentImpl();

    public:
        void Load(const std::wstring &xmlFile);
        void Save(const std::wstring &xmlFile);
        void Validate(const std::wstring &schemaFile);
        NodeImpl* GetRootNode();

    private:
        void InitDoc();
        void CreateProcessingInstruction(const std::wstring &target,
                                         const std::wstring &data);
        void CreateRootNode(const std::wstring &rootNodeName);
        std::string FormatParseError(
                const Msxml2_tlb::TCOMIXMLDOMParseError &parseError);

    private:
        Msxml2_tlb::TCOMIXMLDOMDocument2 m_MainDoc;
    };
}

#endif // _XML_DOCUMENT_IMPL_H
