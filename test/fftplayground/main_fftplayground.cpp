#include <fstream>

#include <core/utils/preciseTimer.h>
#include <core/buffers/bufferFactory.h>

#ifdef WITH_FFTW
#include <fftw3.h>
#endif
#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

using namespace std;
using namespace corecvs;

int main (int argc, char **argv)
{
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    LibpngRuntimeTypeBufferLoader::registerMyself();
    LibpngFileSaver::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif

    PreciseTimer timer;

    std::string input = "samples.bin";
    SYNC_PRINT(("Starting fftplayground\n"));

    ifstream file;
    file.open(input, ios::in | ios::binary);
    if (file.fail())
    {
        SYNC_PRINT(("Can't open input file <%s>\n", input.c_str()));
        return 1;
    }
    SYNC_PRINT(("Opened input file <%s> will load %d byte datasamples\n", input.c_str(), (int)sizeof(float)));

    vector<std::complex<float>> data;

    file.seekg(0, std::ios::end);
    size_t num_elements = file.tellg() / sizeof(std::complex<float>);
    file.seekg(0, std::ios::beg);
    data.reserve(num_elements);

    for(int i = 0; !file.eof(); i++)
    {
        float re = 0.0;
        float im = 0.0;
        file.read((char *)&re, sizeof (float));
        file.read((char *)&im, sizeof (float));
        //cout << "Loaded: " << data.size() << " " << re << " + " << im << "i" << endl;
        data.emplace_back(re, im);
    }
    float sampleFreq = 10e6f;
    float samples    = (float)data.size();
    float dataTime = samples / sampleFreq;

    /* NTSC */
    float frameRate = 30.0 / 1.001;
    float scanLinesPerField = 262.5;
    float scanLines = 525;
    float visibleScanlines  = 486;

    float samplesPerFrame = sampleFreq / frameRate;


    SYNC_PRINT(("Loaded %d samples\n", (int)data.size()));
    SYNC_PRINT((" This is %f seconds\n", dataTime));
    SYNC_PRINT((" We expect per frame %f samples\n", samplesPerFrame));



    file.close();



    int window_size = 512*4;
    float *avg = new float[window_size];
    for(int j = 0; j < window_size; j++)
    {
        avg[j] = 0;
    }

#ifdef WITH_FFTW

    fftw_complex *in  = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window_size);
    fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window_size);


    for (int i = 0; i + window_size < data.size(); i += window_size)
    {
        /* Don't want to use memcpy here so far, for more control*/
        for(int j = 0; j < window_size; j++)
        {
            in[j][0] = data[i+j].real();
            in[j][1] = data[i+j].imag();
        }

        fftw_plan p;
        p = fftw_plan_dft_1d(window_size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
        fftw_execute(p);
        fftw_destroy_plan(p);

        for(int j = 0; j < window_size; j++)
        {
            avg[j] += out[j][0] * out[j][0] + out[j][1] * out[j][1];
        }
    }
    fftw_free(in);
    fftw_free(out);
#endif

    double min = 0;
    double max = 0;
    RGB24Buffer *fftShow = new RGB24Buffer(1000, window_size);

    for(int j = 0; j < window_size; j++)
    {
        //cout << j << " " << avg[j] << endl ;
        int y0 = fftShow->h - 1 - (avg[j] / 500.0);

        fftShow->drawVLine(j, fftShow->h - 1, y0, RGBColor::Red());
    }
    BufferFactory::getInstance()->saveRGB24Bitmap(fftShow, "fftw.png");


    int perframe = 333666;
    RGB24Buffer *syncShow = new RGB24Buffer(525, perframe / 525, RGBColor::White());
    float scale = 1.00088;



    for(int i = 0; i < syncShow->h; i++)
    {
        for(int j = 0; j < syncShow->w; j++)
        {
            int offset = (i * syncShow->w + j) * scale;
            if (offset >= data.size())
            {
                syncShow->element(i,j) = RGBColor::Green();
                continue;
            }

            float re = data[offset].real() * 4000.0;
            float im = data[offset].imag() * 4000.0;

            Vector3df color;
            color[0] = sqrt((re * re) + (im * im));
            color[1] = 0;
            color[2] = 0;

            syncShow->element(i,j) = RGBColor::FromFloat(color);

        }
    }
    BufferFactory::getInstance()->saveRGB24Bitmap(syncShow, "sync.png");



    return 0;
}

