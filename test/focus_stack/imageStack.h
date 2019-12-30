#ifndef IMAGESTACK_H
#define IMAGESTACK_H

#include "core/buffers/rgb24/rgb24Buffer.h"
#include "fsAlgorithm.h"

using namespace corecvs;
using namespace std;

/**
    This class represents a stack of images taken with different focus distance.

**/

class ImageStack
{
public:
    ImageStack(ImageStack * imageStack);
    ~ImageStack();
    /**
    *    Add image to stack, creating a copy of it if image size are the same as size of other images in stack.
    *    @param image Image which should be added to stack.
    *    @return True if image was added successfully, otherwise false.
    **/
    bool addImageToStack(RGB24Buffer & image);
    /**
    *    Remove image from stack.
    *    @param index Index of image in stack that should be removed.
    *    @return True if image was removed successfully, otherwise false.
    **/
    bool removeImageFromStack(int index);
    /**
    *    Save stack to directory. Bmp format is used.
    *    @param pathToDir Directory where the stack should be saved.
    **/
    void saveStack(string pathToDir);
    /**
    *    Save merged image to directory. Bmp format is used.
    *    @param pathToDir Directory where image should be saved.
    **/
    void saveMergedImage(string pathToDir);
    /**
    *    Load stack from vector of images creating copies of them.
    *    @param images Vector from which images should be loaded to stack.
    *    @return Loaded stack.
    **/
    static ImageStack * loadStack(vector<RGB24Buffer*> images);
    /**
    *    Load stack from directory. This metthod will try to load all images in directory as one stack.
    *    @param pathToFolder Directory from which the stack should be loaded.
    *    @return Loaded stack.
    **/
    static ImageStack * loadStack(const string &pathToFolder);
    /**
    *    Run focus stacking and save result to merged image.
    *    @param algo FS algorithm that should be used.
    **/
    void focus_stack(FSAlgorithm & algo);
private:
    ImageStack(pair<int, int> dimensions);
    bool checkDimensions(RGB24Buffer & image);
    /**
    *    Check if image has same dimensions as provided in the second argument.
    *    @param image Image for checking dimensions
    *    @param dimensions Dimensions which are compared with image ones.
    *    @return True if dimenensions are equal, false otherwise.
    **/
    static bool checkDimensions(RGB24Buffer & image, pair<int, int> & dimensions);
    /**
    *    Returns the dimensions of the image.
    *    @param image Image which dimensions should be returned.
    *    @return Pair with image dimensions.
    **/
    static pair<int, int> getDimensions(RGB24Buffer & image);
    /**
    *    Vector for storing images of stack.
    **/
    vector<RGB24Buffer*> imageStack;
    int height;
    int width;
    /**
    *    Image that contains the result of some algorithms which use the whole stack.
    **/
    RGB24Buffer * mergedImage;
};

#endif // IMAGESTACK_H
