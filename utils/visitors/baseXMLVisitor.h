#pragma once
/**
 * \file baseXMLVisitor.h
 *
 * \date Nov 11, 2013
 **/

#include <QtXml/QDomDocument>
#include <vector>



class BaseXMLVisitor
{
public:
    BaseXMLVisitor() {}


protected:
    QDomElement getChildByTag(const char *name);

    std::vector<QDomNode> mNodePath;

    virtual ~BaseXMLVisitor() {}

};

/* EOF */
