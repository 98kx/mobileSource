/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#ifndef _XML_NODE_IMPL_H
#define _XML_NODE_IMPL_H

#include <string>
#include <MSXML2_TLB.h>


namespace XML
{
    class NodeImpl
    {
    public:
        explicit NodeImpl(const Msxml2_tlb::TCOMIXMLDOMNode &xmlNode);
        ~NodeImpl();

    public:
        std::wstring GetName();
        std::wstring GetText();
        void SetText(const std::wstring &text);
        void AppendText(const std::wstring &text);
        void PrependTextBeforeChild(const std::wstring &text,
                                    unsigned int childIndex);

        unsigned int GetNumChildNodes();
        NodeImpl* GetChildNode(unsigned int index);
        NodeImpl* AppendChildNode(const std::wstring &name);

        void SetAttribute(const std::wstring &name,
                          const std::wstring &value);
        std::wstring GetAttribute(const std::wstring &name);

    private:
        Msxml2_tlb::TCOMIXMLDOMElement GetElementInterface();

    private:
        Msxml2_tlb::TCOMIXMLDOMNode m_XMLNode;
    };
}

#endif // _XML_NODE_IMPL_H
