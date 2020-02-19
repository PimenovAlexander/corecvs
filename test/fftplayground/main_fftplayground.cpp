#include <fstream>

#include "core/reflection/commandLineSetter.h"
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

void drawFFT(int window_size, vector<complex<float>> &data, std::string name)
{
    float *avg = new float[window_size];
    for(int j = 0; j < window_size; j++)
    {
        avg[j] = 0;
    }

#ifdef WITH_FFTW
    fftw_complex *in       = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window_size);
    fftw_complex *out      = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window_size);

    for (size_t i = 0; i + window_size < data.size(); i += window_size)
    {
        /* Don't want to use memcpy here so far, for more control*/
        for(int j = 0; j < window_size; j++)
        {
            in[j][0] = data[i+j].real();
            in[j][1] = data[i+j].imag();
        }

        fftw_plan pf;
        pf = fftw_plan_dft_1d(window_size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
        fftw_execute(pf);
        fftw_destroy_plan(pf);

        for(int j = 0; j < window_size; j++)
        {
            avg[j] += out[j][0] * out[j][0] + out[j][1] * out[j][1];
        }
    }
#endif

    int dscale = window_size / 1024;
    float vscale = 0;

    for(int j = 0; j < window_size; j++)
    {
        if (avg[j] > vscale) vscale = avg[j];
    }
    if (vscale == 0) vscale = 1;
    vscale = 3000.0;
    cout << "vscale :" << vscale << endl;

    RGB24Buffer *fftShow = new RGB24Buffer(1000, 1024);

    for(int j = 0; j < fftShow->w; j ++)
    {
        double value = 0;
        for (int k = 0; k < dscale; k++)
        {
            value += avg[j * dscale + k];
        }
        value /= dscale;

        //cout << j << " " << avg[j] << endl ;
        int y0 = fftShow->h - 1 - (value / vscale);

        fftShow->drawVLine(j, fftShow->h - 1, y0, RGBColor::Red());
    }
    BufferFactory::getInstance()->saveRGB24Bitmap(fftShow, name);

    deletearr_safe(avg);
}


int main (int argC, char **argV)
{
    CommandLineSetter s(argC, argV);

#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    LibpngRuntimeTypeBufferLoader::registerMyself();
    LibpngFileSaver::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif

    PreciseTimer timer;

    std::string input = "samples.bin";

    if (s.nonPrefix().size() > 1)
    {
        input = s.nonPrefix()[1];
    }

    SYNC_PRINT(("Starting fftplayground for %s\n", input.c_str()));

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



    int window_size = 4 * 1024 * 1024;

    /**
     * Shift data to center.
     *
     * For more information see http://www.fftw.org/faq/section3.html#centerorigin
     **/
    for(size_t i = 0; i < data.size(); i++)
    {
        if (i % 2) {
            data[i].real(-data[i].real());
            data[i].imag(-data[i].imag());
        }
    }

#ifdef WITH_FFTW

    fftw_complex *in       = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window_size);
    fftw_complex *out      = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window_size);

    fftw_complex *freq     = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window_size);

    fftw_complex *sync     = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window_size);
    fftw_complex *filtered = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * window_size);

    drawFFT(window_size, data, "prefilter.png");


    for (int i = 0; i + window_size < data.size(); i += window_size)
    {
        /* Don't want to use memcpy here so far, for more control*/
        for(int j = 0; j < window_size; j++)
        {
            in[j][0] = data[i+j].real();
            in[j][1] = data[i+j].imag();
        }

        fftw_plan pf;
        pf = fftw_plan_dft_1d(window_size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
        fftw_execute(pf);
        fftw_destroy_plan(pf);

        /* Filter. Don't want to think of anything more complicated */
        /* First delete the aliasing artifact from capute*/
        out[window_size / 2][0] = 0;
        out[window_size / 2][1] = 0;


        /* Seems like leftmost peak is a nice feature to get h-sync*/

        /* Prepare data */

        int shift = 2 * 1024 * 700;
        int swindow = 2 * 1024 * 122;

        for(int j = 0; j < swindow; j++)
        {
            freq[j][0] = out[j + shift][0];
            freq[j][1] = out[j + shift][1];
        }

        for(int j = swindow; j < window_size; j++)
        {
            freq[j][0] = 0;
            freq[j][1] = 0;
        }


        fftw_plan pb;
        pb = fftw_plan_dft_1d(window_size, freq, filtered, FFTW_BACKWARD, FFTW_ESTIMATE);
        fftw_execute(pb);
        fftw_destroy_plan(pb);

        for(int j = 0; j < window_size; j++)
        {
            data[i+j].real(filtered[j][0] / window_size);
            data[i+j].imag(filtered[j][1] / window_size);
        }
    }
    fftw_free(in);
    fftw_free(out);
    fftw_free(sync);
    fftw_free(filtered);

    drawFFT(window_size, data, "postfilter.png");

#endif

    RGB24Buffer *sliceGraph = new RGB24Buffer(500, 4000, RGBColor::White());
    for (int i = 1; i < sliceGraph->w && i < data.size(); i++)
    {
        double value0 = data[i - 1].imag() * data[i - 1].imag() + data[i - 1].real() * data[i - 1].real();
        double value1 = data[i    ].imag() * data[i    ].imag() + data[i    ].real() * data[i    ].real();

        value0 = sqrt(value0);
        value1 = sqrt(value1);

        value0 *= 4000;
        value1 *= 4000;

        sliceGraph->drawLine( i - 1, sliceGraph->h - 1 - (int)value0,
                              i    , sliceGraph->h - 1 - (int)value1,
                             RGBColor::Blue());
    }
    BufferFactory::getInstance()->saveRGB24Bitmap(sliceGraph, "osciloscope.png");



    int perframe = 333666;
    int width = perframe / scanLines;
    RGB24Buffer *syncShow = new RGB24Buffer(scanLines, width, RGBColor::White());
    float scale = 1.00088;
    int startOffset = width * 201.15;


    for(int i = 0; i < syncShow->h; i++)
    {
        for(int j = 0; j < syncShow->w; j++)
        {
            int offset = (i * syncShow->w + j) * scale + startOffset;
            if (offset >= data.size())
            {
                syncShow->element(i,j) = RGBColor::Green();
                continue;
            }

            float re = data[offset].real() * 4000.0;
            float im = data[offset].imag() * 4000.0;

            Vector3df color;
            color[0] = 255.0 - sqrt((re * re) + (im * im));
            color[1] = color[0];
            color[2] = color[0];

            syncShow->element(i,j) = RGBColor::FromFloat(color);

        }
    }
    BufferFactory::getInstance()->saveRGB24Bitmap(syncShow, "sync.png");

    RGB24Buffer *deinterlace = new RGB24Buffer(scanLines, width, RGBColor::White());
    for(int i = 0; i < deinterlace->h; i++)
    {
        for(int j = 0; j < deinterlace->w; j++)
        {
            int h = i / 2;
            if (i % 2) {
                h += scanLines / 2;
            }
            deinterlace->element(i,j) = syncShow->element(h, j);

        }
    }

    BufferFactory::getInstance()->saveRGB24Bitmap(deinterlace, "deint.png");


    return 0;
}

