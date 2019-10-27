#include <iostream>
#include "joystickInterface.h"

namespace corecvs {

using namespace std;

void JoystickConfiguration::print() {
    cout << "Joystick: " << name << endl;
    cout << "Version: " << version << endl;

    cout << "Axis    :" << axisNumber << endl;
    cout << "Buttons :" << buttonNumber << endl;
}


} // namespace corecvs
