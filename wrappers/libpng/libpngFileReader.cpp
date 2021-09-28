#include "libpngFileReader.h"
#include "core/utils/utils.h" // HelperUtils

#include <string>
#include <cstdio>
#include <cstdlib>
#include <png.h>

using namespace corecvs;

string  LibpngFileReader::prefix1(".png");
string  LibpngFileReader::prefix2(".PNG");

const char *LibpngFileReader::getColorTypeName(int value)
{
    switch (value)
    {

        case PNG_COLOR_TYPE_GRAY:       return "PNG_COLOR_TYPE_GRAY";
        case PNG_COLOR_TYPE_PALETTE:    return "PNG_COLOR_TYPE_PALETTE";
        case PNG_COLOR_TYPE_RGB:        return "PNG_COLOR_TYPE_RGB";
        case PNG_COLOR_TYPE_RGB_ALPHA:  return "PNG_COLOR_TYPE_RGB_ALPHA";
        case PNG_COLOR_TYPE_GRAY_ALPHA: return "PNG_COLOR_TYPE_GRAY_ALPHA";
        default : return "Not in range"; break ;
    }
    return "Not in range";
}

bool LibpngFileReader::acceptsFile(const string & name)
{
    return HelperUtils::endsWith(name, prefix1) ||
            HelperUtils::endsWith(name, prefix2);
}

RGB24Buffer *LibpngFileReader::load(const string & name)
{
    RGB24Buffer *toReturn;
    png_byte ** row_pointers;
    FILE *fp = fopen(name.c_str(), "rb");
    const int number = 4;
    uint8_t header[number];

    if (!fp)
        return NULL;

    if (fread(header, 1, number, fp) != number)
        return NULL;

    bool is_png = !png_sig_cmp(header, 0, number);
    if (!is_png)
        return NULL;

    png_structp png_ptr = png_create_read_struct
           (PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if (!png_ptr)
        return NULL;

    png_infop info_ptr = png_create_info_struct(png_ptr);

    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr,
            nullptr, nullptr);
        return NULL;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(fp);
        return NULL;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, number);
    png_read_info(png_ptr, info_ptr);

    png_uint_32 width;
    png_uint_32 height;
    int color_type;
    int bit_depth;
    int channels  = 3;

    png_get_IHDR(png_ptr, info_ptr, &width, &height,
          &bit_depth, &color_type, nullptr,
          nullptr, nullptr);

    if (color_type != PNG_COLOR_TYPE_RGB  && color_type != PNG_COLOR_TYPE_RGB_ALPHA &&
        color_type != PNG_COLOR_TYPE_GRAY && color_type != PNG_COLOR_TYPE_GRAY_ALPHA )
    {
        SYNC_PRINT(("LibpngFileReader::load(): color type not supported (%s)\n", getColorTypeName(color_type)));
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(fp);
        return NULL;
    }

    if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
       channels = 1;
    }

    if (bit_depth != 8) {
        SYNC_PRINT(("LibpngFileReader::load(): not a 8 bit image (%d)\n", bit_depth));
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(fp);
        return NULL;
    }

    if (color_type & PNG_COLOR_MASK_ALPHA)
          png_set_strip_alpha(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    row_pointers = (png_byte **) png_malloc(png_ptr, height * sizeof (png_byte *));
    for (png_uint_32 y = 0; y < height; y++)
        row_pointers[y] = (png_byte *) png_malloc (png_ptr, sizeof (uint8_t) * width * channels);

    png_read_image(png_ptr, row_pointers);

    toReturn = new RGB24Buffer(height, width);

    if (channels == 3)
    {
        for (uint32_t y = 0; y < height; y++)
        {
            png_byte *row = row_pointers[y];
            for (uint32_t x = 0; x < width; x++)
            {
                RGBColor *dest = &toReturn->element(y,x);
                uint32_t r = *row++;
                uint32_t g = *row++;
                uint32_t b = *row++;
                *dest = RGBColor(r,g,b);
            }
        }
    } else {
        for (uint32_t y = 0; y < height; y++)
        {
            png_byte *row = row_pointers[y];
            for (uint32_t x = 0; x < width; x++)
            {
                RGBColor *dest = &toReturn->element(y,x);
                uint32_t g = *row++;
                *dest = RGBColor::Gray(g);
            }
        }
    }

    for (uint32_t y = 0; y < height; y++) {
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);

    png_read_end(png_ptr, nullptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

    return toReturn;
}

RGB48Buffer *LibpngFileReader::loadRGB48(const std::string &name)
{
    RGB48Buffer *toReturn = NULL;
    png_byte ** row_pointers;
    FILE *fp = fopen(name.c_str(), "rb");
    const int number = 4;
    uint8_t header[number];

    if (!fp)
        return NULL;

    if (fread(header, 1, number, fp) != number)
        return NULL;

    bool is_png = !png_sig_cmp(header, 0, number);
    if (!is_png)
        return NULL;

    png_structp png_ptr = png_create_read_struct
           (PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if (!png_ptr)
        return NULL;

    png_infop info_ptr = png_create_info_struct(png_ptr);

    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return NULL;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(fp);
        return NULL;
    }
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, number);
    png_read_info(png_ptr, info_ptr);

    png_uint_32 width, height;
    int color_type, bit_depth;

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, nullptr,
          nullptr, nullptr);

    if (0) {
        cout << "Bit depth  :" << bit_depth << endl;
        cout << "Width      :" << width << endl;
        cout << "Height     :" << height << endl;
        cout << "Color type :" << getColorTypeName(color_type) << endl;
    }

    if (color_type != PNG_COLOR_TYPE_RGB && color_type != PNG_COLOR_TYPE_RGB_ALPHA)
    {
        SYNC_PRINT(("LibpngFileReader::loadRGB48(): color type not supported (%s)\n", getColorTypeName(color_type)));
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(fp);
        return NULL;
    }

    if (bit_depth != 16) {
        SYNC_PRINT(("LibpngFileReader::loadRGB48(): not a 16 bit image (%d)\n", bit_depth));
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        fclose(fp);
        return NULL;
    }

    if (color_type & PNG_COLOR_MASK_ALPHA)
          png_set_strip_alpha(png_ptr);

    png_read_update_info(png_ptr, info_ptr);

    row_pointers = (png_byte **) png_malloc(png_ptr, height * sizeof (png_byte *));
    for (png_uint_32 y = 0; y < height; y++)
        row_pointers[y] = (png_byte *) png_malloc (png_ptr, sizeof (uint8_t) * width * 3 * (bit_depth / 8));

    png_read_image(png_ptr, row_pointers);

    toReturn = new RGB48Buffer(height, width);

    for (uint32_t y = 0; y < height; y++) {
        png_byte *row = (png_byte *)row_pointers[y];
        for (uint32_t x = 0; x < width; x++) {
            RGBColor48 *dest = &toReturn->element(y,x);
            uint32_t r = (uint16_t)row[1] | ((uint16_t)row[0] << 8); row += 2;
            uint32_t g = (uint16_t)row[1] | ((uint16_t)row[0] << 8); row += 2;
            uint32_t b = (uint16_t)row[1] | ((uint16_t)row[0] << 8); row += 2;

            *dest = RGBColor48(r,g,b);
        }
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);

    png_read_end(png_ptr, nullptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
    return toReturn;
}

