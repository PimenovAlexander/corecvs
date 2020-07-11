#include "libgifFileReader.h"
#include "core/utils/utils.h" // HelperUtils

#include <string>
#include <cstdio>
#include <cstdlib>
#include <gif_lib.h>

using namespace corecvs;

string  LibgifFileReader::prefix1(".gif");
string  LibgifFileReader::prefix2(".GIF");


#define D_GIF_SUCCEEDED          0
#define D_GIF_ERR_OPEN_FAILED    101    /* And DGif possible errors. */
#define D_GIF_ERR_READ_FAILED    102
#define D_GIF_ERR_NOT_GIF_FILE   103
#define D_GIF_ERR_NO_SCRN_DSCR   104
#define D_GIF_ERR_NO_IMAG_DSCR   105
#define D_GIF_ERR_NO_COLOR_MAP   106
#define D_GIF_ERR_WRONG_RECORD   107
#define D_GIF_ERR_DATA_TOO_BIG   108
#define D_GIF_ERR_NOT_ENOUGH_MEM 109
#define D_GIF_ERR_CLOSE_FAILED   110
#define D_GIF_ERR_NOT_READABLE   111
#define D_GIF_ERR_IMAGE_DEFECT   112
#define D_GIF_ERR_EOF_TOO_SOON   113

const char *LibgifFileReader::getErrorName(int value)
{
    switch (value)
    {

        case D_GIF_SUCCEEDED:          return "D_GIF_SUCCEEDED";
        case D_GIF_ERR_OPEN_FAILED:    return "D_GIF_ERR_OPEN_FAILED";
        case D_GIF_ERR_READ_FAILED:    return "D_GIF_ERR_READ_FAILED";
        case D_GIF_ERR_NOT_GIF_FILE:   return "D_GIF_ERR_NOT_GIF_FILE";
        case D_GIF_ERR_NO_SCRN_DSCR:   return "D_GIF_ERR_NO_SCRN_DSCR";

        case D_GIF_ERR_NO_IMAG_DSCR:   return "D_GIF_ERR_NO_IMAG_DSCR";
        case D_GIF_ERR_NO_COLOR_MAP:   return "D_GIF_ERR_NO_COLOR_MAP";
        case D_GIF_ERR_WRONG_RECORD:   return "D_GIF_ERR_WRONG_RECORD";
        case D_GIF_ERR_DATA_TOO_BIG:   return "D_GIF_ERR_DATA_TOO_BIG";
        case D_GIF_ERR_NOT_ENOUGH_MEM: return "D_GIF_ERR_NOT_ENOUGH_MEM";

        case D_GIF_ERR_CLOSE_FAILED:   return "D_GIF_ERR_CLOSE_FAILED";
        case D_GIF_ERR_NOT_READABLE:   return "D_GIF_ERR_NOT_READABLE";
        case D_GIF_ERR_IMAGE_DEFECT:   return "D_GIF_ERR_IMAGE_DEFECT";
        case D_GIF_ERR_EOF_TOO_SOON:   return "D_GIF_ERR_EOF_TOO_SOON";

        default : return "Not in range"; break ;
    }
    return "Not in range";
}

bool LibgifFileReader::acceptsFile(const string & name)
{
    SYNC_PRINT(("LibgifFileReader::acceptsFile(%s): called\n", name.c_str()));
    return HelperUtils::endsWith(name, prefix1) ||
           HelperUtils::endsWith(name, prefix2);
}

