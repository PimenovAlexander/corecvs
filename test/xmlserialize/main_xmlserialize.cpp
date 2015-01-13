#include <stdio.h>
#include <unistd.h>

#include <QtXml/QDomDocument>

#include "vector3d.h"
#include "xmlSetter.h"
#include "xmlGetter.h"

void quickTest()
{
    QDomDocument document("document");
    //QDomElement mDocumentElement = document.createElement("xml");
    //QDomElement mDocumentElement = document; //document.documentElement();

    //QDomElement mDocumentElement1 = mDocumentElement;
    QDomElement newElement = document.createElement("tag");
    newElement.setAttribute("value", "test");
    QDomNode node = document.appendChild(newElement);

    qDebug() << document.toString();
}

int main (int /*argc*/, char **/*argv*/)
{
    printf("Quick test\n");

    quickTest();

	printf("Serializing some data\n");

	Vector3dd someData(1.0,2.0,5.0);
	{
	    XmlSetter setter("output.xml");
	    setter.visit(someData, "This_is_some_data");
	}

    printf("====== Loading back some data ======\n");

	Vector3dd result(1.5,2.5,5.5);
	{
	    XmlGetter getter("output.xml");
	    getter.visit(result, "This_is_some_data");
	}

    cout << "Result:" << result;

	return 0;
}
