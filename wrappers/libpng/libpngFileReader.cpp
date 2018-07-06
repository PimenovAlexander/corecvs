#include "libpngFileReader.h"
#include "core/utils/utils.h" // HelperUtils

#include <string>
#include <cstdio>
#include <cstdlib>
#include <png.h>

using namespace corecvs;

string  LibpngFileReader::prefix1(".png");
string  LibpngFileReader::prefix2(".PNG");

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

    //png_info_init(info_ptr);
    //png_read_init(png_ptr);

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, number);
    png_read_info(png_ptr, info_ptr);

    png_uint_32 width, height;
    int color_type, bit_depth;

    png_get_IHDR(png_ptr, info_ptr, &width, &height,
          &bit_depth, &color_type, nullptr,
          nullptr, nullptr);

    if (color_type & PNG_COLOR_MASK_ALPHA)
          png_set_strip_alpha(png_ptr);

    png_read_update_info(png_ptr, info_ptr);


    row_pointers = (png_byte **) png_malloc(png_ptr, height * sizeof (png_byte *));
    for (png_uint_32 y = 0; y < height; y++)
        row_pointers[y] = (png_byte *) png_malloc (png_ptr, sizeof (uint8_t) * width * 3);

    png_read_image(png_ptr, row_pointers);
    png_read_end(png_ptr, nullptr);
    png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

    toReturn = new RGB24Buffer(height, width);

    for (uint32_t y = 0; y < height; y++) {
        png_byte *row = row_pointers[y];
        for (uint32_t x = 0; x < width; x++) {
            RGBColor *dest = &toReturn->element(y,x);
            uint32_t r = *row++;
            uint32_t g = *row++;
            uint32_t b = *row++;
            *dest = RGBColor(r,g,b);
        }
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);

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
    if (!fp)
        return toReturn;

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

corecvs::RuntimeTypeBuffer *LibpngRuntimeTypeBufferLoader::load(const string &name)
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
    delete tmp;
    return result;
}
