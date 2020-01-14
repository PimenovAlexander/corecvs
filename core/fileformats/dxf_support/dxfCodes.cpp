//
// Created by Myasnikov Vladislav on 21.10.2019.
//

#include "core/fileformats/dxf_support/dxfCodes.h"

namespace corecvs {

const std::string DxfCodes::DXF_LINE_TYPE_NAME_DEFAULT = "BYLAYER";

std::map<std::string, std::vector<int>> DxfCodes::VARIABLE_CODES = {
        {"$ACADMAINTVER",{70}},{"$ACADVER",{1}},{"$ANGBASE",{50}},{"$ANGDIR",{70}},{"$ATTMODE",{70}},
        {"$AUNITS",{70}},{"$AUPREC",{70}},{"$CECOLOR",{62}},{"$CELTSCALE",{40}},{"$CELTYPE",{6}},
        {"$CELWEIGHT",{370}},{"$CEPSNID",{390}},{"$CEPSNTYPE",{380}},{"$CHAMFERA",{40}},{"$CHAMFERB",{40}},
        {"$CHAMFERC",{40}},{"$CHAMFERD",{40}},{"$CLAYER",{8}},{"$CMLJUST",{70}},{"$CMLSCALE",{40}},
        {"$CMLSTYLE",{2}},{"$CSHADOW",{280}},{"$DIMADEC",{70}},{"$DIMALT",{70}},{"$DIMALTD",{70}},
        {"$DIMALTF",{40}},{"$DIMALTRND",{40}},{"$DIMALTTD",{70}},{"$DIMALTTZ",{70}},{"$DIMALTU",{70}},
        {"$DIMALTZ",{70}},{"$DIMAPOST",{1}},{"$DIMASO",{70}},{"$DIMASSOC",{280}},{"$DIMASZ",{40}},
        {"$DIMATFIT",{70}},{"$DIMAUNIT",{70}},{"$DIMAZIN",{70}},{"$DIMBLK",{1}},{"$DIMBLK1",{1}},
        {"$DIMBLK2",{1}},{"$DIMCEN",{40}},{"$DIMCLRD",{70}},{"$DIMCLRE",{70}},{"$DIMCLRT",{70}},
        {"$DIMDEC",{70}},{"$DIMDLE",{40}},{"$DIMDLI",{40}},{"$DIMDSEP",{70}},{"$DIMEXE",{40}},
        {"$DIMEXO",{40}},{"$DIMFAC",{40}},{"$DIMGAP",{40}},{"$DIMJUST",{70}},{"$DIMLDRBLK",{1}},
        {"$DIMLFAC",{40}},{"$DIMLIM",{70}},{"$DIMLUNIT",{70}},{"$DIMLWD",{70}},{"$DIMLWE",{70}},
        {"$DIMPOST",{1}},{"$DIMRND",{40}},{"$DIMSAH",{70}},{"$DIMSCALE",{40}},{"$DIMSD1",{70}},
        {"$DIMSD2",{70}},{"$DIMSE1",{70}},{"$DIMSE2",{70}},{"$DIMSHO",{70}},{"$DIMSOXD",{70}},
        {"$DIMSTYLE",{2}},{"$DIMTAD",{70}},{"$DIMTDEC",{70}},{"$DIMTFAC",{40}},{"$DIMTIH",{70}},
        {"$DIMTIX",{70}},{"$DIMTM",{40}},{"$DIMTMOVE",{70}},{"$DIMTOFL",{70}},{"$DIMTOH",{70}},
        {"$DIMTOL",{70}},{"$DIMTOLJ",{70}},{"$DIMTP",{40}},{"$DIMTSZ",{40}},{"$DIMTVP",{40}},
        {"$DIMTXSTY",{7}},{"$DIMTXT",{40}},{"$DIMTZIN",{70}},{"$DIMUPT",{70}},{"$DIMZIN",{70}},
        {"$DISPSILH",{70}},{"$DRAGVS",{349}},{"$DWGCODEPAGE",{3}},{"$ELEVATION",{40}},{"$ENDCAPS",{280}},
        {"$EXTMAX",{10,20,30}},{"$EXTMIN",{10,20,30}},{"$EXTNAMES",{290}},{"$FILLETRAD",{40}},{"$FILLMODE",{70}},
        {"$FINGERPRINTGUID",{2}},{"$HALOGAP",{280}},{"$HANDSEED",{5}},{"$HIDETEXT",{290}},{"$HYPERLINKBASE",{1}},
        {"$INDEXCTL",{280}},{"$INSBASE",{10,20,30}},{"$INSUNITS",{70}},{"$INTERFERECOLOR",{62}},{"$INTERFEREOBJVS",{345}},
        {"$INTERFEREVPVS",{346}},{"$INTERSECTIONCOLOR",{70}},{"$INTERSECTIONDISPLAY",{290}},{"$JOINSTYLE",{280}},{"$LIMCHECK",{70}},
        {"$LIMMAX",{10,20}},{"$LIMMIN",{10,20}},{"$LTSCALE",{40}},{"$LUNITS",{70}},{"$LUPREC",{70}},
        {"$LWDISPLAY",{290}},{"$MAXACTVP",{70}},{"$MEASUREMENT",{70}},{"$MENU",{1}},{"$MIRRTEXT",{70}},
        {"$OBSCOLOR",{70}},{"$OBSLTYPE",{280}},{"$ORTHOMODE",{70}},{"$PDMODE",{70}},{"$PDSIZE",{40}},
        {"$PELEVATION",{40}},{"$PEXTMAX",{10,20,30}},{"$PEXTMIN",{10,20,30}},{"$PINSBASE",{10,20,30}},{"$PLIMCHECK",{70}},
        {"$PLIMMAX",{10,20}},{"$PLIMMIN",{10,20}},{"$PLINEGEN",{70}},{"$PLINEWID",{40}},{"$PROJECTNAME",{1}},
        {"$PROXYGRAPHICS",{70}},{"$PSLTSCALE",{70}},{"$PSTYLEMODE",{290}},{"$PSVPSCALE",{40}},{"$PUCSBASE",{2}},
        {"$PUCSNAME",{2}},{"$PUCSORG",{10,20,30}},{"$PUCSORGBACK",{10,20,30}},{"$PUCSORGBOTTOM",{10,20,30}},{"$PUCSORGFRONT",{10,20,30}},
        {"$PUCSORGLEFT",{10,20,30}},{"$PUCSORGRIGHT",{10,20,30}},{"$PUCSORGTOP",{10,20,30}},{"$PUCSORTHOREF",{2}},{"$PUCSORTHOVIEW",{70}},
        {"$PUCSXDIR",{10,20,30}},{"$PUCSYDIR",{10,20,30}},{"$QTEXTMODE",{70}},{"$REGENMODE",{70}},{"$SHADEDGE",{70}},
        {"$SHADEDIF",{70}},{"$SHADOWPLANELOCATION",{40}},{"$SKETCHINC",{40}},{"$SKPOLY",{70}},{"$SORTENTS",{280}},
        {"$SPLINESEGS",{70}},{"$SPLINETYPE",{70}},{"$SURFTAB1",{70}},{"$SURFTAB2",{70}},{"$SURFTYPE",{70}},
        {"$SURFU",{70}},{"$SURFV",{70}},{"$TDCREATE",{40}},{"$TDINDWG",{40}},{"$TDUCREATE",{40}},
        {"$TDUPDATE",{40}},{"$TDUSRTIMER",{40}},{"$TDUUPDATE",{40}},{"$TEXTSIZE",{40}},{"$TEXTSTYLE",{7}},
        {"$THICKNESS",{40}},{"$TILEMODE",{70}},{"$TRACEWID",{40}},{"$TREEDEPTH",{70}},{"$UCSBASE",{2}},
        {"$UCSNAME",{2}},{"$UCSORG",{10,20,30}},{"$UCSORGBACK",{10,20,30}},{"$UCSORGBOTTOM",{10,20,30}},{"$UCSORGFRONT",{10,20,30}},
        {"$UCSORGLEFT",{10,20,30}},{"$UCSORGRIGHT",{10,20,30}},{"$UCSORGTOP",{10,20,30}},{"$UCSORTHOREF",{2}},{"$UCSORTHOVIEW",{70}},
        {"$UCSXDIR",{10,20,30}},{"$UCSYDIR",{10,20,30}},{"$UNITMODE",{70}},{"$USERI1",{70}},{"$USERI2",{70}},
        {"$USERI3",{70}},{"$USERI4",{70}},{"$USERI5",{70}},{"$USERR1",{40}},{"$USERR2",{40}},
        {"$USERR3",{40}},{"$USERR4",{40}},{"$USERR5",{40}},{"$USRTIMER",{70}},{"$VERSIONGUID",{2}},
        {"$VISRETAIN",{70}},{"$WORLDVIEW",{70}},{"$XCLIPFRAME",{290}},{"$XEDIT",{290}}
};

std::map<std::string, DxfElementType> DxfCodes::ELEMENT_TYPES = {
        {"LAYER",DxfElementType::DXF_LAYER},{"LTYPE",DxfElementType::DXF_LINE_TYPE},{"LINE",DxfElementType::DXF_LINE},{"LWPOLYLINE",DxfElementType::DXF_LW_POLYLINE},
        {"POLYLINE",DxfElementType::DXF_POLYLINE},{"VERTEX",DxfElementType::DXF_VERTEX},{"SEQEND",DxfElementType::DXF_SEQ_END},{"CIRCLE", DxfElementType::DXF_CIRCLE},
        {"ARC",DxfElementType::DXF_ARC},{"ELLIPSE", DxfElementType::DXF_ELLIPSE},{"POINT", DxfElementType::DXF_POINT},{"BLOCK_RECORD",DxfElementType::DXF_BLOCK_RECORD},
        {"BLOCK",DxfElementType::DXF_BLOCK},{"ENDBLK",DxfElementType::DXF_END_BLOCK},{"INSERT",DxfElementType::DXF_INSERT}
};

std::vector<std::vector<uint8_t>> DxfCodes::DXF_COLORS = {
        {0,0,0},{255,0,0},{255,255,0},{0,255,0},{0,255,255},{0,0,255},{255,0,255},{0,0,0},
        {128,128,128},{192,192,192},{255,0,0},{255,127,127},{165,0,0},{165,82,82},{127,0,0},{127,63,63},
        {76,0,0},{76,38,38},{38,0,0},{38,19,19},{255,63,0},{255,159,127},{165,41,0},{165,103,82},
        {127,31,0},{127,79,63},{76,19,0},{76,47,38},{38,9,0},{38,23,19},{255,127,0},{255,191,127},
        {165,82,0},{165,124,82},{127,63,0},{127,95,63},{76,38,0},{76,57,38},{38,19,0},{38,28,19},
        {255,191,0},{255,223,127},{165,124,0},{165,145,82},{127,95,0},{127,111,63},{76,57,0},{76,66,38},
        {38,28,0},{38,33,19},{255,255,0},{255,255,127},{165,165,0},{165,165,82},{127,127,0},{127,127,63},
        {76,76,0},{76,76,38},{38,38,0},{38,38,19},{191,255,0},{223,255,127},{124,165,0},{145,165,82},
        {95,127,0},{111,127,63},{57,76,0},{66,76,38},{28,38,0},{33,38,19},{127,255,0},{191,255,127},
        {82,165,0},{124,165,82},{63,127,0},{95,127,63},{38,76,0},{57,76,38},{19,38,0},{28,38,19},
        {63,255,0},{159,255,127},{41,165,0},{103,165,82},{31,127,0},{79,127,63},{19,76,0},{47,76,38},
        {9,38,0},{23,38,19},{0,255,0},{127,255,127},{0,165,0},{82,165,82},{0,127,0},{63,127,63},
        {0,76,0},{38,76,38},{0,38,0},{19,38,19},{0,255,63},{127,255,159},{0,165,41},{82,165,103},
        {0,127,31},{63,127,79},{0,76,19},{38,76,47},{0,38,9},{19,38,23},{0,255,127},{127,255,191},
        {0,165,82},{82,165,124},{0,127,63},{63,127,95},{0,76,38},{38,76,57},{0,38,19},{19,38,28},
        {0,255,191},{127,255,223},{0,165,124},{82,165,145},{0,127,95},{63,127,111},{0,76,57},{38,76,66},
        {0,38,28},{19,38,33},{0,255,255},{127,255,255},{0,165,165},{82,165,165},{0,127,127},{63,127,127},
        {0,76,76},{38,76,76},{0,38,38},{19,38,38},{0,191,255},{127,223,255},{0,124,165},{82,145,165},
        {0,95,127},{63,111,127},{0,57,76},{38,66,76},{0,28,38},{19,33,38},{0,127,255},{127,191,255},
        {0,82,165},{82,124,165},{0,63,127},{63,95,127},{0,38,76},{38,57,76},{0,19,38},{19,28,38},
        {0,63,255},{127,159,255},{0,41,165},{82,103,165},{0,31,127},{63,79,127},{0,19,76},{38,47,76},
        {0,9,38},{19,23,38},{0,0,255},{127,127,255},{0,0,165},{82,82,165},{0,0,127},{63,63,127},
        {0,0,76},{38,38,76},{0,0,38},{19,19,38},{63,0,255},{159,127,255},{41,0,165},{103,82,165},
        {31,0,127},{79,63,127},{19,0,76},{47,38,76},{9,0,38},{23,19,38},{127,0,255},{191,127,255},
        {82,0,165},{124,82,165},{63,0,127},{95,63,127},{38,0,76},{57,38,76},{19,0,38},{28,19,38},
        {191,0,255},{223,127,255},{124,0,165},{145,82,165},{95,0,127},{111,63,127},{57,0,76},{66,38,76},
        {28,0,38},{33,19,38},{255,0,255},{255,127,255},{165,0,165},{165,82,165},{127,0,127},{127,63,127},
        {76,0,76},{76,38,76},{38,0,38},{38,19,38},{255,0,191},{255,127,223},{165,0,124},{165,82,145},
        {127,0,95},{127,63,111},{76,0,57},{76,38,66},{38,0,28},{38,19,33},{255,0,127},{255,127,191},
        {165,0,82},{165,82,124},{127,0,63},{127,63,95},{76,0,38},{76,38,57},{38,0,19},{38,19,28},
        {255,0,63},{255,127,159},{165,0,41},{165,82,103},{127,0,31},{127,63,79},{76,0,19},{76,38,47},
        {38,0,9},{38,19,23},{84,84,84},{118,118,118},{160,160,160},{192,192,192},{224,224,224},{0,0,0}
};

std::map<int, DxfDrawingUnits> DxfCodes::DRAWING_UNITS = {
        {0,DxfDrawingUnits::UNITLESS},{1,DxfDrawingUnits::INCHES},{2,DxfDrawingUnits::FEET},{3,DxfDrawingUnits::MILES},
        {4,DxfDrawingUnits::MILLIMETERS},{5,DxfDrawingUnits::CENTIMETERS},{6,DxfDrawingUnits::METERS},{7,DxfDrawingUnits::KILOMETERS},
        {8,DxfDrawingUnits::MICROINCHES},{9,DxfDrawingUnits::MILS},{10,DxfDrawingUnits::YARDS},{11,DxfDrawingUnits::ANGSTROMS},
        {12,DxfDrawingUnits::NANOMETERS},{13,DxfDrawingUnits::MICRONS},{14,DxfDrawingUnits::DECIMETERS},{15,DxfDrawingUnits::DECAMETERS},
        {16,DxfDrawingUnits::HECTOMETERS},{17,DxfDrawingUnits::GIGAMETERS},{18,DxfDrawingUnits::ASTRONOMICAL},{19,DxfDrawingUnits::LIGHT_YEARS},
        {20,DxfDrawingUnits::PARSECS}
};

int DxfCodes::CODE_RANGES[][3] = {
        {0,9,DXF_STRING},{10,59,DXF_DOUBLE},{60,79,DXF_INT16},{90,99,DXF_INT32},
        {100,100,DXF_STRING},{102,102,DXF_STRING},{105,105,DXF_STRING},{110,149,DXF_DOUBLE},
        {160,169,DXF_INT64},{170,179,DXF_INT16},{210,239,DXF_DOUBLE},{270,299,DXF_INT16},
        {300,369,DXF_STRING},{370,389,DXF_STRING},{390,399,DXF_STRING},{400,409,DXF_INT16},
        {410,419,DXF_STRING},{420,429,DXF_INT32},{430,439,DXF_STRING},{440,449,DXF_INT32},
        {450,459,DXF_INT64},{460,469,DXF_DOUBLE},{470,481,DXF_STRING},{999,1009,DXF_STRING},
        {1010,1059,DXF_DOUBLE},{1060,1070,DXF_INT16},{1071,1071,DXF_INT32}
};

} // namespace corecvs
