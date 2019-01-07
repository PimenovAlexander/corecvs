#ifndef DRAW_3D_CAMERA_PARAMETERS_H_
#define DRAW_3D_CAMERA_PARAMETERS_H_
/**
 * \file draw3dCameraParameters.h
 * \attention This file is automatically generated and should not be in general modified manually
 *
 * \date MMM DD, 20YY
 * \author autoGenerator
 * Generated from draw3d.xml
 */

#include "core/reflection/reflection.h"
#include "core/reflection/defaultSetter.h"
#include "core/reflection/printerVisitor.h"

/*
 *  Embed includes.
 */
#include "draw3dParameters.h"
/*
 *  Additional includes for Composite Types.
 */
#include "core/xml/generated/rgbColorParameters.h"

// using namespace corecvs;

/*
 *  Additional includes for Pointer Types.
 */

// namespace corecvs {
// }
/*
 *  Additional includes for enum section.
 */
#include "draw3dStyle.h"
#include "draw3dTextureGen.h"

/**
 * \brief draw 3d Camera Parameters 
 * draw 3d Camera Parameters 
 **/
class Draw3dCameraParameters : public corecvs::BaseReflection<Draw3dCameraParameters>
{
public:
    enum FieldId {
        FOVH_ID,
        FOVV_ID,
        NEARPLANE_ID,
        FARPLANE_ID,
        STYLE_ID,
        POINT_COLOR_ID,
        POINT_COLOR_OVERRIDE_ID,
        POINT_SIZE_ID,
        EDGE_COLOR_ID,
        EDGE_COLOR_OVERRIDE_ID,
        EDGE_WIDTH_ID,
        FACE_COLOR_ID,
        FACE_COLOR_OVERRIDE_ID,
        SHOW_CAPTION_ID,
        FONT_SIZE_ID,
        FONT_WIDTH_ID,
        FONT_COLOR_ID,
        TEXTURE_CORRODINATES_ID,
        TEXTURE_ALPHA_ID,
        TEXTURE_SCALE_ID,
        DECAL_MATRIX_TYPE_ID,
        DECAL_LEFT_CAM_ID,
        DECAL_LEFT_ALPHA_ID,
        DECAL_RIGHT_CAM_ID,
        DECAL_RIGHT_ALPHA_ID,
        DRAW_3D_CAMERA_PARAMETERS_FIELD_ID_NUM
    };

    /** Section with variables */

    /** 
     * \brief fovH 
     * fovH 
     */
    double mFovH;

    /** 
     * \brief fovV 
     * fovV 
     */
    double mFovV;

    /** 
     * \brief nearPlane 
     * nearPlane 
     */
    double mNearPlane;

    /** 
     * \brief farPlane 
     * farPlane 
     */
    double mFarPlane;

    /** 
     * \brief style 
     * style 
     */
    int mStyle;

    /** 
     * \brief Point Color 
     * Point Color 
     */
    RgbColorParameters mPointColor;

    /** 
     * \brief Point Color Override 
     * Point Color Override 
     */
    bool mPointColorOverride;

    /** 
     * \brief Point Size 
     * Point Size 
     */
    int mPointSize;

    /** 
     * \brief Edge Color 
     * Edge Color 
     */
    RgbColorParameters mEdgeColor;

    /** 
     * \brief Edge Color Override 
     * Edge Color Override 
     */
    bool mEdgeColorOverride;

    /** 
     * \brief Edge Width 
     * Edge Width 
     */
    int mEdgeWidth;

    /** 
     * \brief Face Color 
     * Face Color 
     */
    RgbColorParameters mFaceColor;

    /** 
     * \brief Face Color Override 
     * Face Color Override 
     */
    bool mFaceColorOverride;

    /** 
     * \brief Show caption 
     * Show caption 
     */
    bool mShowCaption;

    /** 
     * \brief Font Size 
     * Font Size 
     */
    int mFontSize;

    /** 
     * \brief Font Width 
     * Font Width 
     */
    int mFontWidth;

    /** 
     * \brief Font Color 
     * Font Color 
     */
    RgbColorParameters mFontColor;

