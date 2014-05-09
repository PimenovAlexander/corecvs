/**
 * \file baseXMLVisitor.cpp
 *
 * \date Nov 11, 2013
 **/
#include "baseXMLVisitor.h"

#include <QtXml/QDomDocument>

QDomElement BaseXMLVisitor::getChildByTag(const char *name)
{

    //qDebug("BaseXMLVisitor::getChildByTag(%s): called", name );
    QDomNode mainNode = mNodePath.back();
    if (mainNode.isNull()) {
        qDebug("  BaseXMLVisitor::getChildByTag(): path is null");
        return QDomElement();
    }

    if (mainNode.isElement())
    {
//        qDebug("  BaseXMLVisitor::getChildByTag(): path ends with element");

        QDomElement mainElement = mainNode.toElement();
        QDomNodeList nodesWithTag = mainElement.elementsByTagName(name);
        if (nodesWithTag.length() != 0)
        {
//            qDebug("  BaseXMLVisitor::getChildByTag(): found sub element");
            return nodesWithTag.at(0).toElement();
        }
        return QDomElement();
    }

    if (mainNode.isDocument())
    {
//        qDebug("  BaseXMLVisitor::getChildByTag(): path ends with document");
        QDomDocument mainDocument = mainNode.toDocument();
        QDomNodeList nodesWithTag = mainDocument.elementsByTagName(name);
        if (nodesWithTag.length() != 0)
        {
//            qDebug("  BaseXMLVisitor::getChildByTag(): found sub element");
            return nodesWithTag.at(0).toElement();
        }
        return QDomElement();
    }

    return QDomElement();
}
