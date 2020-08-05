#include "libjpegFileReader.h"

#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>

#include "core/utils/utils.h"

using namespace corecvs;

string  LibjpegFileReader::prefix1(".jpg");
string  LibjpegFileReader::prefix2(".jpeg");
string  LibjpegFileReader::prefix3(".JPG");
string  LibjpegFileReader::prefix4(".JPEG");

bool LibjpegFileReader::acceptsFile(const std::string &name)
{
    return HelperUtils::endsWith(name, prefix1) ||
           HelperUtils::endsWith(name, prefix2) ||
           HelperUtils::endsWith(name, prefix3) ||
           HelperUtils::endsWith(name, prefix4);
}

RGB24Buffer *LibjpegFileReader::load(const std::string & name)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY buffer;
    int row_stride;

    FILE * infile = NULL;		/* source file */

    if ((infile = fopen(name.c_str(), "rb")) == NULL) {
      SYNC_PRINT(("can't open %s\n", name.c_str()));
      return NULL;
    }

    /* Step 1: allocate and initialize JPEG decompression object */

    cinfo.err = jpeg_std_error(&jerr);
   /* jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
      jpeg_destroy_decompress(&cinfo);
      fclose(infile);
      return NULL;
    }*/

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    (void) jpeg_read_header(&cinfo, TRUE);
    (void) jpeg_start_decompress(&cinfo);

    /*SYNC_PRINT(("Parsed header [%dx%d] out = [%dx%d]\n",
                cinfo.image_width,  cinfo.image_height,
                cinfo.output_width, cinfo.output_height));*/

    RGB24Buffer *result = new RGB24Buffer(cinfo.output_height, cinfo.output_width);

    row_stride = cinfo.output_width * cinfo.output_components;

    buffer = (*cinfo.mem->alloc_sarray)
          ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    while (cinfo.output_scanline < cinfo.output_height) {
        int i = cinfo.output_scanline;
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);

        for (JDIMENSION j = 0; j < cinfo.output_width; j++)
        {
            if (!result->isValidCoord(i, j))
            {
                SYNC_PRINT(("-(%dx%d)\n", i, j ));
            }

            result->element(i, j) =
                    RGBColor(
                        buffer[0][j*cinfo.output_components + 0],
                        buffer[0][j*cinfo.output_components + 1],
                        buffer[0][j*cinfo.output_components + 2]
                    );
        }
        //put_scanline_someplace(buffer[0], row_stride);
    }

    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return result;
}

