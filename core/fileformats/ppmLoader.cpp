/**
 * \file ppmLoader.cpp
 * \brief Add Comment Here
 *
 *  TODO : Remove doubling code in PPM and PPM6 loader ASAP
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */
#include <string>

#include "global.h"

#include "ppmLoader.h"

namespace corecvs {

string PPMLoader::prefix1(".pgm");

bool PPMLoader::acceptsFile(string name)
{
    //string ending = name.substr(name.length() - prefix1.length(), prefix1.length());

    return (name.compare(name.length() - prefix1.length(), prefix1.length(), prefix1) == 0);
}


G12Buffer* PPMLoader::load(string name)
{
    DOTRACE(("Will load the file %s as PPM\n ", name.c_str()));

    G12Buffer *toReturn = g12BufferCreateFromPPM(name);
    return toReturn;
}

G12Buffer* PPMLoader::g12BufferCreateFromPPM (string& name)
{
    FILE      *fp        = NULL;
    uint8_t   *charImage = NULL;
    G12Buffer *toReturn  = NULL;


    //PGM Headers Variable Declaration
    unsigned long int i, j;
    unsigned long int h, w;
    int type;
    unsigned short int maxval;
    int shiftCount = 0;

    //Open file for Reading in Binary Mode
    fp = fopen(name.c_str(), "rb");

    if (fp == NULL) {
        printf("Image %s does not exist \n", name.c_str());
        return NULL;
    }

    if (!readHeader(fp, &h, &w, &maxval, &type))
    {
        return NULL;
    }

    if (maxval <= 255)
    {
        charImage = new uint8_t[w * h];
        if (fread(charImage, 1, (w * h) * sizeof(uint8_t), fp) == 0)
        {
            printf("fread() call failed %s():%d\n", __FILE__, __LINE__);
            goto done;
        }

        toReturn = new G12Buffer(h, w, false);

        for (i = 0; i < h; i++)
            for (j = 0; j < w; j++)
                toReturn->element(i, j) = (charImage[i * w + j]) << 4;

    } else {
        for (shiftCount = 0; (maxval >> shiftCount) > G12Buffer::BUFFER_MAX_VALUE; shiftCount++);

        charImage = new uint8_t[2 * w * h];
        if (fread(charImage, 1, (w * h) * sizeof(uint8_t) * 2, fp) == 0)
        {
            printf("fread() call failed %s:%d\n", __FILE__, __LINE__);
            goto done;
        }

        toReturn = new G12Buffer(h, w, false);

        for (i = 0; i < h; i++)
        {
            for (j = 0; j < w; j++)
            {
                int offset = i * w + j;
                toReturn->element(i, j) = ((charImage[offset * 2]) << 8 |
                                          (charImage[offset * 2 + 1])) >> shiftCount;

                ASSERT_FALSE((toReturn->element(i, j) >= (1 << G12Buffer::BUFFER_BITS)), "Internal error in image loader\n");
            }
        }
    }

done:
    if (fp != NULL)
        fclose(fp);
    if (charImage != NULL)
        delete[] charImage;
    return toReturn;
}

bool PPMLoader::readHeader(FILE *fp, unsigned long int *h, unsigned long int *w, unsigned short int *maxval, int *type)
{
    char header[255];

    //Check the PGM file Type P2 or P5
    if (fgets(header, sizeof(header), fp) == NULL)
    {
        printf("fgets() call failed %s:%d\n", __FILE__, __LINE__);
        return false;
    }
    if ((header[0] != 'P') || ((header[1] != '5') && (header[1] != '6'))) {
        printf("Image is not a supported PGM\n");
        return false;
    }
    *type = header[1];

    int isComment = 1;
    //Check the Comments and empty lines
    do {
        if (fgets(header, sizeof(header), fp) == NULL)
        {
            printf("fgets() call failed %s:%d\n", __FILE__, __LINE__);
            return false;
        }

        if (header[0] == '#')
            continue;
        char *runner = header;
        while (*runner != 0)
        {
            if (!(*runner == ' ' || *runner == '\n' || *runner == '\r' || *runner == '\t'))
            {
                isComment = 0;
                break;
            }
            runner++;
        }
    } while (isComment);

    if (sscanf(header, "%lu %lu", w, h) != 2)
    {
        printf("Image dimensions could not be read form line %s\n", header);
        return false;
    }
    if (fgets(header, sizeof(header), fp) == NULL)
    {
        printf("fgets() call failed %s:%d\n", __FILE__, __LINE__);
        return false;
    }
    if (sscanf(header, "%hu", maxval) != 1)
    {
        printf("Image metric could not be read form line %s\n", header);
        return false;
    }

    DOTRACE(("Image is P5 PPM [%lu %lu] max=%u\n", *h, *w, *maxval));
    return true;
}


int PPMLoader::save(string name, G12Buffer *buffer)
{
    int h = buffer->h;
    int w = buffer->w;

    if (buffer == NULL)
        return -1;

    FILE *fp;
    fp = fopen(name.c_str(), "wb");

    if (fp == NULL) {
        printf("Image %s could not be written \n", name.c_str());
        return -1;
    }

    fprintf(fp, "P5\n");
    fprintf(fp, "############################################\n");
    fprintf(fp, "# This file is written by DeepView library.\n");
    fprintf(fp, "# \n");
    fprintf(fp, "# The original source buffer had 12 bit.\n");

    /// \todo TODO: Add some metadata saving

    fprintf(fp, "############################################\n");
    fprintf(fp, "%d %d\n", w, h);
    fprintf(fp, "%d\n", G12Buffer::BUFFER_MAX_VALUE);

    uint8_t *charImage = new uint8_t[2 * w * h];
    int i,j;
    for (i = 0; i < h; i++)
        for (j = 0; j < w; j++)
        {
            int offset = i * w + j;
            charImage[offset * 2]     = (buffer->element(i,j) >> 8) & 0xFF;
            charImage[offset * 2 + 1] =  buffer->element(i,j)       & 0xFF;
        }
    fwrite(charImage, 2, h * w, fp);
    delete[] charImage;
    fclose(fp);
    return 0;
}

int PPMLoader::saveG16(string name, G12Buffer *buffer)
{
    int h = buffer->h;
    int w = buffer->w;

    if (buffer == NULL)
        return -1;

    FILE *fp;
    fp = fopen(name.c_str(), "wb");

    if (fp == NULL) {
        printf("Image %s could not be written \n", name.c_str());
        return -1;
    }

    fprintf(fp, "P5\n");
    fprintf(fp, "############################################\n");
    fprintf(fp, "# This file is written by DeepView library.\n");
    fprintf(fp, "# \n");
    fprintf(fp, "# The original source buffer had 16 bit.\n");

    /// \todo TODO: Add some metadata saving

    fprintf(fp, "############################################\n");
    fprintf(fp, "%d %d\n", w, h);
    fprintf(fp, "%d\n", 0xFFFF);

    uint8_t *charImage = new uint8_t[2 * w * h];
    int i,j;
    for (i = 0; i < h; i++)
    {
        for (j = 0; j < w; j++)
        {
            int offset = i * w + j;
            charImage[offset * 2]     = (buffer->element(i,j) >> 8) & 0xFF;
            charImage[offset * 2 + 1] =  buffer->element(i,j)       & 0xFF;
        }
    }
    fwrite(charImage, 2, h * w, fp);
    delete[] charImage;
    fclose(fp);
    return 0;
}

/* Temporary 16bit download*/

G12Buffer* PPMLoader::g16BufferCreateFromPPM (string& name)
{
    FILE      *fp        = NULL;
    uint8_t   *charImage = NULL;
    G12Buffer *toReturn  = NULL;


    //PGM Headers Variable Declaration
    unsigned long int i, j;
    unsigned long int h, w;
    int type;
    unsigned short int maxval;
    int shiftCount = 0;

    //Open file for Reading in Binary Mode
    fp = fopen(name.c_str(), "rb");

    if (fp == NULL) {
        printf("Image %s does not exist \n", name.c_str());
        return NULL;
    }

    if (!readHeader(fp, &h, &w, &maxval, &type))
    {
        return NULL;
    }

    if (maxval <= 255)
    {
        charImage = new uint8_t[w * h];
        if (fread(charImage, 1, (w * h) * sizeof(uint8_t), fp) == 0)
        {
            printf("fread() call failed %s():%d\n", __FILE__, __LINE__);
            goto done;
        }

        toReturn = new G12Buffer(h, w, false);

        for (i = 0; i < h; i++)
            for (j = 0; j < w; j++)
                toReturn->element(i, j) = (charImage[i * w + j]) << 8;

    } else {
        for (shiftCount = 0; (maxval >> shiftCount) > (1 << 16); shiftCount++);

        charImage = new uint8_t[2 * w * h];
        if (fread(charImage, 1, (w * h) * sizeof(uint8_t) * 2, fp) == 0)
        {
            printf("fread() call failed %s:%d\n", __FILE__, __LINE__);
            goto done;
        }

        toReturn = new G12Buffer(h, w, false);

        for (i = 0; i < h; i++)
        {
            for (j = 0; j < w; j++)
            {
                int offset = i * w + j;
                toReturn->element(i, j) = ((charImage[offset * 2]) << 8 |
                                          (charImage[offset * 2 + 1])) >> shiftCount;

                ASSERT_FALSE((toReturn->element(i, j) >= (1 << 16)), "Internal error in image loader\n");
            }
        }
    }

done:
    if (fp != NULL)
        fclose(fp);
    if (charImage != NULL)
        delete[] charImage;
    return toReturn;
}


} //namespace corecvs

