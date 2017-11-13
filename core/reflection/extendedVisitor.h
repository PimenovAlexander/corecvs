#ifndef EXTENDEDVISITOR_H
#define EXTENDEDVISITOR_H
/**
 *
 *  Library has both static and dynamic polimorphism serializers.
 *
 *  Former is implemented in classes using accept template method
 *  Latter with the interface below.
 *
 **/

#include <string>

namespace corecvs {

class CreatorFabric {

    /**
     *  Enjoy your static desctructor. Leeks and pain are inevitable, but that's how we roll
     **/
    static void *createObject(std::string &name);

};


class Visitable {



};

/**
 *
 *  Base class for visitors that go beyond simple template based functions
 **/
class ExtendedVisitor
{
public:
    ExtendedVisitor();

};


} // namespace

#endif // EXTENDEDVISITOR_H
