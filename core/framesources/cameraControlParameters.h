#pragma once

#include <string>
#include <vector>

#include "imageCaptureInterface.h"

using std::string;
using std::vector;

class CaptureParameter
{
public:
    CaptureParameter()
    : mActive(false)
    , mAutoSupported(false)
    , mMaximum(0)
    , mDefaultValue(0)
    , mMinimum(0)
    , mStep(0)
    , mIsMenu(false)
    {}

    CaptureParameter(
       bool autoSupported,
       int maximum,
       int defaultValue,
       int minimum,
       int step
    ) :
      mActive (true)
    , mAutoSupported(autoSupported)
    , mMaximum(maximum)
    , mDefaultValue(defaultValue)
    , mMinimum(minimum)
    , mStep(step)
    , mIsMenu(false)
    {}


    int           maximum() const                          { return mMaximum;    }
    void          setMaximum(int const &maximum)           { mMaximum = maximum; }

    int           defaultValue() const                     { return mDefaultValue;         }
    void          setDefaultValue(int const &defaultValue) { mDefaultValue = defaultValue; }

    int           minimum() const                          { return mMinimum;    }
    void          setMinimum(int const &minimum)           { mMinimum = minimum; }

    int           step() const                             { return mStep; }
    void          setStep(int const &step)                 { mStep = step; }

    bool          active() const                           { return mActive;   }
    void          setActive(bool active)                   { mActive = active; }

    bool          isMenu() const                           { return mIsMenu;   }
    void          setIsMenu(bool isMenu)                   { mIsMenu = isMenu; }

    bool          autoSupported() const                    { return mAutoSupported;          }
    void          setAutoSupported(bool autoSupported)     { mAutoSupported = autoSupported; }

    bool          hasMenuItems() const                     { return getMenuItemNumber() != 0;    }
    unsigned      getMenuItemNumber() const                { return (unsigned)mMenuItems.size(); }
    const string& getMenuItem (int index) const            { return mMenuItems[index]; }
    int           getMenuValue(int index) const            { return mMenuValue[index]; }

    void pushMenuItem(const string& name, int value)
    {
        mMenuItems.push_back(name);
        mMenuValue.push_back(value);
    }

    //bool operator==(CaptureParameter const &other);

private:
    bool mActive;
    bool mAutoSupported;
    int  mMaximum;
    int  mDefaultValue;
    int  mMinimum;
    int  mStep;
    bool mIsMenu;
    vector<string> mMenuItems;
    vector<int>    mMenuValue;
};

class CameraParameters
{
public:
    enum CameraControls {
        FIRST,
        EXPOSURE = FIRST,
        EXPOSURE_AUTO,
        EXPOSURE_REL,
        GAIN,
        GAIN_MULTIPLIER,
        GAIN_DIGITAL,
        GAIN_AUTO,
        GAIN_BOOST,
        GAIN_RED,
        GAIN_RED_MULTIPLIER,
        GAIN_RED_DIGITAL,
        GAIN_GREEN,
        GAIN_GREEN_MULTIPLIER,
        GAIN_GREEN_DIGITAL,
        GAIN_BLUE,
        GAIN_BLUE_MULTIPLIER,
        GAIN_BLUE_DIGITAL,
        GAIN_GREEN2,
        GAIN_GREEN2_MULTIPLIER,
        GAIN_GREEN2_DIGITAL,
        IRIS,
        FPS,
        FOCUS,
        ZOOM,
        PIXEL_CLOCK,
        HUE,
        SATURATION,
        SHARPNESS,
        BRIGHTNESS,
        CONTRAST,
        GAMMA,
        WHITE_BALANCE,
        AUTO_WHITE_BALANCE,
        POWER_LINE_FREQUENCY,
        BACKLIGHT_COMPENSATION,

        FOCUS_RELATIVE,
        AUTO_FOCUS,
        /* SOME UEYE SPECIFIC STUFF */

        MASTER_FLASH_DELAY,
        MASTER_FLASH_DURATION,
        SLAVE_TRIGGER_DELAY,

        HORIZONTAL_FLIP,
        VERTICAL_FLIP,

        SHUTTER_WIDTH_U,
        SHUTTER_WIDTH_L,
        SHUTTER_DEL,

        LAST
    };

    static const char *names[];
    static const char *getName(CameraControls control) { return names[control]; }

    CameraParameters() {}

    CaptureParameter mCameraControls[LAST];
};