/** based on https://github.com/LuaDist/libjpeg/blob/master/example.c */
bool LibjpegFileReader::saveJPEG(const std::string &name, const RGB24Buffer *buffer, int quality, bool alpha)
{
    CORE_UNUSED(alpha);
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    if (buffer == NULL) {
        return false;
    }

    FILE * outfile = NULL;		/* target file */
    JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
    int row_stride;		        /* physical row width in image buffer */

    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_compress(&cinfo);

    if ((outfile = fopen(name.c_str(), "wb")) == NULL) {
      SYNC_PRINT(("LibjpegFileReader::saveJPEG(): can't open %s\n", name.c_str()));
      return false;
    }
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width  = buffer->w; 	/* image width and height, in pixels */
    cinfo.image_height = buffer->h;
    cinfo.input_components = 3;		    /* # of color components per pixel */
    cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

    jpeg_start_compress(&cinfo, TRUE);

    row_stride = buffer->w * 3;	/* JSAMPLEs per row in image_buffer */

    uint8_t *scratch = new uint8_t[row_stride];
    row_pointer[0] = scratch;

    while (cinfo.next_scanline < (JDIMENSION)buffer->h) {
        const RGBColor *c = &buffer->element(cinfo.next_scanline, 0);
        for (int j = 0; j < buffer->w; j++)
        {
            scratch[3 * j + 0] = c->r();
            scratch[3 * j + 1] = c->g();
            scratch[3 * j + 2] = c->b();
            c++;
        }
       (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    deletearr_safe(scratch);

    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    jpeg_destroy_compress(&cinfo);

    return true;
}


bool LibjpegFileReader::saveJPEG(std::vector<unsigned char> &mem_vector, const RGB24Buffer *buffer, int quality, bool alpha)
{
    CORE_UNUSED(alpha);
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    if (buffer == NULL) {
        return false;
    }

    JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
    int row_stride;		        /* physical row width in image buffer */

    cinfo.err = jpeg_std_error(&jerr);

    jpeg_create_compress(&cinfo);

    unsigned char *mem = NULL;
    unsigned long mem_size = 0;
    jpeg_mem_dest(&cinfo, &mem, &mem_size);

    cinfo.image_width  = buffer->w; 	/* image width and height, in pixels */
    cinfo.image_height = buffer->h;
    cinfo.input_components = 3;		    /* # of color components per pixel */
    cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

    jpeg_start_compress(&cinfo, TRUE);

    row_stride = buffer->w * 3;	/* JSAMPLEs per row in image_buffer */

    uint8_t *scratch = new uint8_t[row_stride];
    row_pointer[0] = scratch;

    while (cinfo.next_scanline < (JDIMENSION)buffer->h) {
        const RGBColor *c = &buffer->element(cinfo.next_scanline, 0);
        for (int j = 0; j < buffer->w; j++)
        {
            scratch[3 * j + 0] = c->r();
            scratch[3 * j + 1] = c->g();
            scratch[3 * j + 2] = c->b();
            c++;
        }
       (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    deletearr_safe(scratch);

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    SYNC_PRINT(("LibjpegFileReader::saveJPEG(): in memory size %d\n", (int)mem_size));

    mem_vector.resize(mem_size);
    memcpy(mem_vector.data(), mem, mem_size);
    free(mem);
    return true;
}

LibjpegFileReader::LibjpegFileReader()
{}

LibjpegFileReader::~LibjpegFileReader()
{}


bool LibjpegRuntimeTypeBufferLoader::acceptsFile(const std::string &name)
{
    return HelperUtils::endsWith(name, LibjpegFileReader::prefix1) ||
           HelperUtils::endsWith(name, LibjpegFileReader::prefix2) ||
           HelperUtils::endsWith(name, LibjpegFileReader::prefix3) ||
           HelperUtils::endsWith(name, LibjpegFileReader::prefix4);
}

RuntimeTypeBuffer *LibjpegRuntimeTypeBufferLoader::load(const std::string & name)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY buffer;
    int row_stride;

    FILE * infile = NULL;		/* source file */

    if ((infile = fopen(name.c_str(), "rb")) == NULL) {
      SYNC_PRINT(("can't open %s\n", name.c_str()));
      return NULL;
    }

    /* Step 1: allocate and initialize JPEG decompression object */

    cinfo.err = jpeg_std_error(&jerr);
   /* jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
      jpeg_destroy_decompress(&cinfo);
      fclose(infile);
      return NULL;
    }*/

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    (void) jpeg_read_header(&cinfo, TRUE);
    (void) jpeg_start_decompress(&cinfo);

    SYNC_PRINT(("Parsed header [%dx%d] out = [%dx%d]\n",
                cinfo.image_width,  cinfo.image_height,
                cinfo.output_width, cinfo.output_height));

    RuntimeTypeBuffer *result = new RuntimeTypeBuffer(cinfo.output_height, cinfo.output_width, BufferType::U8);

    row_stride = cinfo.output_width * cinfo.output_components;

    buffer = (*cinfo.mem->alloc_sarray)
          ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    while (cinfo.output_scanline < cinfo.output_height) {
        int i = cinfo.output_scanline;
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);

        for (JDIMENSION j = 0; j < cinfo.output_width; j++)
        {
#if 0
            if (i >= cinfo.image_height || j >= cinfo.image_width)
            {
                SYNC_PRINT(("-(%dx%d)\n", i, j ));
            }
#endif
            uint8_t gray = RGBColor(
                buffer[0][j*cinfo.output_components + 0],
                buffer[0][j*cinfo.output_components + 1],
                buffer[0][j*cinfo.output_components + 2]).brightness();

            result->at<uint8_t>(i,j) = gray;
        }
    }

    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return result;
}

LibjpegRuntimeTypeBufferLoader::LibjpegRuntimeTypeBufferLoader()
{}

LibjpegRuntimeTypeBufferLoader::~LibjpegRuntimeTypeBufferLoader()
{}