    /** 
     * \brief Texture Corrodinates 
     * Texture Corrodinates 
     */
    int mTextureCorrodinates;

    /** 
     * \brief Texture Alpha 
     * Texture Alpha 
     */
    int mTextureAlpha;

    /** 
     * \brief Texture Scale 
     * Texture Scale 
     */
    double mTextureScale;

    /** 
     * \brief Decal Matrix Type 
     * Decal Matrix Type 
     */
    int mDecalMatrixType;

    /** 
     * \brief Decal Left Cam 
     * Decal Left Cam 
     */
    bool mDecalLeftCam;

    /** 
     * \brief Decal Left Alpha 
     * Decal Left Alpha 
     */
    int mDecalLeftAlpha;

    /** 
     * \brief Decal Right Cam 
     * Decal Right Cam 
     */
    bool mDecalRightCam;

    /** 
     * \brief Decal Right Alpha 
     * Decal Right Alpha 
     */
    int mDecalRightAlpha;

    /** Static fields init function, this is used for "dynamic" field initialization */ 
    static int staticInit(corecvs::Reflection *toFill);

    static int relinkCompositeFields();

    /** Section with getters */
    const void *getPtrById(int fieldId) const
    {
        return (const unsigned char *)(this) + fields()[fieldId]->offset;
    }
    double fovH() const
    {
        return mFovH;
    }

    double fovV() const
    {
        return mFovV;
    }

    double nearPlane() const
    {
        return mNearPlane;
    }

    double farPlane() const
    {
        return mFarPlane;
    }

    Draw3dStyle::Draw3dStyle style() const
    {
        return static_cast<Draw3dStyle::Draw3dStyle>(mStyle);
    }

    RgbColorParameters pointColor() const
    {
        return mPointColor;
    }

    bool pointColorOverride() const
    {
        return mPointColorOverride;
    }

    int pointSize() const
    {
        return mPointSize;
    }

    RgbColorParameters edgeColor() const
    {
        return mEdgeColor;
    }

    bool edgeColorOverride() const
    {
        return mEdgeColorOverride;
    }

    int edgeWidth() const
    {
        return mEdgeWidth;
    }

    RgbColorParameters faceColor() const
    {
        return mFaceColor;
    }

    bool faceColorOverride() const
    {
        return mFaceColorOverride;
    }

    bool showCaption() const
    {
        return mShowCaption;
    }

    int fontSize() const
    {
        return mFontSize;
    }

    int fontWidth() const
    {
        return mFontWidth;
    }

    RgbColorParameters fontColor() const
    {
        return mFontColor;
    }

    Draw3dTextureGen::Draw3dTextureGen textureCorrodinates() const
    {
        return static_cast<Draw3dTextureGen::Draw3dTextureGen>(mTextureCorrodinates);
    }

    int textureAlpha() const
    {
        return mTextureAlpha;
    }

    double textureScale() const
    {
        return mTextureScale;
    }

    int decalMatrixType() const
    {
        return mDecalMatrixType;
    }

    bool decalLeftCam() const
    {
        return mDecalLeftCam;
    }

    int decalLeftAlpha() const
    {
        return mDecalLeftAlpha;
    }

    bool decalRightCam() const
    {
        return mDecalRightCam;
    }

    int decalRightAlpha() const
    {
        return mDecalRightAlpha;
    }

    /* Section with setters */
    void setFovH(double fovH)
    {
        mFovH = fovH;
    }

    void setFovV(double fovV)
    {
        mFovV = fovV;
    }

    void setNearPlane(double nearPlane)
    {
        mNearPlane = nearPlane;
    }

    void setFarPlane(double farPlane)
    {
        mFarPlane = farPlane;
    }

    void setStyle(Draw3dStyle::Draw3dStyle style)
    {
        mStyle = style;
    }

    void setPointColor(RgbColorParameters const &pointColor)
    {
        mPointColor = pointColor;
    }

    void setPointColorOverride(bool pointColorOverride)
    {
        mPointColorOverride = pointColorOverride;
    }

