#ifndef PROPERTY_LIST_H_
#define PROPERTY_LIST_H_

/**
 * \file propertyList.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date May 24, 2010
 * \author alexander
 */

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <limits>

#include "global.h"
#include "utils.h"

namespace corecvs {

using namespace std;

class PropertyListVectorContainer : public vector< pair <string, string> >
{
protected:
    PropertyListVectorContainer::iterator find (string name)
    {
        PropertyListVectorContainer::iterator it;
        for(it = begin(); it != end(); ++it)
        {
            if ((*it).first == name)
            {
                break;
            }
        }
        return it;
    }

    void insert(pair <string, string> pair)
    {
        push_back(pair);
    }
};

class PropertyList : public PropertyListVectorContainer
{
private:
template<typename ValueType>
    ValueType getTypedProperty(string name, ValueType defaultValue)
    {
        string* value = getProperty(name);
        if (value == NULL)
            return defaultValue;
        istringstream i(*value);
        ValueType result = defaultValue;
        i >> result;
        return result;
    }

template<typename ValueType>
    void setTypedProperty(string name, ValueType value)
    {
        PropertyList::iterator it = this->find(name);
        ostringstream o;
        o.precision(numeric_limits<double>::digits10 + 3);
        o << value;
        string tosave = o.str();

        if (it == this->end())
        {
            insert (pair<string, string>(name,tosave));
            return;
        }
        (*it).second = value;
    }
public:
    string *getProperty(string name)
    {
         PropertyList::iterator it = this->find(name);
        if (it == this->end())
            return NULL;
        else
            return &((*it).second);
    }


    int getIntProperty(string name, int defaultValue)
    {
        return getTypedProperty<int>(name, defaultValue);
    }

    double getDoubleProperty(string name, double defaultValue)
    {
        return getTypedProperty<double>(name, defaultValue);
    }

    string getStringProperty(string name, string defaultValue)
    {
        return getTypedProperty<string>(name, defaultValue);
    }


    void setIntProperty(string name, int value)
    {
        setTypedProperty<int>(name, value);
    }

    void setDoubleProperty(string name, double value)
    {

        setTypedProperty<double>(name, value);
    }

    void setStringProperty(string name, string value)
    {
        setTypedProperty<string>(name, value);
    }

    /* Fix the limited line length */
    int load(istream &inStream)
    {
        string line;

        while (!inStream.eof())
        {
            corecvs::HelperUtils::getlineSafe(inStream, line);
            std::istringstream streamLine(line);
            //cout << "Parsing:" << line << endl;

            string name;
            string dummy;
            string value;
            streamLine >> name;
            streamLine >> dummy;

            if (streamLine.fail() || dummy != "=") {
                continue;
            }

            getline(streamLine, value);

            //cout << name << ":" << dummy << ":" << value << endl;

            DOTRACE(("Property: %s = %s \n", name.c_str(), value.c_str()));
            setStringProperty(name.c_str(), value.c_str());
        }
        return true;
    }

    int load(string fileName)
    {
        ifstream file;
        file.open (fileName.c_str(),ios::in);
        cout << "Opening:" << fileName << endl;
        if (file.fail())
        {
            cout << "Can't open property list file" << endl;
            return false;
        }

        int result = load(file);
        file.close();
        return result;
    }

    void save(ostream &outStream)
    {
        outStream << "#There are " << size() << " parameters\n";
        PropertyList::iterator it;
        for (it = this->begin(); it != this->end(); it++)
        {
            outStream << (*it).first << " = " << (*it).second << "\n";
        }
    }

    void save(string fileName)
    {
        ofstream file;
        file.open (fileName.c_str(),ios::out );
        save(file);
        file.close();
    }
};




} //namespace corecvs
#endif /* PROPERTY_LIST_H_ */

