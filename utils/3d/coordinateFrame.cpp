/**
 * \file coordinateFrame.cpp
 *
 * \date Dec 1, 2012
 **/

#include "coordinateFrame.h"
#include "opengl/openGLTools.h"

CoordinateFrame::CoordinateFrame()
{
    type = Scene3D::COORDINATE_FRAME;
}

CoordinateFrame::~CoordinateFrame()
{
}

void CoordinateFrame::prepareMesh(CloudViewDialog* dialog)
{
    for (unsigned i = 0; i < mChildren.size(); i++)
    {
          mChildren[i]->prepareMesh(dialog);
    }

}

void CoordinateFrame::drawMyself(CloudViewDialog* dialog)
{    

    //qDebug("Drawing coordinate frame");
    //cout << "CoordinateFrame::drawMyself:" << std::endl;
    //cout << OpenGLTools::glGetProjectionMatrix() << std::endl;

    if (mParameters.style() != Draw3dStyle::POINTS) {
        OpenGLTools::drawOrts(10.0, 1.0);
    }

    for (unsigned i = 0; i < mChildren.size(); i++)
    {
            mChildren[i]->draw(dialog);
    }

}

void CoordinateFrame::dumpPCD(ostream& /*out*/)
{
}

void CoordinateFrame::dumpPLY(ostream& /*out*/)
{
}
