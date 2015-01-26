/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#include "XMLDocument.h"
#include "XMLDocumentImpl.h"


using XML::Document;
using XML::DocumentImpl;
using XML::Node;

Document::Document(const std::wstring &xmlFile)
    : m_Impl(new DocumentImpl(xmlFile))
{
}

Document::Document(const std::wstring &xmlVersion,
                   const std::wstring &encoding,
                   const std::wstring &rootNodeName)
    : m_Impl(new DocumentImpl(xmlVersion, encoding, rootNodeName))
{
}

Document::~Document()
{
}

void Document::Load(const std::wstring &xmlFile)
{
    m_Impl->Load(xmlFile);
}

void Document::Save(const std::wstring &xmlFile)
{
    m_Impl->Save(xmlFile);
}

void Document::Validate(const std::wstring &schemaFile)
{
    m_Impl->Validate(schemaFile);
}

Node Document::GetRootNode()
{
    return Node(m_Impl->GetRootNode());
}

void Document::Beautify()
{
    Node rootNode(GetRootNode());
    DoBeautify(rootNode, 1);
}

static void AppendTabs(std::wstring &str, unsigned int numTabs)
{
    for (unsigned int i=0; i<numTabs; ++i)
    {
        str += L"\t";
    }
}

void Document::DoBeautify(Node &node, unsigned int depth)
{
    unsigned int numChildNodes = node.GetNumChildNodes();

    if (numChildNodes == 0)
    {
        return;
    }

    std::wstring spaces(L"\n");
    AppendTabs(spaces, depth);

    for (unsigned int i=0; i<numChildNodes; ++i)
    {
        const unsigned int childIndex = i * 2;
        node.PrependTextBeforeChild(spaces, childIndex);
    }

    numChildNodes = node.GetNumChildNodes();

    for (unsigned int i=1; i<numChildNodes; i+=2)
    {
        Node childNode(node.GetChildNode(i));
        DoBeautify(childNode, depth+1);
    }

    spaces = L"\n";
    AppendTabs(spaces, depth-1);

    node.AppendText(spaces);
}
