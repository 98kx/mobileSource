/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#include "XMLNode.h"
#include "XMLNodeImpl.h"


using XML::Node;
using XML::NodeImpl;

Node::Node(NodeImpl *impl)
    : m_Impl(impl)
{
}

Node::~Node()
{
}

std::wstring Node::GetName() const
{
    return m_Impl->GetName();
}

std::wstring Node::GetText() const
{
    return m_Impl->GetText();
}

void Node::SetText(const std::wstring &text)
{
    m_Impl->SetText(text);
}

void Node::AppendText(const std::wstring &text)
{
    m_Impl->AppendText(text);
}

void Node::PrependTextBeforeChild(const std::wstring &text,
                                  unsigned int childIndex)
{
    m_Impl->PrependTextBeforeChild(text, childIndex);
}

unsigned int Node::GetNumChildNodes() const
{
    return m_Impl->GetNumChildNodes();
}

Node Node::GetChildNode(unsigned int index) const
{
    return Node(m_Impl->GetChildNode(index));
}

Node Node::AppendChildNode(const std::wstring &name)
{
    return Node(m_Impl->AppendChildNode(name));
}

Node Node::AppendChildNode(const std::wstring &name,
                           const std::wstring &text)
{
    Node childNode(AppendChildNode(name));
    childNode.SetText(text);

    return childNode;
}

void Node::SetAttribute(const std::wstring &name,
                        const std::wstring &value)
{
    m_Impl->SetAttribute(name, value);
}

std::wstring Node::GetAttribute(const std::wstring &name) const
{
    return m_Impl->GetAttribute(name);
}
