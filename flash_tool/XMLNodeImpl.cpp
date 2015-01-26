/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
*
*****************************************************************************/

#include <cassert>
#include <stdexcept>
#include <vcl.h>
#include "XMLNodeImpl.h"


using XML::NodeImpl;
using Msxml2_tlb::TCOMIXMLDOMDocument;
using Msxml2_tlb::TCOMIXMLDOMElement;
using Msxml2_tlb::TCOMIXMLDOMNode;
using Msxml2_tlb::TCOMIXMLDOMNodeList;
using Msxml2_tlb::TCOMIXMLDOMText;
using std::runtime_error;

//
// NodeImpl
//
NodeImpl::NodeImpl(const TCOMIXMLDOMNode &xmlNode)
    : m_XMLNode(xmlNode)
{
}

NodeImpl::~NodeImpl()
{
}

std::wstring NodeImpl::GetName()
{
    return static_cast<BSTR>(TOleString(m_XMLNode->nodeName));
}

std::wstring NodeImpl::GetText()
{
    return static_cast<BSTR>(TOleString(m_XMLNode->text));
}

void NodeImpl::SetText(const std::wstring &text)
{
    m_XMLNode->set_text(WideString(text.c_str()));
}

void NodeImpl::AppendText(const std::wstring &text)
{
    TCOMIXMLDOMDocument document(m_XMLNode->get_ownerDocument());
    assert(document);

    const TOleString oleStr(WideToOLESTR(text.c_str()));

    TCOMIXMLDOMText textNode(document->createTextNode(oleStr));
    assert(textNode);

    if (FAILED(m_XMLNode->appendChild(textNode, NULL)))
    {
        throw runtime_error("Failed to add newline");
    }
}

void NodeImpl::PrependTextBeforeChild(const std::wstring &text,
                                      unsigned int childIndex)
{
    assert(childIndex < GetNumChildNodes());

    TCOMIXMLDOMNodeList childNodes(m_XMLNode->childNodes);
    TCOMIXMLDOMNode node(childNodes->get_item(childIndex));

    VARIANT variant;
    ::VariantInit(&variant);

    variant.punkVal = node;
    variant.vt = VT_UNKNOWN;

    TCOMIXMLDOMDocument document(m_XMLNode->get_ownerDocument());
    assert(document);

    const TOleString oleStr(WideToOLESTR(text.c_str()));

    TCOMIXMLDOMText textNode(document->createTextNode(oleStr));
    assert(textNode);

    if (FAILED(m_XMLNode->insertBefore(textNode, variant, NULL)))
    {
        throw runtime_error("Failed to prepend text");
    }
}

unsigned int NodeImpl::GetNumChildNodes()
{
    return TCOMIXMLDOMNodeList(m_XMLNode->childNodes)->length;
}

NodeImpl* NodeImpl::GetChildNode(unsigned int index)
{
    assert(index < GetNumChildNodes());

    return new NodeImpl(
            TCOMIXMLDOMNodeList(m_XMLNode->childNodes)->get_item(index));
}

NodeImpl* NodeImpl::AppendChildNode(const std::wstring &name)
{
    TCOMIXMLDOMDocument document(m_XMLNode->get_ownerDocument());
    assert(document);

    TCOMIXMLDOMElement childNode(
            document->createElement(WideString(name.c_str())));
    assert(childNode);

    if (FAILED(m_XMLNode->appendChild(childNode, NULL)))
    {
        throw runtime_error("Failed to append child node");
    }

    return new NodeImpl(childNode);
}

void NodeImpl::SetAttribute(const std::wstring &name,
                            const std::wstring &value)
{
    const WideString _name(name.c_str());
    const WideString _value(value.c_str());

    VARIANT variant;
    ::VariantInit(&variant);

    variant.bstrVal = _value;
    variant.vt = VT_BSTR;

    if (FAILED(GetElementInterface()->setAttribute(_name, variant)))
    {
        throw runtime_error("Failed to add attribute to the node");
    }
}

std::wstring NodeImpl::GetAttribute(const std::wstring &name)
{
    const WideString _name(name.c_str());
    VARIANT _value;

    if (FAILED(GetElementInterface()->getAttribute(_name, &_value)))
    {
        throw runtime_error("Failed to get attribute of the node");
    }

    if (_value.bstrVal == NULL)
    {
        return std::wstring();
    }

    const std::wstring value(_value.bstrVal);
    ::VariantClear(&_value);

    return value;
}

TCOMIXMLDOMElement NodeImpl::GetElementInterface()
{
    TCOMIXMLDOMElement element;

    if (FAILED(m_XMLNode->QueryInterface(
                    Msxml2_tlb::IID_IXMLDOMElement, (void **) &element)))
    {
        throw runtime_error("IXMLDOMElement interface not supported");
    }

    return element;
}
