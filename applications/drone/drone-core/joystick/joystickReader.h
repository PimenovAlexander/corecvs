#ifndef JOYSTICKREADER_H
#define JOYSTICKREADER_H

#include "joystickInterface.h"

#include <QObject>

class JoystickReader : public JoystickInterface
{

private:

    QString deviceName = "/dev/input/js0";

public:
    JoystickReader(const std::string &deviceName)  ;
    ~JoystickReader();
signals:

public slots:


};


#endif // JOYSTICKREADER_H
