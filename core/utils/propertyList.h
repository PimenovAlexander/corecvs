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

#include "core/utils/global.h"
#include "core/utils/utils.h"

namespace corecvs {

class PropertyListVectorContainer : public std::vector<std::pair<std::string, std::string>>
{
protected:
    PropertyListVectorContainer::iterator find(std::string name)
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

    void insert(std::pair<std::string, std::string> pair)
    {
        push_back(pair);
    }
};

class PropertyList : public PropertyListVectorContainer
{
private:
template<typename ValueType>
    ValueType getTypedProperty(std::string name, ValueType defaultValue)
    {
        std::string* value = getProperty(name);
        if (value == NULL)
            return defaultValue;
        std::istringstream i(*value);
        ValueType result = defaultValue;
        i >> result;
        return result;
    }

template<typename ValueType>
    void setTypedProperty(std::string name, ValueType value)
    {
        PropertyList::iterator it = this->find(name);
        std::ostringstream o;
        o.precision(std::numeric_limits<double>::digits10 + 3);
        o << value;
        std::string tosave = o.str();

        if (it == this->end())
        {
            insert(std::pair<std::string, std::string>(name, tosave));
            return;
        }
        (*it).second = value;
    }

public:
    std::string *getProperty(std::string name)
    {
         PropertyList::iterator it = this->find(name);
        if (it == this->end())
            return NULL;
        else
            return &((*it).second);
    }


    int getIntProperty(std::string name, int defaultValue)
    {
        return getTypedProperty<int>(name, defaultValue);
    }

    double getDoubleProperty(std::string name, double defaultValue)
    {
        return getTypedProperty<double>(name, defaultValue);
    }

    std::string getStringProperty(std::string name, std::string defaultValue)
    {
        return getTypedProperty<std::string>(name, defaultValue);
    }


    void setIntProperty(std::string name, int value)
    {
        setTypedProperty<int>(name, value);
    }

    void setDoubleProperty(std::string name, double value)
    {
        setTypedProperty<double>(name, value);
    }

    void setStringProperty(std::string name, std::string value)
    {
        setTypedProperty<std::string>(name, value);
    }

    /* Fix the limited line length */
    int load(std::istream &inStream)
    {
        std::string line;

        while (!inStream.eof())
        {
            corecvs::HelperUtils::getlineSafe(inStream, line);
            std::istringstream streamLine(line);
            //cout << "Parsing:" << line << endl;

            std::string name, dummy, value;
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

    int load(std::string fileName)
    {
        std::ifstream file;
        file.open(fileName.c_str(), std::ios::in);
        std::cout << "Opening:" << fileName << std::endl;
        if (file.fail())
        {
            std::cout << "Can't open property list file" << std::endl;
            return false;
        }

        int result = load(file);
        file.close();
        return result;
    }

    void save(std::ostream &outStream)
    {
        outStream << "#There are " << size() << " parameters\n";
        PropertyList::iterator it;
        for (it = this->begin(); it != this->end(); it++)
        {
            outStream << (*it).first << " = " << (*it).second << "\n";
        }
    }

    void save(std::string fileName)
    {
        std::ofstream file;
        file.open(fileName.c_str(), std::ios::out);
        save(file);
        file.close();
    }
};

} //namespace corecvs

#endif /* PROPERTY_LIST_H_ */