bool LibpngFileReader::save(const string &name, const RGB24Buffer *buffer, int quality, bool alpha)
{
    return savePNG(name, buffer, quality, alpha);
}

bool LibpngFileReader::savePNG(const string &name, const RGB24Buffer *buffer, int quality, bool alpha)
{
    //CORE_UNUSED(quality); /*TODO: connect with PNG_COMPRESSION_TYPE_DEFAULT below*/
    bool toReturn = false;
    int pixel_size = alpha ? 4 : 3;
    int depth = 8;
    png_uint_32 width = buffer->w, height = buffer->h;
    png_structp png_ptr;
    png_infop info_ptr;
    png_byte **row_pointers;

    FILE *fp = fopen(name.c_str(), "wb");
    if (!fp) {
        SYNC_PRINT(("Unable to open file %s\n", name.c_str()));
        return toReturn;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) {
        goto png_create_write_struct_failed;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        goto png_create_info_struct_failed;
    }

    /* Set up error handling. */
    if (setjmp (png_jmpbuf(png_ptr))) {
        goto png_failure;
    }

    /* Set image attributes. */
    png_set_IHDR(png_ptr,
                 info_ptr,
                 width,
                 height,
                 depth,
                 alpha ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    /* Initialize rows of PNG. */
    row_pointers = (png_byte **)png_malloc(png_ptr, height * sizeof(png_byte *));
    for (uint32_t y = 0; y < height; ++y)
    {
        auto row = (png_byte *)png_malloc(png_ptr, sizeof(uint8_t) * width * pixel_size);
        row_pointers[y] = row;
        for (uint32_t x = 0; x < width; ++x)
        {
            RGBColor pixel = buffer->element(y, x);
            *row++ = pixel.r();
            *row++ = pixel.g();
            *row++ = pixel.b();
            if (alpha) {
                *row++ = pixel.a();
            }
        }
    }

    /* Write the image data to "fp". */
    png_init_io  (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    
    toReturn = true;

    for (uint32_t y = 0; y < height; ++y) {
        png_free(png_ptr, row_pointers[y]);
    }
    png_free(png_ptr, row_pointers);

    png_failure:
    png_create_info_struct_failed:
        png_destroy_write_struct(&png_ptr, &info_ptr);
    png_create_write_struct_failed:
        fclose (fp);

        return toReturn;
}

typedef struct
{
    //png_bytep data;
    //int size;

    std::vector<unsigned char> *vdata;
} ImageTarget;

void pngWriteCallback(png_structp png_ptr, png_bytep data, png_size_t length)
{
    //cout << "- length ----------- " << length << endl;
    ImageTarget * itarget = (ImageTarget*)png_get_io_ptr(png_ptr);
    size_t nsize = itarget->vdata->size() + length;
    //cout << "- nsize ----------- " << nsize << endl;
    //cout << "- data ------ " << (size_t) itarget->data << endl;

    size_t old_size = itarget->vdata->size();
    itarget->vdata->resize(nsize);
    memcpy(itarget->vdata->data() + old_size, data, length);
}

bool LibpngFileReader::savePNG(std::vector<unsigned char> &mem_vector, const RGB24Buffer *buffer, int quality, bool alpha)
{
    //CORE_UNUSED(quality); /*TODO: connect with PNG_COMPRESSION_TYPE_DEFAULT below*/
    bool toReturn = false;
    int pixel_size = alpha ? 4 : 3;
    int depth = 8;
    png_uint_32 width = buffer->w, height = buffer->h;
    png_structp png_ptr;
    png_infop info_ptr;
    png_byte **row_pointers;

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) {
        goto png_create_write_struct_failed;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        goto png_create_info_struct_failed;
    }

    /* Set up error handling. */
    if (setjmp (png_jmpbuf(png_ptr))) {
        goto png_failure;
    }

    /* Set image attributes. */
    png_set_IHDR(png_ptr,
                 info_ptr,
                 width,
                 height,
                 depth,
                 alpha ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    /* Initialize rows of PNG. */
    row_pointers = (png_byte **)png_malloc(png_ptr, height * sizeof(png_byte *));
    for (uint32_t y = 0; y < height; ++y)
    {
        auto row = (png_byte *)png_malloc(png_ptr, sizeof(uint8_t) * width * pixel_size);
        row_pointers[y] = row;
        for (uint32_t x = 0; x < width; ++x)
        {
            RGBColor pixel = buffer->element(y, x);
            *row++ = pixel.r();
            *row++ = pixel.g();
            *row++ = pixel.b();
            if (alpha) {
                *row++ = pixel.a();
            }
        }
    }

    /* Write the image data to "fp". */
    //png_init_io  (png_ptr, fp);
    ImageTarget itarget;
    mem_vector.clear();
    itarget.vdata = &mem_vector;
    png_set_write_fn(png_ptr, &itarget, pngWriteCallback, NULL);


    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    toReturn = true;

    for (uint32_t y = 0; y < height; ++y) {
        png_free(png_ptr, row_pointers[y]);
    }
    png_free(png_ptr, row_pointers);

    png_failure:
    png_create_info_struct_failed:
        png_destroy_write_struct(&png_ptr, &info_ptr);
    png_create_write_struct_failed:

    return toReturn;
}

bool LibpngFileReader::savePNG(const string &name, const RGB48Buffer *buffer, int quality, bool alpha)
{
    //CORE_UNUSED(quality); /*TODO: connect with PNG_COMPRESSION_TYPE_DEFAULT below*/
    bool toReturn = false;
    int pixel_size = alpha ? 4 : 3;
    int depth = 16;
    png_uint_32 width = buffer->w;
    png_uint_32 height = buffer->h;
    png_structp png_ptr;
    png_infop info_ptr;
    png_byte **row_pointers = NULL;

    FILE *fp = fopen(name.c_str(), "wb");
    if (!fp) {
        SYNC_PRINT(("Unable to open file %s\n", name.c_str()));
        return toReturn;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (png_ptr == nullptr) {
        goto png_create_write_struct_failed;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        goto png_create_info_struct_failed;
    }

    /* Set up error handling. */
    if (setjmp (png_jmpbuf(png_ptr))) {
        goto png_failure;
    }

    /* Set image attributes. */
    png_set_IHDR(png_ptr,
                 info_ptr,
                 width,
                 height,
                 depth,
                 alpha ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    /* Initialize rows of PNG. */
    row_pointers = (png_byte **)png_malloc(png_ptr, height * sizeof(png_byte *));
    for (uint32_t y = 0; y < height; ++y)
    {
        png_byte *row = (png_byte *)png_malloc(png_ptr, sizeof(uint8_t) * width * pixel_size * (depth / 8));
        row_pointers[y] = (png_byte *)row;
        for (uint32_t x = 0; x < width; ++x)
        {
            RGBColor48 pixel = buffer->element(y, x);
            *row++ = (pixel.r() >>   8);
            *row++ = (pixel.r() & 0xFF);
            *row++ = (pixel.g() >>   8);
            *row++ = (pixel.g() & 0xFF);
            *row++ = (pixel.b() >>   8);
            *row++ = (pixel.b() & 0xFF);

            if (alpha) {
                *row++ = (pixel.a() >>   8);
                *row++ = (pixel.a() & 0xFF);
            }
        }
    }

    /* Write the image data to "fp". */
    png_init_io  (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    toReturn = true;

    for (uint32_t y = 0; y < height; ++y) {
        png_free(png_ptr, row_pointers[y]);
    }
    png_free(png_ptr, row_pointers);

    png_failure:
    png_create_info_struct_failed:
        png_destroy_write_struct(&png_ptr, &info_ptr);
    png_create_write_struct_failed:
        fclose (fp);

    return toReturn;
}

RuntimeTypeBuffer *LibpngRuntimeTypeBufferLoader::load(const string &name)
{
    RGB24Buffer *tmp = LibpngFileReader().load(name);
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

/**
 * Optical flow maps are saved as 3-channel uint16 PNG images: The first channel
 * contains the u-component, the second channel the v-component and the third
 * channel denotes if the pixel is valid or not (1 if true, 0 otherwise). To convert
 * the u-/v-flow into floating point values, convert the value to float, subtract
 * 2^15 and divide the result by 64.0:
 *
 *   flow_u(u,v) = ((float)I(u,v,1)-2^15)/64.0;
 *   flow_v(u,v) = ((float)I(u,v,2)-2^15)/64.0;
 *   valid(u,v)  = (bool)I(u,v,3);
 **/

FloatFlowBuffer *KittiFlowLoader::load(const std::string &name)
{
    RGB48Buffer *tmp = LibpngFileReader().loadRGB48(name);
    if (tmp == nullptr)
        return nullptr;

    FloatFlowBuffer *result = new FloatFlowBuffer(tmp->h, tmp->w);

    for (RGB24Buffer::InternalIndexType y = 0; y < tmp->h; y++)
    {
        for (RGB24Buffer::InternalIndexType x = 0; x < tmp->w; x++)
        {
            RGBColor48 pixel = tmp->element(y, x);
            double u = pixel.r();
            double v = pixel.g();
            bool valid = pixel.b();

            u = (u - 0x8000u) / 64.0;
            v = (v - 0x8000u) / 64.0;

            //cout << "[" << pixel.r() << ", " << pixel.g() << " - " << pixel.b() << "] " << " = [" << u << ", " << v << " - " << valid << "] ";

            result->element(y, x) = FloatFlow(u, v, valid);
        }
    }
    delete tmp;
    return result;
}

bool KittiFlowSaver::save(const FloatFlowBuffer &buffer, const std::string &name, int quality)
{
    RGB48Buffer *result = new RGB48Buffer(buffer.h, buffer.w);
    for (RGB24Buffer::InternalIndexType y = 0; y < buffer.h; ++y)
    {
        for (RGB24Buffer::InternalIndexType x = 0; x < buffer.w; ++x)
        {
            const FloatFlow &element = buffer.element(y, x);

            double u = element.vector.x();
            double v = element.vector.y();
            u = (u * 64.0) + 0x8000u;
            v = (v * 64.0) + 0x8000u;
            result->element(y,x) = RGBColor48((uint16_t)u, (uint16_t)v,  element.isKnown);
        }
    }

    LibpngFileReader().savePNG(name, result, quality);
    delete_safe(result);
    return true;

}
