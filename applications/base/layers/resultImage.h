#pragma once
/**
 * \file resultImage.h
 *
 * \date Oct 17, 2012
 **/

#include <vector>
#include <QtGui/QImage>

#include "global.h"


using std::vector;

class ResultLayerBase
{
public:
    enum ResultLayerType {
        LAYER_IMAGE,
        LAYER_FLOW,
        LAYER_STEREO,
        LAYER_TILES,
        LAYER_CLUSTERS,
        LAYER_OBJECT,
        LAYER_PARAMS
    };

    static const char* LAYER_TYPE_NAMES[];

    ResultLayerType getType() const {
    	return mType;
    }

    virtual void drawImage    (QImage * /*image*/) {}
    virtual int  modifyWidth  (int width)  { return width;  }
    virtual int  modifyHeight (int height) { return height; }

    virtual ~ResultLayerBase() {}

protected:
    ResultLayerBase(ResultLayerType type) :
        mType(type)
    {}

private:
    ResultLayerType mType;
};



class ResultLayerStereo : public ResultLayerBase {
    ResultLayerStereo() : ResultLayerBase(ResultLayerBase::LAYER_STEREO) {}


};

class ResultLayerFlow : public ResultLayerBase {
    ResultLayerFlow() : ResultLayerBase(ResultLayerBase::LAYER_FLOW) {}


};



class ResultImage
{
    vector<ResultLayerBase *> mLayers;

    int mHeight;
    int mWidth;

public:
    ResultImage() :
    	mHeight(0)
    ,   mWidth (0)
    {}

    virtual ~ResultImage()
    {
        for (unsigned i = 0; i < mLayers.size(); i++)
        {
            delete_safe(mLayers[i]);
        }
    }

    void drawImage (QImage *image)
    {
        if (image == NULL)
            return;

        for (unsigned i = 0; i < mLayers.size(); i++)
        {
            if (mLayers[i] != NULL)
            {
//            	qDebug("Applying layer %d of type %d", i, mLayers[i]->getType());
                mLayers[i]->drawImage(image);
            } else {
//            	qDebug("Layer %d is NULL", i);
            }
        }
    }

    unsigned layerNum() const
    {
    	return (unsigned)mLayers.size();
    }

    const ResultLayerBase *layer(unsigned number) const
    {
    	return mLayers[number];
    }

    void addLayer(ResultLayerBase *layer)
    {
        mLayers.push_back(layer);
    }

    int height() const
    {
        int maxHeight = mHeight;

        for (unsigned i = 0; i < mLayers.size(); i++)
        {
            if (mLayers[i] == NULL)
                continue;

            int newHeight = mLayers[i]->modifyHeight(mHeight);
            maxHeight = std::max(newHeight, maxHeight);
        }

        return maxHeight;
    }

    int width() const
    {
        int maxWidth = mWidth;
        for (unsigned i = 0; i < mLayers.size(); i++)
        {
            if (mLayers[i] == NULL)
               continue;

            int newWidth = mLayers[i]->modifyWidth(mWidth);
            maxWidth = std::max(newWidth, maxWidth);
        }

        return maxWidth;
    }

    void setHeight(int height) { mHeight = height; }

    void setWidth(int width)   { mWidth = width;   }

    void print ()
    {
    	printf("Result Image: %d layers", layerNum());
    	for (unsigned i = 0; i < layerNum(); i++)
    	{
        	printf("  %s\n", ResultLayerBase::LAYER_TYPE_NAMES[layer(i)->getType()]);
    	}
    }

};

/* EOF */
