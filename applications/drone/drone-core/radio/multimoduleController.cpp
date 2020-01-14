#include "multimoduleController.h"

#if USE_QT
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#endif

#include <bitset>
#include <iostream>
#include <sstream>
#include <thread>
#include <memory.h>

#include "core/utils/global.h"
#include "core/utils/utils.h"
#include "core/filesystem/folderScanner.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <asm/ioctls.h>
#include <asm/termbits.h>

using namespace std;
using namespace corecvs;

MultimoduleController::MultimoduleController()
{  
}

#define PRINT_HELPER(X, Y)  if (tty.X & Y) { cout << "  " #Y << endl; }

void print(const struct termios2 &tty)
{
  cout << "IFlags" << "[" << tty.c_iflag << "]" << endl;		/* input mode flags */
      if (tty.c_iflag & IGNBRK) { cout << "  IGNBRK" << endl; }
      if (tty.c_iflag & BRKINT) { cout << "  BRKINT" << endl; }
      if (tty.c_iflag & IGNPAR) { cout << "  IGNPAR" << endl; }
      if (tty.c_iflag & PARMRK) { cout << "  PARMRK" << endl; }
      if (tty.c_iflag & INPCK)  { cout << "  INPCK" << endl; }
      if (tty.c_iflag & ISTRIP) { cout << "  ISTRIP" << endl; }

      if (tty.c_iflag & INLCR) { cout << "  INLCR" << endl; }
      if (tty.c_iflag & IGNCR) { cout << "  IGNCR" << endl; }
      if (tty.c_iflag & ICRNL) { cout << "  ICRNL" << endl; }
      if (tty.c_iflag & IUCLC) { cout << "  IUCLC" << endl; }
      if (tty.c_iflag & IXON)  { cout << "  IXON" << endl; }
      if (tty.c_iflag & IXANY) { cout << "  IXANY" << endl; }

      if (tty.c_iflag & IXOFF)   { cout << "  IXOFF" << endl; }
      if (tty.c_iflag & IMAXBEL) { cout << "  IMAXBEL" << endl; }
      if (tty.c_iflag & IUTF8)   { cout << "  IUTF8" << endl; }

  cout << "OFlags" << "[" << tty.c_oflag << "]" << endl;		/* output mode flags */
      if (tty.c_oflag & OPOST) { cout << "  OPOST" << endl; }
      if (tty.c_oflag & OLCUC) { cout << "  OLCUC" << endl; }
      if (tty.c_oflag & ONLCR) { cout << "  ONLCR" << endl; }
      if (tty.c_oflag & OCRNL) { cout << "  OCRNL" << endl; }
      if (tty.c_oflag & ONOCR) { cout << "  ONOCR" << endl; }
      if (tty.c_oflag & ONLRET){ cout << "  ONLRET" << endl; }
      if (tty.c_oflag & OFILL) { cout << "  OFILL" << endl; }
      if (tty.c_oflag & OFDEL) { cout << "  OFDEL" << endl; }

  cout << "CFlags" << "[" << hex << tty.c_cflag << dec << "]" << endl;     /* control mode flags */
      PRINT_HELPER(c_cflag, B0)
      PRINT_HELPER(c_cflag, B75)
      PRINT_HELPER(c_cflag, B110)
      PRINT_HELPER(c_cflag, B134)

      PRINT_HELPER(c_cflag, B150)
      PRINT_HELPER(c_cflag, B200)
      PRINT_HELPER(c_cflag, B300)
      PRINT_HELPER(c_cflag, B600)

      PRINT_HELPER(c_cflag, B1200)
      PRINT_HELPER(c_cflag, B1800)
      PRINT_HELPER(c_cflag, B2400)
      PRINT_HELPER(c_cflag, B4800)

      PRINT_HELPER(c_cflag, B9600)
      PRINT_HELPER(c_cflag, B19200)
      PRINT_HELPER(c_cflag, B38400)

      cout << "  Bits:" << hex << CSIZE << " " <<  (tty.c_cflag & CSIZE) << dec << endl;
      if ((tty.c_cflag & CSIZE) == CS5) { cout << "   CS5" << endl; }
      if ((tty.c_cflag & CSIZE) == CS6) { cout << "   CS6" << endl; }
      if ((tty.c_cflag & CSIZE) == CS7) { cout << "   CS7" << endl; }
      if ((tty.c_cflag & CSIZE) == CS8) { cout << "   CS8" << endl; }

      PRINT_HELPER(c_cflag, CSTOPB)
      PRINT_HELPER(c_cflag, CREAD)
      PRINT_HELPER(c_cflag, PARENB)
      PRINT_HELPER(c_cflag, PARODD)
      PRINT_HELPER(c_cflag, HUPCL)
      PRINT_HELPER(c_cflag, CLOCAL)

      PRINT_HELPER(c_cflag, B57600)
      PRINT_HELPER(c_cflag, B115200)
      PRINT_HELPER(c_cflag, B230400)
      PRINT_HELPER(c_cflag, B460800)

      PRINT_HELPER(c_cflag, B500000)
      PRINT_HELPER(c_cflag, B576000)
      PRINT_HELPER(c_cflag, B921600)
      PRINT_HELPER(c_cflag, B1000000)

      PRINT_HELPER(c_cflag, B1152000)
      PRINT_HELPER(c_cflag, B1500000)
      PRINT_HELPER(c_cflag, B2000000)
      PRINT_HELPER(c_cflag, B2500000)

      PRINT_HELPER(c_cflag, B3000000)
      PRINT_HELPER(c_cflag, B3500000)
      PRINT_HELPER(c_cflag, B4000000)

  cout << "CLine" << "[" << tty.c_line << "]" << endl;		/* line discipline */

  // cout << cc_t c_cc[NCCS];		/* control characters */
  cout << "ISpeed" << "[" << tty.c_ispeed << "]" << endl;		/* input speed */
  cout << "OSpeed" << "[" << tty.c_ospeed << "]" << endl;		/* output speed */
}

