#ifndef JOYSTICKREADER_H
#define JOYSTICKREADER_H

#include <QObject>

#include "linuxJoystickInterface.h"


class JoystickReader : public LinuxJoystickInterface
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
