#include <vector>
#include "buffers/correspondenceList.h"
#include "rectification/essentialEstimator.h"

using namespace std;
namespace corecvs {

Matrix derivative2(const double in[], const vector<Correspondence *> *samples)
{
    Matrix result((int)samples->size(), EssentialEstimator::CostFunctionBase::VECTOR_SIZE, false);
    double Qx = in[EssentialEstimator::CostFunctionBase::ROTATION_Q_X]; 
    double Qy = in[EssentialEstimator::CostFunctionBase::ROTATION_Q_Y]; 
    double Qz = in[EssentialEstimator::CostFunctionBase::ROTATION_Q_Z]; 
    double Qt = in[EssentialEstimator::CostFunctionBase::ROTATION_Q_T]; 
    double Tx = in[EssentialEstimator::CostFunctionBase::TRANSLATION_X]; 
    double Ty = in[EssentialEstimator::CostFunctionBase::TRANSLATION_Y]; 
    double Tz = in[EssentialEstimator::CostFunctionBase::TRANSLATION_Z]; 
   double m[9];
   double md[7 * 9 + 7];
   const double cse1 = 0-Ty;
   const double cse3c = Qz*Qz;
   const double cse43 = 0-Tz;
   const double cse38 = Qx*Qx;
   const double cse30 = Qx+Qx;
   const double cse58 = Qy*Qy;
   const double cse5c = 0-Tx;
   const double cse24 = Qz+Qz;
   const double cse22 = Qt+Qt;
   const double cse64 = Qy+Qy;
   const double cse92 = Qt*Qt;
   const double cse2a = cse38+cse58;
   const double cse70 = (cse2a)+cse3c;
   const double cse48 = (cse70)+cse92;
   const double cse32 = sqrt(cse48);
   const double cse5e = cse32*(cse24);
   const double csea3 = cse32*cse32;
   const double cse10 = cse32*(cse64);
   const double cse5b = cse32*(cse30);
   const double cse15 = cse32*(cse22);
   const double cse54 = 2/cse32;
   const double csea8 = Qz*(cse54);
   const double cse59 = 0.5*(cse5b);
   const double cse2b = Qx*(cse54);
   const double cse6a = 0.5*(cse10);
   const double cse4e = Qy*(cse54);
   const double cse14 = 0.5*(cse15);
   const double cse34 = 0.5*(cse5e);
   const double cse51 = 2*(cse59);
   const double cse31 = Qx*(cse4e);
   const double cse55 = Qx*(csea8);
   const double cse47 = Qt*(csea8);
   const double cse3a = Qt*(cse2b);
   const double cse8c = Qy*(cse4e);
   const double cse71 = Qy*(csea8);
   const double cse61 = 2*(cse34);
   const double cse62 = Qx*(cse2b);
   const double cse90 = 2*(cse14);
   const double cse17 = 2*(cse6a);
   const double cse1a = Qt*(cse4e);
   const double cse6d = Qz*(csea8);
   const double cse5f = 0-cse51;
   const double cse8e = cse8c+cse6d;
   const double cse37 = cse31+cse47;
   const double cse12 = cse62+cse6d;
   const double cse13 = cse55+cse1a;
   const double cse9c = cse55-cse1a;
   const double cse53 = 0-cse17;
   const double cse1b = 0-cse61;
   const double cse67 = cse62+cse8c;
   const double cse63 = cse71+cse3a;
   const double cse9a = cse71-cse3a;
   const double cse7 = cse31-cse47;
   const double cse91 = 0-cse90;
   const double cse6e = 1-(cse12);
   const double cse49 = (cse91)/(csea3);
   const double cse6f = (cse53)/(csea3);
   const double cse44 = (cse5f)/(csea3);
   const double cse7d = 1-(cse67);
   const double csea9 = 1-(cse8e);
   const double cse23 = (cse1b)/(csea3);
   const double cse2e = Qz*(cse49);
   const double cse16 = Qy*(cse23);
   const double cse74 = Qx*(cse6f);
   const double cse42 = Qy*(cse6f);
   const double cse4d = Qx*(cse49);
   const double cse89 = Qz*(cse23);
   const double cse4c = Qy*(cse49);
   const double cse29 = Qz*(cse6f);
   const double cse76 = Qx*(cse44);
   const double cse21 = Qx*(cse23);
   const double cse79 = Qz*(cse44);
   const double cse52 = Qy*(cse44);
   const double cse20 = Qt*(cse29);
   const double cse1d = Qx*(cse16);
   const double cse5d = Qt*(cse16);
   const double cse25 = Qt*(cse52);
   const double cse26 = Qy*(cse16);
   const double cse7c = Qx*(cse74);
   const double cse1f = cse54+cse42;
   const double cse7a = Qz*(cse79);
   const double cse57 = Qx*(cse29);
   const double cse78 = Qy*(cse79);
   const double cse97 = Qy*(cse2e);
   const double cse69 = Qt*(cse2e);
   const double cse75 = cse54+cse76;
   const double cse9b = Qy*(cse52);
   const double csee = Qx*(cse21);
   const double csef = Qx*(cse2e);
   const double cse7f = Qt*(cse21);
   const double cse3f = Qx*(cse4d);
   const double cse40 = cse54+cse89;
   const double cse3b = Qx*(cse4c);
   const double csea5 = Qy*(cse4c);
   const double csea4 = Qx*(cse79);
   const double cse33 = Qx*(cse52);
   const double cse93 = Qz*(cse2e);
   const double cse4b = Qz*(cse29);
   const double cse95 = Qt*(cse4d);
   const double cse81 = Qt*(cse74);
   const double cse2d = Qt*(cse79);
   const double cse2c = Qt*(cse4c);
   const double cse80 = Qy*(cse29);
   const double cse94 = cse3f+cse93;
   const double cse7b = Qt*(cse75);
   const double cse9 = csea5+cse93;
   const double cse88 = Qz*(cse40);
   const double cse8b = Qy*(cse40);
   const double cse73 = cse4e+cse33;
   const double cse65 = cse7c+cse4b;
   const double csed = cse4e+cse2c;
   const double csec = cse9b+cse7a;
   const double cse11 = cse2b+cse95;
   const double cse4f = Qx*(cse1f);
   const double cse50 = Qy*(cse1f);
   const double csea0 = csee+cse26;
   const double cse28 = Qt*(cse1f);
   const double cse60 = csea8+csea4;
   const double csea6 = csea8+cse69;
   const double csea7 = cse3f+csea5;
   const double cse66 = csea8+cse80;
   const double cse1c = Qx*(cse75);
   const double cse68 = Qx*(cse40);
   const double cse19 = Qt*(cse40);
   const double cse8f = cse8b+cse7f;
   const double cse4 = cse3b-(csea6);
   const double cse8 = 0-(cse9);
   const double cse8d = (cse60)+cse25;
   const double cse3 = cse57+cse28;
   const double cse3d = (cse60)-cse25;
   const double csea2 = 0-(csea0);
   const double csea1 = cse97-(cse11);
   const double cse96 = 0-(cse94);
   const double cse9f = cse8b-cse7f;
   const double cse9d = cse4e+cse50;
   const double cse5 = cse1d-cse19;
   const double cse2 = cse68+cse5d;
   const double cse6 = (cse73)-cse2d;
   const double cse99 = cse3b+(csea6);
   const double cse98 = cse97+(cse11);
   const double cse77 = (cse73)+cse2d;
   const double cse2f = csef-(csed);
   const double cse4a = csef+(csed);
   const double cse5a = cse2b+cse1c;
   const double cse1e = cse57-cse28;
   const double cse6b = csea8+cse88;
   const double cse36 = cse68-cse5d;
   const double cse18 = cse1d+cse19;
   const double cse46 = 0-(csec);
   const double cse7e = 0-(cse65);
   const double cse82 = (cse66)-cse81;
   const double cse83 = cse78-cse7b;
   const double cse84 = cse78+cse7b;
   const double cse86 = (cse66)+cse81;
   const double cse39 = cse4f-cse20;
   const double cse87 = 0-(csea7);
   const double cse41 = cse4f+cse20;
   const double cse3e = cse26+(cse6b);
   const double cse45 = csee+(cse6b);
   const double cse35 = (cse5a)+cse7a;
   const double cse56 = (cse5a)+cse9b;
   const double cse9e = cse7c+(cse9d);
   const double cseb = (cse9d)+cse4b;
   const double cse27 = 0-(cse56);
   const double cse6c = 0-(cse9e);
   const double cse72 = 0-(cseb);
   const double cse85 = 0-(cse35);
   const double csea = 0-(cse3e);
   const double cse8a = 0-(cse45);
m[0] = (cse43)*(cse37)+Ty*(cse9c);
md[0] = (cse43)*(cse77)+Ty*(cse3d);
md[1] = (cse43)*(cse41)+Ty*(cse1e);
md[2] = (cse43)*(cse18)+Ty*(cse36);
md[3] = (cse43)*(cse99)+Ty*(cse2f);
md[4] = 0;
md[5] = cse9c;
md[6] = (-1)*(cse37);
m[1] = (cse43)*(cse6e)+Ty*(cse63);
md[7] = (cse43)*(cse85)+Ty*(cse84);
md[8] = (cse43)*(cse7e)+Ty*(cse86);
md[9] = (cse43)*(cse8a)+Ty*(cse8f);
md[10] = (cse43)*(cse96)+Ty*(cse98);
md[11] = 0;
md[12] = cse63;
md[13] = (-1)*(cse6e);
m[2] = (cse43)*(cse9a)+Ty*(cse7d);
md[14] = (cse43)*(cse83)+Ty*(cse27);
md[15] = (cse43)*(cse82)+Ty*(cse6c);
md[16] = (cse43)*(cse9f)+Ty*(csea2);
md[17] = (cse43)*(csea1)+Ty*(cse87);
md[18] = 0;
md[19] = cse7d;
md[20] = (-1)*(cse9a);
m[3] = Tz*(csea9)+(cse5c)*(cse9c);
md[21] = Tz*(cse46)+(cse5c)*(cse3d);
md[22] = Tz*(cse72)+(cse5c)*(cse1e);
md[23] = Tz*(csea)+(cse5c)*(cse36);
md[24] = Tz*(cse8)+(cse5c)*(cse2f);
md[25] = (-1)*(cse9c);
md[26] = 0;
md[27] = csea9;
m[4] = Tz*(cse7)+(cse5c)*(cse63);
md[28] = Tz*(cse6)+(cse5c)*(cse84);
md[29] = Tz*(cse39)+(cse5c)*(cse86);
md[30] = Tz*(cse5)+(cse5c)*(cse8f);
md[31] = Tz*(cse4)+(cse5c)*(cse98);
md[32] = (-1)*(cse63);
md[33] = 0;
md[34] = cse7;
m[5] = Tz*(cse13)+(cse5c)*(cse7d);
md[35] = Tz*(cse8d)+(cse5c)*(cse27);
md[36] = Tz*(cse3)+(cse5c)*(cse6c);
md[37] = Tz*(cse2)+(cse5c)*(csea2);
md[38] = Tz*(cse4a)+(cse5c)*(cse87);
md[39] = (-1)*(cse7d);
md[40] = 0;
md[41] = cse13;
m[6] = (cse1)*(csea9)+Tx*(cse37);
md[42] = (cse1)*(cse46)+Tx*(cse77);
md[43] = (cse1)*(cse72)+Tx*(cse41);
md[44] = (cse1)*(csea)+Tx*(cse18);
md[45] = (cse1)*(cse8)+Tx*(cse99);
md[46] = cse37;
md[47] = (-1)*(csea9);
md[48] = 0;
m[7] = (cse1)*(cse7)+Tx*(cse6e);
md[49] = (cse1)*(cse6)+Tx*(cse85);
md[50] = (cse1)*(cse39)+Tx*(cse7e);
md[51] = (cse1)*(cse5)+Tx*(cse8a);
md[52] = (cse1)*(cse4)+Tx*(cse96);
md[53] = cse6e;
md[54] = (-1)*(cse7);
md[55] = 0;
m[8] = (cse1)*(cse13)+Tx*(cse9a);
md[56] = (cse1)*(cse8d)+Tx*(cse83);
md[57] = (cse1)*(cse3)+Tx*(cse82);
md[58] = (cse1)*(cse2)+Tx*(cse9f);
md[59] = (cse1)*(cse4a)+Tx*(csea1);
md[60] = cse9a;
md[61] = (-1)*(cse13);
md[62] = 0;

   for (size_t i = 0; i < samples->size(); i++)
   {
        double startx = (*samples)[i]->start.x();
        double starty = (*samples)[i]->start.y();
        double endx   = (*samples)[i]->end.x();
        double endy   = (*samples)[i]->end.y();

        double m1 = (startx*m[0]+starty*m[3])+m[6];
        double m2 = (startx*m[1]+starty*m[4])+m[7];
        double m3 = (startx*m[2]+starty*m[5])+m[8];

        double denum = (m1)*(m1)+(m2)*(m2);
        double sqrt1 = sqrt(denum);

        double v1 = (m1*endx+m2*endy+m3);

        double v = v1 /sqrt1;

#ifndef WITH_AVX2
         for (int j = 0; j < 7; j++)
         {
                double l1 = (startx*md[0+j]+starty*md[21+j])+md[42+j];
                double l2 = (startx*md[7+j]+starty*md[28+j])+md[49+j];
                double l3 = (startx*md[14+j]+starty*md[35+j])+md[56+j];

                double r =
                ((l1*endx + l2*endy + l3)*sqrt1-
                 (v1)*((m1*l1+l2*m2) / sqrt1))/denum;

                result.element((int)i, (int)j) = (v < 0) ? -r : r;
         }
#else
         {
             Doublex4 vstartx(startx);
             Doublex4 vstarty(starty);
             Doublex4 vendx(endx);
             Doublex4 vendy(endy);

             Doublex4 vm1(m1);
             Doublex4 vm2(m2);

             Doublex4 vdenum(denum);
             Doublex4 vsqrt1(sqrt1);
             Doublex4 vv1(v1);

             Doublex4 l1 = (vstartx * Doublex4(&md[ 0]) + vstarty * Doublex4(&md[21]) + Doublex4(&md[42]));
             Doublex4 l2 = (vstartx * Doublex4(&md[ 7]) + vstarty * Doublex4(&md[28]) + Doublex4(&md[49]));
             Doublex4 l3 = (vstartx * Doublex4(&md[14]) + vstarty * Doublex4(&md[35]) + Doublex4(&md[56]));

             Doublex4 r =
             ((l1*vendx + l2*vendy + l3)*vsqrt1-(vv1)*((vm1*l1+l2*vm2) / vsqrt1))/vdenum;

             if (v < 0) r = -r;
             r.save(&result.element(i,0));

             Doublex4 l1a = (vstartx * Doublex4(&md[ 0 + 4]) + vstarty * Doublex4(&md[21 + 4]) + Doublex4(&md[42 + 4]));
             Doublex4 l2a = (vstartx * Doublex4(&md[ 7 + 4]) + vstarty * Doublex4(&md[28 + 4]) + Doublex4(&md[49 + 4]));
             Doublex4 l3a = (vstartx * Doublex4(&md[14 + 4]) + vstarty * Doublex4(&md[35 + 4]) + Doublex4(&md[56 + 4]));

             Doublex4 r1 =
             ((l1a*vendx + l2a*vendy + l3a)*vsqrt1-(vv1)*((vm1*l1a+l2a*vm2) / vsqrt1))/vdenum;

             if (v < 0) r1 = -r1;
             r1.save(&result.element(i,4));


             /*for (int j = 4; j < 7; j++)
             {
                    double l1 = (startx*md[0+j]+starty*md[21+j])+md[42+j];
                    double l2 = (startx*md[7+j]+starty*md[28+j])+md[49+j];
                    double l3 = (startx*md[14+j]+starty*md[35+j])+md[56+j];

                    double r =
                    ((l1*endx + l2*endy + l3)*sqrt1-
                     (v1)*((m1*l1+l2*m2) / sqrt1))/denum;

                    result.element(i,j) = (v < 0) ? -r : r;
             }*/

         }
#endif



   }
   return result;
}
} // namespace
