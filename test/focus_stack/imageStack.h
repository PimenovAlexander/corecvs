#ifndef IMAGESTACK_H
#define IMAGESTACK_H

#include "core/buffers/rgb24/rgb24Buffer.h"
#include "FSAlgorithm.h"

using namespace corecvs;
using namespace std;

class ImageStack
{
public:
    ImageStack(ImageStack * imageStack);
    ~ImageStack();
    bool addImageToStack(RGB24Buffer & image);
    bool removeImageFromStack(int index);
    void saveStack(string pathToDir);
    void saveMegredImage(string pathToDir);
    static pair<int, int> getDimensions(RGB24Buffer & image);
    static ImageStack * loadStack(vector<RGB24Buffer*> images);
    static ImageStack * loadStack(string pathToFolder, int amountOfImages);
    void focus_stack(FSAlgorithm & algo);
private:
    ImageStack(pair<int, int> dimensions);
    bool checkDimensions(RGB24Buffer & image);
    static bool checkDimensions(RGB24Buffer & image, pair<int, int> & dimensions);
    vector<RGB24Buffer*> imageStack;
    int height;
    int width;
    RGB24Buffer * mergedImage;
};

#endif // IMAGESTACK_H
