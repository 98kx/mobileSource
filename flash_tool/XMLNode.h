/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#ifndef _XML_NODE_H
#define _XML_NODE_H

#include <string>
#include "SharedPtr.h"


namespace XML
{
    class NodeImpl;

    class Node
    {
    public:
        explicit Node(NodeImpl *impl);
        ~Node();

    public:
        std::wstring GetName() const;
        std::wstring GetText() const;
        void SetText(const std::wstring &text);
        void AppendText(const std::wstring &text);
        void PrependTextBeforeChild(const std::wstring &text,
                                    unsigned int childIndex);

        unsigned int GetNumChildNodes() const;
        Node GetChildNode(unsigned int index) const;
        Node AppendChildNode(const std::wstring &name);
        Node AppendChildNode(const std::wstring &name,
                             const std::wstring &text);

        void SetAttribute(const std::wstring &name,
                          const std::wstring &value);
        std::wstring GetAttribute(const std::wstring &name) const;

    private:
        SharedPtr<NodeImpl> m_Impl;
    };
}

#endif // _XML_NODE_H