bool MultimoduleController::connectToModule(const std::string &device)
{
    if (mSpinThread != NULL) {
        SYNC_PRINT(("Already running\n"));
        return false;
    }

    serialPort = open(device.c_str(), O_RDWR);
    struct termios2 tty;
    memset (&tty, 0, sizeof(tty));

#if 0
    if ( if(ioctl(serialPort, TCGETS2, &tty)) {
         cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << endl;
         return;
    }
#endif

    cout << "Settings before setup" << endl;
    print(tty);

    /* Set Baud Rate */
    //cfsetospeed (&tty, B100000);
    //cfsetispeed (&tty, B1000000);
    tty.c_ispeed = 100000;
    tty.c_ospeed = 100000;

    // 8N1
    tty.c_cflag     |=  PARENB;
    tty.c_cflag     &=  ~PARODD;
    tty.c_cflag     |=  CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;
    tty.c_cflag     &=  ~CRTSCTS;       // no flow control

    tty.c_cflag     |=  BOTHER;

#if 1
    tty.c_lflag     =   0;          // no signaling chars, no echo, no canonical processing
    tty.c_oflag     =   0;                  // no remapping, no delays
    tty.c_cc[VMIN]      =   0;                  // read doesn't block
    tty.c_cc[VTIME]     =   5;                  // 0.5 seconds read timeout


    tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines
    tty.c_iflag     &=  ~(IXON | IXOFF | IXANY);// turn off s/w flow ctrl
    tty.c_lflag     &=  ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    tty.c_oflag     &=  ~OPOST;              // make raw
#endif

    cout << "Settings after setup" << endl;
    print(tty);
    /* Flush Port, then applies attributes */
    fsync(serialPort);

    if ( ioctl( serialPort, TCSETS2, &tty ) != 0)
    {
        cout << "Error " << errno << " from ioctl" << endl;
        return false;
    }

    timeToStop = false;
    mSpinThread = new std::thread(&MultimoduleController::run, this);

#if 0
    QString serialPortName = QString::fromStdString(device);
    serialPort.setPortName(serialPortName);
    serialPort.setParity(QSerialPort::EvenParity);
    const int serialPortBaudRate = QSerialPort::BaudRate(100000);
    serialPort.setBaudRate(serialPortBaudRate);
    serialPort.setStopBits(QSerialPort::StopBits(2));
    if (!serialPort.open(QIODevice::ReadWrite)) {
        cout <<"Failed to open port, please check that /dev/ttyUSB0 exists" << endl;
    } else {
        mSpinThread = new std::thread(&MultimoduleController::run, this);
    }
#endif
    return true;
}

bool MultimoduleController::disconnect()
{
    if (mSpinThread == NULL) {
        return false;
    }

    timeToStop = true;
    usleep(300);
    mSpinThread->join();
    delete_safe(mSpinThread);
    SYNC_PRINT(("MultimoduleController::disconnect(): disconnected\n"));
    return true;
}

void MultimoduleController::newData(const CopterInputs &input)
{
    channelsProtect.lock();
    channels = input;
    channelsProtect.unlock();
}



std::vector<string> MultimoduleController::getDevices(const string &prefix)
{
    vector<string> toReturn;

    static const int maxDeviceId = 5;

    for (int i = 0; i < maxDeviceId; i ++)
    {
        /* Stupid C++99 */
        std::stringstream ss;
        ss << i;
        std::string dev = prefix + ss.str();

        if (FolderScanner::pathExists(dev))
        {
           toReturn.push_back(dev);
        }
    }
    return toReturn;
}

void MultimoduleController::run()
{
    SYNC_PRINT(("MultimoduleController::run(): started\n"));
    while (!timeToStop)
    {
        vector<uint8_t> message(FrSkyMultimodule::MESSAGE_SIZE, 0);
        channelsProtect.lock();
        FrSkyMultimodule().pack(channels, message.data());
        channelsProtect.unlock();
        sendOurValues(message);
        usleep(8000);
        //cout << "#";
    }
    SYNC_PRINT(("MultimoduleController::run(): stopped\n"));
}

void MultimoduleController::sendOurValues(std::vector<uint8_t> ourValues)
{
    size_t written = write  (serialPort, (const char *)ourValues.data(), ourValues.size());
    fsync(serialPort);
}