RGB24Buffer *LibgifFileReader::load(const string & name)
{
    SYNC_PRINT(("LibgifFileReader::load(%s): called\n", name.c_str()));

    RGB24Buffer *toReturn = NULL;
    int error = 0;
    GifFileType *gif = DGifOpenFileName(name.c_str(), &error);
    if (gif == NULL) {
        SYNC_PRINT(("Unable to open file <%s>\n", name.c_str()));
        return NULL;
    }

    int res = DGifSlurp(gif);
    if (res != GIF_OK) {
        SYNC_PRINT(("Unable to parse gif file <%s>\n", name.c_str()));
        DGifCloseFile(gif, &error);
        return 0;
    }

    SYNC_PRINT(("Opened gif <%s>\n", name.c_str()));
    SYNC_PRINT(("  %d images of size [%d x %d]\n", gif->ImageCount, gif->SWidth, gif->SHeight));
    SYNC_PRINT(("  color resolution: %d\n", gif->SColorResolution));
    SYNC_PRINT(("  background color: %d\n", gif->SBackGroundColor));

    SYNC_PRINT(("  extention blocks: %d\n", gif->ExtensionBlockCount));

    RGBColor *palette = NULL;

    if (gif->SColorMap != NULL)
    {
        palette = new RGBColor[gif->SColorMap->ColorCount];

        SYNC_PRINT(("  colors:%d bpp:%d\n", gif->SColorMap->ColorCount, gif->SColorMap->BitsPerPixel));
        for (int i = 0; i < gif->SColorMap->ColorCount; i++) {
            int r = gif->SColorMap->Colors[i].Red;
            int g = gif->SColorMap->Colors[i].Green;
            int b = gif->SColorMap->Colors[i].Blue;

            palette[i] = RGBColor(r,g,b);
            cout << i << ":" << r << " " << g << " " << b << " " << endl;
        }
    }

    SavedImage* gifFrame = gif->SavedImages + 0;

    toReturn = new RGB24Buffer(gif->SHeight, gif->SWidth);

    for (int y = 0; y < toReturn->h; y++)
    {
        uint8_t  *in   = (uint8_t*)(gifFrame->RasterBits) + gif->SWidth * y;
        RGBColor *dest = &toReturn->element(y,0);

        for (int x = 0; x < toReturn->w; x++)
        {
            *dest = palette[*in];

            //cout << (int)in[0] << " " << (int)in[1] << " " << (int)in[2] << " ";
            in++;
            dest++;
        }
    }



    DGifCloseFile(gif, &error);
    return toReturn;
}


bool LibgifFileReader::save(const string &name, const RGB24Buffer *buffer, int quality, bool alpha)
{
    return saveGIF(name, buffer, quality, alpha);
}

bool LibgifFileReader::saveGIF(const string &name, const RGB24Buffer *buffer, int quality, bool alpha)
{

    ColorMapObject *cmap = GifMakeMapObject(256, NULL);

    if (NULL == cmap){
        SYNC_PRINT(("cmap is NULL"));
        return false;
    }
/*
    int res = GifQuantizeBuffer(
            buffer->w, buffer->h, &_colors,
           redBuff, greenBuff, blueBuff,
           gifimgbuf, cmap->Colors);
*/


    /*if (res == GIF_ERROR)
    {
        GifFreeMapObject(cmap);
        SYNC_PRINT(("Error quantising\n"));
        return false;
    }*/

    return false;
}


RuntimeTypeBuffer *LibpgifRuntimeTypeBufferLoader::load(const string &name)
{
    RGB24Buffer *tmp = LibgifFileReader().load(name);
    if (tmp == nullptr)
        return nullptr;

    RuntimeTypeBuffer *result = new RuntimeTypeBuffer(tmp->h, tmp->w, BufferType::U8);
    
    for (RGB24Buffer::InternalIndexType y = 0; y < tmp->h; ++y)
    {
        for (RGB24Buffer::InternalIndexType x = 0; x < tmp->w; ++x)
        {
            RGBColor pixel = tmp->element(y, x);
            result->at<uint8_t>(y, x) = pixel.brightness();
        }
    }
    delete_safe(tmp);
    return result;
}


int GifEncoder::startEncoding(const std::string &name, int h, int w, int codec_id)
{

}

void GifEncoder::addFrame(RGB24Buffer *)
{

}

void GifEncoder::endEncoding()
{

}
