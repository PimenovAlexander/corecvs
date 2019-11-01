#include "joystickReader.h"
#include "iostream"

JoystickReader::JoystickReader(const std::string &deviceName) :
    LinuxJoystickInterface (deviceName)
{
    std::cout<<"JS Reader created"<<std::endl;
}


JoystickReader::~JoystickReader()
{

}
