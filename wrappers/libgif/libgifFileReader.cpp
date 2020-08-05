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

const char *LibgifFileReader::getExtentionName(int value)
{
    switch (value) {
        case CONTINUE_EXT_FUNC_CODE   : return "CONTINUE_EXT_FUNC_CODE";
        case COMMENT_EXT_FUNC_CODE    : return "COMMENT_EXT_FUNC_CODE";
        case GRAPHICS_EXT_FUNC_CODE   : return "GRAPHICS_EXT_FUNC_CODE";
        case PLAINTEXT_EXT_FUNC_CODE  : return "PLAINTEXT_EXT_FUNC_CODE";
        case APPLICATION_EXT_FUNC_CODE: return "APPLICATION_EXT_FUNC_CODE";
        default: return "Not in range";
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
            //cout << i << ":" << r << " " << g << " " << b << " " << endl;
        }
    }

    SavedImage* gifFrame = gif->SavedImages + 0;

    toReturn = new RGB24Buffer(gif->SHeight, gif->SWidth);

    for(int fr = 0; fr < gif->ImageCount; fr ++)
    {
        SavedImage* gifFrame = gif->SavedImages + fr;
        SYNC_PRINT(("Info frame: %d\n", fr));
        SYNC_PRINT(("  Frame Size      : %d x %d [+%d +%d]\n",
                    gifFrame->ImageDesc.Width,
                    gifFrame->ImageDesc.Height,
                    gifFrame->ImageDesc.Left,
                    gifFrame->ImageDesc.Top
                    ));
        SYNC_PRINT(("  Frame Extentions : %d\n", gifFrame->ExtensionBlockCount));

        for (int ex = 0; ex < gifFrame->ExtensionBlockCount; ex++)
        {
            SYNC_PRINT(("    Extentions : %d\n", ex));
            ExtensionBlock *block = &gifFrame->ExtensionBlocks[ex];
            SYNC_PRINT(("    Type: %s\n",  getExtentionName(block->Function)));
            SYNC_PRINT(("    Size: %d\n", (int)block->ByteCount));
            if (block->Function == GRAPHICS_EXT_FUNC_CODE)
            {
                GraphicsControlBlock graphicsControlBlock;

                DGifExtensionToGCB(block->ByteCount, block->Bytes, &graphicsControlBlock);
                SYNC_PRINT(("     Delay            :%d\n", graphicsControlBlock.DelayTime));
                SYNC_PRINT(("     Disposal Mode    :%d\n", graphicsControlBlock.DisposalMode));
                SYNC_PRINT(("     User Input Flag  :%d\n", graphicsControlBlock.UserInputFlag));
                SYNC_PRINT(("     Transparent Color:%d\n", graphicsControlBlock.TransparentColor));
            }

            if (block->Function == APPLICATION_EXT_FUNC_CODE)
            {
                if (block->ByteCount == 11)
                {
                    SYNC_PRINT(("    Seem to be netscape 2.0 block.\n"));
                    if (!memcmp(block->Bytes, "NETSCAPE2.0", 11))
                    {
                        SYNC_PRINT(("    NETSCAPE2.0 signature found\n"));
                        if (ex < gifFrame->ExtensionBlockCount - 1)
                        {
                            ExtensionBlock *blockData = &gifFrame->ExtensionBlocks[ex + 1];
                            if (blockData->ByteCount == 3 && blockData->Bytes[0] == 0x1)
                            {
                                uint16_t loopCount = blockData->Bytes[1] | (blockData->Bytes[2] << 8);
                                SYNC_PRINT(("Loop Count: %d\n", loopCount));
                            } else {
                                SYNC_PRINT(("    Unknown data block %d\n", blockData->Bytes[0]));
                            }
                        }
                    }
                }

            }
        }
    }

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
    SYNC_PRINT(("LibgifFileReader::saveGIF(%s, [%d x %d]):called\n", name.c_str(), buffer->w, buffer->h));
    int paletteSize = 256;
    ColorMapObject *cmap = GifMakeMapObject(paletteSize, NULL);
    cmap->BitsPerPixel = 8;

    if (cmap == NULL){
        SYNC_PRINT(("cmap is NULL"));
        return false;
    }

    uint8_t *rB = new uint8_t[buffer->h * buffer->w];
    uint8_t *gB = new uint8_t[buffer->h * buffer->w];
    uint8_t *bB = new uint8_t[buffer->h * buffer->w];

    for (int i = 0; i < buffer->h; i++)
    {
        for (int j = 0; j < buffer->w; j++)
        {
            rB[i * buffer->w + j] = buffer->element(i,j).r();
            gB[i * buffer->w + j] = buffer->element(i,j).g();
            bB[i * buffer->w + j] = buffer->element(i,j).b();
        }
    }

    uint8_t *oB = new uint8_t[buffer->h * buffer->w];

    int res = GifQuantizeBuffer(
            buffer->w, buffer->h, &paletteSize,
            rB, gB, bB,
            oB, cmap->Colors);

#if 0
    for (int i = 0; i < cmap->ColorCount; i++) {
        int r = cmap->Colors[i].Red;
        int g = cmap->Colors[i].Green;
        int b = cmap->Colors[i].Blue;

        cout << i << ":" << r << " " << g << " " << b << " " << endl;
    }