    void setPointSize(int pointSize)
    {
        mPointSize = pointSize;
    }

    void setEdgeColor(RgbColorParameters const &edgeColor)
    {
        mEdgeColor = edgeColor;
    }

    void setEdgeColorOverride(bool edgeColorOverride)
    {
        mEdgeColorOverride = edgeColorOverride;
    }

    void setEdgeWidth(int edgeWidth)
    {
        mEdgeWidth = edgeWidth;
    }

    void setFaceColor(RgbColorParameters const &faceColor)
    {
        mFaceColor = faceColor;
    }

    void setFaceColorOverride(bool faceColorOverride)
    {
        mFaceColorOverride = faceColorOverride;
    }

    void setShowCaption(bool showCaption)
    {
        mShowCaption = showCaption;
    }

    void setFontSize(int fontSize)
    {
        mFontSize = fontSize;
    }

    void setFontWidth(int fontWidth)
    {
        mFontWidth = fontWidth;
    }

    void setFontColor(RgbColorParameters const &fontColor)
    {
        mFontColor = fontColor;
    }

    void setTextureCorrodinates(Draw3dTextureGen::Draw3dTextureGen textureCorrodinates)
    {
        mTextureCorrodinates = textureCorrodinates;
    }

    void setTextureAlpha(int textureAlpha)
    {
        mTextureAlpha = textureAlpha;
    }

    void setTextureScale(double textureScale)
    {
        mTextureScale = textureScale;
    }

    void setDecalMatrixType(int decalMatrixType)
    {
        mDecalMatrixType = decalMatrixType;
    }

    void setDecalLeftCam(bool decalLeftCam)
    {
        mDecalLeftCam = decalLeftCam;
    }

    void setDecalLeftAlpha(int decalLeftAlpha)
    {
        mDecalLeftAlpha = decalLeftAlpha;
    }

    void setDecalRightCam(bool decalRightCam)
    {
        mDecalRightCam = decalRightCam;
    }

    void setDecalRightAlpha(int decalRightAlpha)
    {
        mDecalRightAlpha = decalRightAlpha;
    }

    /* Section with embedded classes */
    /** 
     * \brief Base Parameters 
     * Base Parameters 
     */
    Draw3dParameters baseParameters() const
    {
        return Draw3dParameters(
             style()
           , pointColor()
           , pointColorOverride()
           , pointSize()
           , edgeColor()
           , edgeColorOverride()
           , edgeWidth()
           , faceColor()
           , faceColorOverride()
           , showCaption()
           , fontSize()
           , fontWidth()
           , fontColor()
           , textureCorrodinates()
           , textureAlpha()
           , textureScale()
           , decalMatrixType()
           , decalLeftCam()
           , decalLeftAlpha()
           , decalRightCam()
           , decalRightAlpha()
        );
    }

    void setBaseParameters(Draw3dParameters const &baseParameters)
    {
        mStyle         = baseParameters.style();
        mPointColor    = baseParameters.pointColor();
        mPointColorOverride = baseParameters.pointColorOverride();
        mPointSize     = baseParameters.pointSize();
        mEdgeColor     = baseParameters.edgeColor();
        mEdgeColorOverride = baseParameters.edgeColorOverride();
        mEdgeWidth     = baseParameters.edgeWidth();
        mFaceColor     = baseParameters.faceColor();
        mFaceColorOverride = baseParameters.faceColorOverride();
        mShowCaption   = baseParameters.showCaption();
        mFontSize      = baseParameters.fontSize();
        mFontWidth     = baseParameters.fontWidth();
        mFontColor     = baseParameters.fontColor();
        mTextureCorrodinates = baseParameters.textureCorrodinates();
        mTextureAlpha  = baseParameters.textureAlpha();
        mTextureScale  = baseParameters.textureScale();
        mDecalMatrixType = baseParameters.decalMatrixType();
        mDecalLeftCam  = baseParameters.decalLeftCam();
        mDecalLeftAlpha = baseParameters.decalLeftAlpha();
        mDecalRightCam = baseParameters.decalRightCam();
        mDecalRightAlpha = baseParameters.decalRightAlpha();
    }

