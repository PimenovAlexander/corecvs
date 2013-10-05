#ifndef BASEPATHVISITOR_H_
#define BASEPATHVISITOR_H_
/**
 * \file basePathVisitor.h
 * \brief Add Comment Here
 *
 * \date Nov 27, 2011
 * \author alexander
 */
#include <deque>
#include <string>

namespace corecvs {

using std::string;
using std::deque;

/**
 *  Base class for visitors that iterate over the hierarchy of types
 **/
class BasePathVisitor : public deque<string>
{
public:
    static string SEPARATOR_DEFAULT;

    string separator;

    string getCurrentPath()
    {
        if (empty())
            return "";
        return back();
    }

    string getChildPath(string child)
    {
       string newPath = getCurrentPath();
       if (!newPath.empty())
           newPath.append(separator);
       newPath.append(child);
       return newPath;
    }


    BasePathVisitor(/*string root,*/ string _separator = SEPARATOR_DEFAULT) :
        separator(_separator)
    {
    }

    void pushChild (const string child)
    {
        push_back(getChildPath(child));
    }

    void popChild ()
    {
        pop_back();
    }

};


} //namespace corecvs
#endif // BASEPATHVISITOR