#endif

    int error = 0;
    GifFileType *gif = EGifOpenFileName(name.c_str(), false, &error);
    if (gif == NULL) {
        SYNC_PRINT(("Unable to open file <%s>\n", name.c_str()));
        return false;
    }

    gif->SWidth  = buffer->w;
    gif->SHeight = buffer->h;
    gif->Image.ColorMap = NULL;
    gif->Image.Height   = buffer->h;
    gif->Image.Width    = buffer->w;
    gif->Image.Left    = 0;
    gif->Image.Top     = 0;
    gif->Image.Interlace = false;
    gif->Image.ColorMap = NULL;

    gif->SColorResolution = 8;
    gif->SBackGroundColor = 0;
    gif->SColorMap = cmap;

    SavedImage *image = GifMakeSavedImage(gif, NULL);
    if (image == NULL) {
        SYNC_PRINT(("Unable to create image in gif... \n"));
        return false;
    }
    image->RasterBits = oB;
    image->ExtensionBlockCount = 0;
    image->ImageDesc = gif->Image;

    SYNC_PRINT(("images %d\n", gif->ImageCount ));
    SYNC_PRINT(("colors %d\n", gif->SColorMap->ColorCount ));
    SYNC_PRINT(("image  %p %p\n", image, gif->SavedImages));


    EGifSetGifVersion(gif, true);

    if (EGifSpew(gif) == GIF_ERROR)
    {
        SYNC_PRINT(("Unable to write file <%s>\n", name.c_str()));
        return false;
    }

    //cout << gif->SavedImages[0].RasterBits << endl;

    deletearr_safe(rB);
    deletearr_safe(gB);
    deletearr_safe(bB);
    deletearr_safe(oB);

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
    int error = 0;
    gif = EGifOpenFileName(name.c_str(), false, &error);
    if (gif == NULL) {
        SYNC_PRINT(("Unable to open file <%s>\n", name.c_str()));
        return false;
    }

    gif->SWidth  = w;
    gif->SHeight = h;
    gif->Image.ColorMap = NULL;
    gif->Image.Height   = h;
    gif->Image.Width    = w;
    gif->Image.Left    = 0;
    gif->Image.Top     = 0;
    gif->Image.Interlace = false;
    gif->Image.ColorMap = NULL;

    gif->SColorResolution = 8;
    gif->SBackGroundColor = 0;

    gif->SColorMap = NULL;

    return 0;
}

void GifEncoder::addFrame(RGB24Buffer *frame)
{
    int paletteSize = 256;
    ColorMapObject *cmap = GifMakeMapObject(paletteSize, NULL);
    cmap->BitsPerPixel = 8;

    if (cmap == NULL){
        SYNC_PRINT(("cmap is NULL"));
        return;
    }

    uint8_t *rB = new uint8_t[frame->h * frame->w];
    uint8_t *gB = new uint8_t[frame->h * frame->w];
    uint8_t *bB = new uint8_t[frame->h * frame->w];

    for (int i = 0; i < frame->h; i++)
    {
        for (int j = 0; j < frame->w; j++)
        {
            rB[i * frame->w + j] = frame->element(i,j).r();
            gB[i * frame->w + j] = frame->element(i,j).g();
            bB[i * frame->w + j] = frame->element(i,j).b();
        }
    }

    uint8_t *oB = new uint8_t[frame->h * frame->w];

    int res = GifQuantizeBuffer(
            frame->w, frame->h, &paletteSize,
            rB, gB, bB,
            oB, cmap->Colors);

    SavedImage *image = GifMakeSavedImage(gif, NULL);
    if (image == NULL) {
        SYNC_PRINT(("Unable to create image in gif... \n"));
        return;
    }
    image->RasterBits = oB;
    image->ExtensionBlockCount = 0;
    image->ImageDesc = gif->Image;
    image->ImageDesc.ColorMap = cmap;

    GraphicsControlBlock graphicsControlBlock;
    graphicsControlBlock.DelayTime        = 20;
    graphicsControlBlock.DisposalMode     = 0;
    graphicsControlBlock.UserInputFlag    = 0;
    graphicsControlBlock.TransparentColor = -1;

    EGifGCBToSavedExtension(&graphicsControlBlock, gif, gif->ImageCount - 1);

    if (gif->ImageCount == 1) {
        gif->SColorMap = cmap;

        GifAddExtensionBlock(
            &image->ExtensionBlockCount,
            &image->ExtensionBlocks,
            APPLICATION_EXT_FUNC_CODE,
            11,
            (unsigned char *)"NETSCAPE2.0"
        );
        const unsigned char data[] = {0x1, 0x0, 0x0};
        GifAddExtensionBlock(
            &image->ExtensionBlockCount,
            &image->ExtensionBlocks,
            CONTINUE_EXT_FUNC_CODE,
            3,
            (unsigned char *)data
        );

    }

    deletearr_safe(rB);
    deletearr_safe(gB);
    deletearr_safe(bB);
}

void GifEncoder::endEncoding()
{
    SYNC_PRINT(("images %d\n", gif->ImageCount ));
    SYNC_PRINT(("colors %d\n", gif->SColorMap->ColorCount ));
    //SYNC_PRINT(("image  %p\n", gif->SavedImages));


    EGifSetGifVersion(gif, true);

    if (EGifSpew(gif) == GIF_ERROR)
    {
        SYNC_PRINT(("Unable to write file\n"));
        return;
    }

    int error = 0;
    /*GifFreeSavedImages(gif);
    EGifCloseFile(gif, &error);*/
}