    /* visitor pattern - http://en.wikipedia.org/wiki/Visitor_pattern */
template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(mFovH,                      static_cast<const corecvs::DoubleField *>(fields()[FOVH_ID]));
        visitor.visit(mFovV,                      static_cast<const corecvs::DoubleField *>(fields()[FOVV_ID]));
        visitor.visit(mNearPlane,                 static_cast<const corecvs::DoubleField *>(fields()[NEARPLANE_ID]));
        visitor.visit(mFarPlane,                  static_cast<const corecvs::DoubleField *>(fields()[FARPLANE_ID]));
        visitor.visit((int &)mStyle,              static_cast<const corecvs::EnumField *>(fields()[STYLE_ID]));
        visitor.visit(mPointColor,                static_cast<const corecvs::CompositeField *>(fields()[POINT_COLOR_ID]));
        visitor.visit(mPointColorOverride,        static_cast<const corecvs::BoolField *>(fields()[POINT_COLOR_OVERRIDE_ID]));
        visitor.visit(mPointSize,                 static_cast<const corecvs::IntField *>(fields()[POINT_SIZE_ID]));
        visitor.visit(mEdgeColor,                 static_cast<const corecvs::CompositeField *>(fields()[EDGE_COLOR_ID]));
        visitor.visit(mEdgeColorOverride,         static_cast<const corecvs::BoolField *>(fields()[EDGE_COLOR_OVERRIDE_ID]));
        visitor.visit(mEdgeWidth,                 static_cast<const corecvs::IntField *>(fields()[EDGE_WIDTH_ID]));
        visitor.visit(mFaceColor,                 static_cast<const corecvs::CompositeField *>(fields()[FACE_COLOR_ID]));
        visitor.visit(mFaceColorOverride,         static_cast<const corecvs::BoolField *>(fields()[FACE_COLOR_OVERRIDE_ID]));
        visitor.visit(mShowCaption,               static_cast<const corecvs::BoolField *>(fields()[SHOW_CAPTION_ID]));
        visitor.visit(mFontSize,                  static_cast<const corecvs::IntField *>(fields()[FONT_SIZE_ID]));
        visitor.visit(mFontWidth,                 static_cast<const corecvs::IntField *>(fields()[FONT_WIDTH_ID]));
        visitor.visit(mFontColor,                 static_cast<const corecvs::CompositeField *>(fields()[FONT_COLOR_ID]));
        visitor.visit((int &)mTextureCorrodinates, static_cast<const corecvs::EnumField *>(fields()[TEXTURE_CORRODINATES_ID]));
        visitor.visit(mTextureAlpha,              static_cast<const corecvs::IntField *>(fields()[TEXTURE_ALPHA_ID]));
        visitor.visit(mTextureScale,              static_cast<const corecvs::DoubleField *>(fields()[TEXTURE_SCALE_ID]));
        visitor.visit(mDecalMatrixType,           static_cast<const corecvs::IntField *>(fields()[DECAL_MATRIX_TYPE_ID]));
        visitor.visit(mDecalLeftCam,              static_cast<const corecvs::BoolField *>(fields()[DECAL_LEFT_CAM_ID]));
        visitor.visit(mDecalLeftAlpha,            static_cast<const corecvs::IntField *>(fields()[DECAL_LEFT_ALPHA_ID]));
        visitor.visit(mDecalRightCam,             static_cast<const corecvs::BoolField *>(fields()[DECAL_RIGHT_CAM_ID]));
        visitor.visit(mDecalRightAlpha,           static_cast<const corecvs::IntField *>(fields()[DECAL_RIGHT_ALPHA_ID]));
    }

    Draw3dCameraParameters()
    {
        corecvs::DefaultSetter setter;
        accept(setter);
    }

    Draw3dCameraParameters(
          double fovH
        , double fovV
        , double nearPlane
        , double farPlane
        , Draw3dStyle::Draw3dStyle style
        , RgbColorParameters pointColor
        , bool pointColorOverride
        , int pointSize
        , RgbColorParameters edgeColor
        , bool edgeColorOverride
        , int edgeWidth
        , RgbColorParameters faceColor
        , bool faceColorOverride
        , bool showCaption
        , int fontSize
        , int fontWidth
        , RgbColorParameters fontColor
        , Draw3dTextureGen::Draw3dTextureGen textureCorrodinates
        , int textureAlpha
        , double textureScale
        , int decalMatrixType
        , bool decalLeftCam
        , int decalLeftAlpha
        , bool decalRightCam
        , int decalRightAlpha
    )
    {
        mFovH = fovH;
        mFovV = fovV;
        mNearPlane = nearPlane;
        mFarPlane = farPlane;
        mStyle = style;
        mPointColor = pointColor;
        mPointColorOverride = pointColorOverride;
        mPointSize = pointSize;
        mEdgeColor = edgeColor;
        mEdgeColorOverride = edgeColorOverride;
        mEdgeWidth = edgeWidth;
        mFaceColor = faceColor;
        mFaceColorOverride = faceColorOverride;
        mShowCaption = showCaption;
        mFontSize = fontSize;
        mFontWidth = fontWidth;
        mFontColor = fontColor;
        mTextureCorrodinates = textureCorrodinates;
        mTextureAlpha = textureAlpha;
        mTextureScale = textureScale;
        mDecalMatrixType = decalMatrixType;
        mDecalLeftCam = decalLeftCam;
        mDecalLeftAlpha = decalLeftAlpha;
        mDecalRightCam = decalRightCam;
        mDecalRightAlpha = decalRightAlpha;
    }

    bool operator ==(const Draw3dCameraParameters &other) const 
    {
        if ( !(this->mFovH == other.mFovH)) return false;
        if ( !(this->mFovV == other.mFovV)) return false;
        if ( !(this->mNearPlane == other.mNearPlane)) return false;
        if ( !(this->mFarPlane == other.mFarPlane)) return false;
        if ( !(this->mStyle == other.mStyle)) return false;
        if ( !(this->mPointColor == other.mPointColor)) return false;
        if ( !(this->mPointColorOverride == other.mPointColorOverride)) return false;
        if ( !(this->mPointSize == other.mPointSize)) return false;
        if ( !(this->mEdgeColor == other.mEdgeColor)) return false;
        if ( !(this->mEdgeColorOverride == other.mEdgeColorOverride)) return false;
        if ( !(this->mEdgeWidth == other.mEdgeWidth)) return false;
        if ( !(this->mFaceColor == other.mFaceColor)) return false;
        if ( !(this->mFaceColorOverride == other.mFaceColorOverride)) return false;
        if ( !(this->mShowCaption == other.mShowCaption)) return false;
        if ( !(this->mFontSize == other.mFontSize)) return false;
        if ( !(this->mFontWidth == other.mFontWidth)) return false;
        if ( !(this->mFontColor == other.mFontColor)) return false;
        if ( !(this->mTextureCorrodinates == other.mTextureCorrodinates)) return false;
        if ( !(this->mTextureAlpha == other.mTextureAlpha)) return false;
        if ( !(this->mTextureScale == other.mTextureScale)) return false;
        if ( !(this->mDecalMatrixType == other.mDecalMatrixType)) return false;
        if ( !(this->mDecalLeftCam == other.mDecalLeftCam)) return false;
        if ( !(this->mDecalLeftAlpha == other.mDecalLeftAlpha)) return false;
        if ( !(this->mDecalRightCam == other.mDecalRightCam)) return false;
        if ( !(this->mDecalRightAlpha == other.mDecalRightAlpha)) return false;
        return true;
    }
    friend std::ostream& operator << (std::ostream &out, Draw3dCameraParameters &toSave)
    {
        corecvs::PrinterVisitor printer(out);
        toSave.accept<corecvs::PrinterVisitor>(printer);
        return out;
    }

    void print ()
    {
        std::cout << *this;
    }
};
#endif  //DRAW_3D_CAMERA_PARAMETERS_H_
