#ifndef COMPLEXWAVELET_H
#define COMPLEXWAVELET_H

#include "core/buffers/abstractBuffer.h"
#include "core/buffers/float/dpImage.h"
#include "fsAlgorithm.h"
using namespace std;

/**
 *       This class is an implementation of complex wavelets for extended depth-of-field algorithm.
 *    Algorithm first extends all images of stack to the size of power of 2 if it is not. The original images are saved
 *    to new stack for the purpose of further selection of pixels to make the final result image.
 *    After extending images (if images are multichannel)the next step is to make multichannel conversion to prepare
 *    for wavelets transformations.
 *
 *      Then wavelet transform is made and the wavelet coefficients of the composite image are built up by
 *    a maximum-absolute-value selection rule. The final composite image is obtained after computing the
 *    inverse complex wavelet transform. After that crop to original size of image is made and recovery
 *    to colors is performed. Stack with saved images helps to avoid the introduction of false colors.
 *    More information about algorithm can be found: http://bigwww.epfl.ch/publications/forster0404.html
 **/

class ComplexWavelet : public FSAlgorithm
{
public:
    ComplexWavelet(){}
    void doStacking(vector<corecvs::RGB24Buffer*> & imageStack, corecvs::RGB24Buffer * result) override;
    ~ComplexWavelet() override {}
    static void test();
    static bool test_split();
    static bool test_split_1D();
    static bool test_analysis();
    static bool test_synthesis();
    static bool test_merge_1D();
    static bool test_merge();


private:
    /**
    *    Makes one iteration of the inverse wavelet transformation of an image.
    *    The algorithm uses separations of the wavelet transformation.
    *    The result of the computation is returned in AbstractBuffer<double>.
    *    @param in	representation of image in pixels
    *    @param type1 a type for double 1D vector for lowpass filter
    *    @param type2 a type for double 1D vector for highpass filter
    *    @return merge result
    **/
    static corecvs::AbstractBuffer<double> * merge(corecvs::AbstractBuffer<double> * in, int type1, int type2);
    /**
	*   Makes one iteration of the wavelet transformation of an image.
	*   The algorithm uses separations of the wavelet transformation.
    *   @param in representation of image in pixels
    *   @param type1 a type for double 1D vector for lowpass filter
    *   @param type2 a type for double 1D vector for highpass filter
    *   @return splited image in pixels
	**/
    static corecvs::AbstractBuffer<double> * split(corecvs::AbstractBuffer<double> * in, int type1, int type2);
    /**
    *    Computes the complex wavelets transform of a given image.
    *    The size of image should be interger factor of 2 at the power n.
    *    The input is the result of a wavelet transformation.
    *    @param in input image
    *    @return the wavelets coefficients
    **/
    static pair<corecvs::AbstractBuffer<double> *, corecvs::AbstractBuffer<double> *> analysis(corecvs::DpImage * in);
    /**
    *    This method is intended for subtracting two images elementwise.
    *    @param im1	 image representation in 2D vector
    *    @param im2	 image representation in 2D vector
    *    @return im1 = im1 - im2
    **/
    static void subtract(corecvs::AbstractBuffer<double> * im1, corecvs::AbstractBuffer<double> * im2);
    /**
    *    This method is intended for adding two images elementwise.
    *    @param im1	 image representation in 2D vector
    *    @param im2	 image representation in 2D vector
    *    @return im1 = im1 + im2
    **/
    static void add(corecvs::AbstractBuffer<double> * im1, corecvs::AbstractBuffer<double> * im2);
    /**
    *    Performs an inverse wavelet transformation of the image.
    *    The size of image should be integer factor of 2 at the power n.
    *    The input is the result of a wavelet transformation.
    *    The result is the reconstruction of the image.
    *    @param inRe the real part of the wavelets coefficients
    *    @param inIm the imaginary part of the wavelets coefficients
    *    @return the reconstructed image
    **/
    static corecvs::AbstractBuffer<double> * synthesis(corecvs::AbstractBuffer<double> * inRe, corecvs::AbstractBuffer<double> * inIm);
};

#endif //COMPLEXWAVELET_H