#include "core/buffers/converters/debayerTool.h"
#include "core/buffers/converters/debayer.h"
#include "core/buffers/converters/errorMetrics.h"
#include "core/reflection/commandLineSetter.h"
#include "core/fileformats/ppmLoader.h"
#include "core/fileformats/bmpLoader.h"
#include "core/filesystem/folderScanner.h"
#include "core/utils/utils.h"
#include "core/utils/log.h"

namespace corecvs {

void DebayerTool::usage(cchar* progName)
{
    cout << "Image data converter tool with the internal demosaic filter analysis" << endl
        << "Usage:" << endl
        << corecvs::HelperUtils::getFileNameFromFilePath(progName)
        << " --file=name.pgm [--method=N] [--bpos=P] [--ofile=name.ppm] [--ppmbits=-1]" << endl
        << "       [--ifile=name_bayer8bpp.pgm --ibits=-1]" << endl
        << "       [--compare --target=target --methodCmp=N]      , where:" << endl
        << endl
        << " --file=name.pgm specifies input file and data format, possible having Bayer" << endl
        << endl
        << " --method=N specifies demosaic method, where N:" << endl
        << " \t0\tNearest Neighbor" << endl
        << " \t1\tBilinear\t(default)" << endl
        << " \t2\tAHD" << endl
#ifdef WITH_MKL
        << " \t3\tFFT-based frequency filtering" << endl
#endif
        << endl
        << " --bpos=P specifies Bayer position [0..3], where P=0 - RGGB (default)" << endl
        << endl
        << " --ofile=name.ppm specifies output color image filename and data format" << endl
        << " --ppmbits=-1  positive forces 8-bits output format and sets #bits for r-shifting" << endl
        << endl
        << "  --gamma1=1.0 (raw:0.4)    double value (0.2 - 2.9)" << endl
        << "  --gamma2=1.0 (raw:2.8)    double value (0.2 - 2.9)" << endl
        << endl
        << "  --green=1.0               double value (0.1 - 8.0)" << endl
        << "  --red=1.0    (raw:1.3)    double value (0.1 - 8.0)" << endl
        << "  --blue=1.0   (raw:1.8)    double value (0.1 - 8.0)" << endl
        << "  --obits=12        sets scale for all channels by shift maxWhite value in Debyer" << endl
        << endl
        << " --ifile=<name_bayer8bpp.pgm> sets output filename for converted input Bayer image" << endl
        << " --ibits=-1  positive forces 8-bits input  format and sets #bits for r-shifting" << endl
        << endl
        << " --toBayer indicates that utility should generate an artificial Bayer image, using bpos,oBits options" << endl
        << endl
        << " --compare indicates that debayer should work in comparison mode, ignoring all" << endl
        << "   demosaic options and putting comparison result out," << endl
        << "   comparison mode parameters:" << endl
        << "   --target=target specifies target PPM image to compare with" << endl
        << "   --methodCmp=N specifies comparison method, where N:" << endl
        << "   \t0\tPSNR\t(default)" << endl
        << "   \t1\tRMSD" << endl;
}

int DebayerTool::doCompare(CommandLineSetter &s, const std::string &source)
{
    string target = s.getOption("target");
    int    method = s.getInt("methodCmp", Debayer::CompareMethod::PSNR);

    L_INFO_P("compare <%s> with target <%s> by method: %d", source.c_str(), target.c_str(), method);

    MetaData metaSrc, metaTgt;
    std::unique_ptr<RGB48Buffer> inRgb(PPMLoader().loadRgb48(source, &metaSrc));
    std::unique_ptr<RGB48Buffer> ouRgb(PPMLoader().loadRgb48(target, &metaTgt));

    if (inRgb.get() == nullptr)
    {
        L_ERROR_P("couldn't open input file <%s>", source.c_str());
        return -1;
    }
    if (ouRgb.get() == nullptr)
    {
        L_ERROR_P("couldn't open target file <%s>", target.c_str());
        return -1;
    }

    switch (method)
    {
    case Debayer::CompareMethod::PSNR:
        L_INFO_P("result: %lf", corecvs::ErrorMetrics::psnr(inRgb.get(), ouRgb.get()));
        break;
    case Debayer::CompareMethod::RMSD:
        L_INFO_P("result: %lf", corecvs::ErrorMetrics::rmsd(inRgb.get(), ouRgb.get()));
        break;
    default:
        L_ERROR_P("unknown comparison method: %d", method);
        return -1;
    }

    return 0;
}

int DebayerTool::doConvertToBayer(const std::string &src, const std::string &dst, int obits, int bpos)
{
    MetaData meta;
    if (meta["bits"].empty()) {
        meta["bits"].push_back(obits);
    }

    std::unique_ptr<RGB48Buffer> rgb48(PPMLoader().loadRgb48(src, &meta));
    if (!rgb48)
    {
        L_ERROR_P("couldn't load input file <%s>", src.c_str());
        return -1;
    }

    std::unique_ptr<G12Buffer> bayer(new G12Buffer(rgb48->h, rgb48->w, false));

    Debayer d(bayer.get(), meta["bits"][0], &meta, bpos);
    d.fromRgb(rgb48.get());

    return PPMLoader().save(dst, bayer.get());
}

RGB24Buffer* DebayerTool::loadRgb24(const std::string &src)
{
    if (endsWith(src, "bmp"))
        return BMPLoaderRGB24().load(src);

    if (endsWith(src, "ppm"))
        return PPMLoaderRGB24().load(src);

    if (endsWith(src, "pgm"))  // care of PGM must be here, see DebayerTool::loadRgb48
    {
        // TODO: we're to check here if the given PGM file really has a Bayer or not
        //return PPMLoaderRGB24().load(src); // internally it may call Debayer with def.params, but loadRgb48 is better to do that
        return nullptr;
    }

    return nullptr;
}

RGB48Buffer* DebayerTool::loadRgb48(const std::string &src, int inBits, const std::string &bayer8)
{
    if (endsWith(src, "ppm"))
    {
        return PPMLoader().loadRgb48(src);
    }

    if (endsWith(src, "pgm"))  // care of PGM must be here, see DebayerToolRaw::loadRgb24()
    {
        // here we assume that PGM files have Bayer always that should be demosaic!
        MetaData meta;
        std::unique_ptr<G12Buffer> bayer(PPMLoader().loadG12(src, &meta));
        if (!bayer) {
            L_ERROR_P("couldn't load Bayer from file <%s>", src.c_str());
            return nullptr;
        }

        if (inBits != -1)       // special additional mode to convert Bayer 12 to 8 bits
        {
            PPMLoader().save(bayer8, bayer.get(), nullptr, inBits);
        }

        if (debayerParams.bayerPos() < 0)  // it's unknown yet as PGM doesn't have it
        {
            int bposDef = Debayer::Parameters::BestDefaultsByExt("pgm").bayerPos();
            debayerParams.setBayerPos(bposDef);
            L_INFO_P("unknown Bayer position is set to %d", debayerParams.bayerPos());
        }

        return Debayer::DemosaicRgb48(bayer.get(), debayerParams, meta);
    }

    return nullptr;
}

bool DebayerTool::endsWith(const std::string &src, const std::string &ext) const
{
    string s(src);
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return HelperUtils::endsWith(src, ext);
}

int DebayerTool::proceed(int argc, const char **argv)
{
    CommandLineSetter s(argc, argv);
    bool help = s.hasOption("help");
    if (help || argc < 2) {
        usage(argv[0]);
        return 0;
    }

    string filename = s.getOption("file");
    string outfile  = s.getString("ofile", "");

    if (filename.empty())                   // options were not used
    {
        if (!corecvs::FolderScanner::pathExists(argv[1]))
        {
            L_ERROR << "No valid input filename to process!";
            return -1;
        }
        filename = argv[1];
    }
    if (outfile.empty())                        // the option was not used
    {
        if (argc == 2)
            outfile = corecvs::HelperUtils::getFullPathWithNewExt(filename, ".ppm");
        else
            outfile = argv[2];
    }

    bool compare = s.hasOption("compare");
    if (compare) {
        return doCompare(s, filename);
    }

    string ext = filename.substr(filename.find_last_of("."));
    if (ext.size() > 1)
        ext = string(&ext[1]);
    auto defs = Debayer::Parameters::BestDefaultsByExt(ext);

    int bpos  = s.getInt("bpos",  defs.bayerPos());     // -1 - try to extract it from Bayer's meta
    int obits = s.getInt("obits", defs.numBitsOut());   // 12 - don't apply additional scale to all channels

    bool toBayer = s.hasOption("toBayer");        // flag to get an artifical Bayer image
    if (toBayer) {
        return doConvertToBayer(filename, outfile, obits, bpos);
    }

    int    inBits    = s.getInt("ibits", -1);   // -1 - don't force to 8-bits with some shift
    string filename8 = s.getString("ifile", filename + "_bayer8bpp.pgm");
    int    method    = s.getInt("method", DebayerMethod::BILINEAR);

    vector<double> gains = {
          s.getDouble("red",   defs.gains()[0])
        , s.getDouble("green", defs.gains()[1])
        , s.getDouble("blue",  defs.gains()[2])
    };
    vector<double> gamma = {
          s.getDouble("gamma1", defs.gamma()[0])
        , s.getDouble("gamma2", defs.gamma()[1])
    };

    debayerParams = Debayer::Parameters(bpos, gains, gamma
        , (DebayerMethod::DebayerMethod)method, obits);

    std::unique_ptr<RGB24Buffer> rgb24(loadRgb24(filename));
    std::unique_ptr<RGB48Buffer> rgb48(loadRgb48(filename, inBits, filename8));

    if (!rgb24 && !rgb48)
    {
        L_ERROR_P("couldn't load input file <%s>", filename.c_str());
        return -1;
    }
    if (rgb24 && rgb48)
    {
        L_INFO_P("both loadRgb24/loadRgb48 have good results. We're to drop loadRgb24 one");
        rgb24.release();
        return -1;
    }

    // storing data to the output file

    string outfile2 = outfile;
    std::transform(outfile2.begin(), outfile2.end(), outfile2.begin(), ::tolower);

    if (HelperUtils::endsWith(outfile2, "bmp"))
    {
        if (!rgb24 && rgb48)
        {
            rgb24.reset(new RGB24Buffer(rgb48->getSize(), false));
            Debayer::ConvertRgb48toRgb24(rgb48.get(), rgb24.get());
            L_INFO_P("converting Rgb48toRgb24 ...");
        }
        if (!BMPLoader().save(outfile, rgb24.get()))
        {
            L_ERROR_P("couldn't write to <%s>", outfile.c_str());
            return -1;
        }
    }
    else if (HelperUtils::endsWith(outfile2, "ppm"))
    {
        if (!rgb48 && rgb24)
        {
            rgb48.reset(new RGB48Buffer(rgb24->getSize(), false));
            Debayer::ConvertRgb24toRgb48(rgb24.get(), rgb48.get());
            L_INFO_P("converting Rgb24toRgb48 ...");
        }

        int ppmBits = s.getInt("ppmbits", -1); // -1 - don't force to 8-bits with some shift

        int res = (ppmBits != -1)
            ? PPMLoader().save(outfile, rgb48.get(), nullptr, ppmBits) // trunc to 8 bits
            : PPMLoader().save(outfile, rgb48.get());
        if (res != 0)
            return res;
    }
    else
    {
        if (!rgb24 && rgb48)
        {
            rgb24.reset(new RGB24Buffer(rgb48->getSize(), false));
            Debayer::ConvertRgb48toRgb24(rgb48.get(), rgb24.get());
            L_INFO_P("converting Rgb48toRgb24 ...");
        }

        if (!saveRgb24(outfile, *rgb24))
        {
            L_ERROR_P("couldn't recognize output file format <%s>", outfile.c_str());
            return -1;
        }
    }

    L_INFO_P("saved <%s>", corecvs::HelperUtils::getFileNameFromFilePath(outfile).c_str());
    return 0;
}

} //namespace corecvs
