#include "libjpegFileReader.h"

#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>

#include "utils.h"

using namespace corecvs;

string  LibjpegFileReader::prefix1(".jpg");
string  LibjpegFileReader::prefix2(".jpeg");


bool LibjpegFileReader::acceptsFile(string name)
{
    return HelperUtils::endsWith(name, prefix1) ||
           HelperUtils::endsWith(name, prefix2) ;
}

RGB24Buffer *LibjpegFileReader::load(string name)
{

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY buffer;
    int row_stride;

    FILE * infile;		/* source file */

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

    RGB24Buffer *result = new RGB24Buffer(cinfo.output_height, cinfo.output_width);

    row_stride = cinfo.output_width * cinfo.output_components;

    buffer = (*cinfo.mem->alloc_sarray)
          ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    while (cinfo.output_scanline < cinfo.output_height) {
        int i = cinfo.output_scanline;
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);

        for (int j = 0; j < cinfo.output_width; j++)
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

LibjpegFileReader::LibjpegFileReader()
{

}

LibjpegFileReader::~LibjpegFileReader()
{

}

