/*! \page pDistortionCorrection Distortion correction documentation page

  \author Osechkina Maria

\section Introduction
    This document is devoted to the implementation and details of spherical distortion correction.
\section Problem formulation
    For correction of cameras distortion we propose to find lines, that are
straights in reality, and to construct a transformation, that straighten this lines.
Us usual straights are mapped to arcs after cameras distortion. We have to
construct reverse transform, that should maps some arcs to straights.

\section Algorithms
    In this section seveal algorithms of distortion correction are described and reffered to: \n
    \ref subSectionInversion \n
    \ref subSectionBarrel \n
    \ref subSectionTaylor \n

\subsection subSectionInversion Inversion
    We assume that cameras distortion could be presented us sequence of projection
of space to the plane, map of part plane to the segment of sphere and another projection.
It is necessary to get image closed to the first projection. We have to construct
continuous transform that maps sphere to the plane. As such transform we use inversion of the space
http://en.wikipedia.org/wiki/Inversive_geometry

    Let \f$(x_0, y_0, z_0)\f$ is a point on the sphere with center in the origin and radius \f$R\f$.
Let \f$(0, 0, -R)\f$ is the center of inverion, i. e. sphere contains inversion center and is mapped into the plane.
Let \f$r_0\f$  is inversion radius. The image of point is the point \f$(x_1, y_1, z_1)\f$ belonging to the ray with initial point
in inversion center, which contains preimage of point.
By the inversion definition \f$dist((x_0, y_0, z_0), (0, 0, -R)) * dist((x_1, y_1, z_1), (0, 0, -R)) = r_0 ^ 2 \f$. \n
  \f$(x_0, y_0, z_0)  \in B((0, 0, 0), R)\f$ then \f$z_0 = \sqrt(R ^ 2 - x_0 ^ 2 - y_0 ^ 2) \f$ \n
  \f$\sqrt(x_0 ^ 2 + y_0 ^ 2 + (\sqrt{R ^ 2 - x_0 ^ 2 - y_0 ^ 2} + R) ^ 2) * \sqrt(x_1 ^ 2 + y_1 ^2 + (z_1 + R) ^ 2) = r_0 ^ 2\f$ \n
   \f$(x_1, y_1, z_1)\f$ , \f$(x_0, y_0, z_0)\f$ , \f$(0, 0, -R)\f$ lie on the same line, then
   \f$(x_1, y_1, z_1) = (x_0 * a, y_0 * a, - R + a *(z_0 + R))\f$ \n

  \f$\sqrt{2 * R ^ 2 + 2 * R * \sqrt{R ^ 2 - x_0 ^ 2 - y_0 ^ 2}} * \sqrt{(a*x_0) ^2  + (a * y_0) ^ 2 + (a * (z_0 + R)) ^ 2} = r_0 ^ 2\f$ ,i e
\f$ \left | a \right | *(2 * R ^ 2 + 2 * R * (R ^ 2 - x_0 ^ 2 - y_0 ^ 2)) = r_0^2\f$  \n

\f$a\f$ is positive by the choice of ray direction. \n

    Image of the point \f$(x_0, y_0, z_0)\f$ is
\f$(C * x_0 \over {R + \sqrt{R ^ 2 - x_0 ^ 2 - y_0 ^ 2}}, C * y_0 \over {R + \sqrt{R ^ 2 - x_0 ^ 2 - y_0 ^ 2}}, C * z_0 \over {R + \sqrt{R ^ 2 - x_0 ^ 2 - y_0 ^ 2}})\f$ ,
It is obvious that \f$C\f$ only affects the scale. Picture size should be invariant, since \f$C = R + \sqrt{R ^ 2 - 0. 25 * (image_w ^ 2 + image_h ^ 2)}\f$
(Consider the case \f$R ^ 2 < 0. 25 * (image_w ^ 2 + image_h ^ 2)\f$) \n

After projective transform image of \f$(x_0, y_0)\f$ is \f$(x_0 * C \over {R + \sqrt{R ^ 2 - x_0 ^ 2 - y_0 ^ 2}}, y_0 * C \over {R + \sqrt{R ^ 2 - x_0 ^ 2 - y_0 ^ 2}} )\f$ \n

\subsubsection  sphereRad Determination of sphere radius

    Let us known 3 points in the picture, images of which should lie on the same
line after inversion. Then before projective transform these 3 points and inversion
center should lie on the same circle. If we have \f$(x_1, y_1, z_1)\f$ , \f$(x_2, y_2, z_2)\f$ ,
 \f$(x_3, y_3, z_3)\f$, where \f$(x_i, y_i)\f$ is determined  by user, then
\f$x_i ^ 2 + y_i ^2 + z_i ^ 2 = R ^ 2\f$,
\f$A * x_i + B * y_i + C * z_i + D = 0\f$. \n

Moreover \f$A * 0 + B * 0 - R * C + D = 0\f$, since \f$(0, 0, -R)\f$ and
\f$(x_1, y_1, z_1)\f$ \f$(x_2, y_2, z_2)\f$ \f$(x_3, y_3, z_3)\f$ lie in the same plane. \n

Let \f$C = 1\f$ then \f$D = R\f$,
We got system of equations
\f$A * x_i + B * y_i + \sqrt(R^2 -x_i^2 - y_i ^2) + R = 0\f$ \f$i=1,2,3\f$ \n

We solve the system by Newton method.
Let first approximation is :
\f$R = 500\f$ \n
\f$A = {y_1 * (R + \sqrt{R ^ 2 - x_2 ^ 2 - y_2 ^ 2}) - y_2 * (R + \sqrt{R ^ R - x_1 * x_1 - y_1 * y_1})} \over {x_1 * y_2 - x_2 * y_1}\f$ \n
\f$B =  - {x_1 * (R + \sqrt{R * R - x_2 ^ 2 - y_2 ^ 2}) - x_2 * (R + \sqrt{ R * R - x_1 ^ 2 - y_1 ^ 2})} \over {x_1 * y_2 - x_2 * y_1}\f$ \n

  \subsection subSectionBarrel  Barrel Distortion Correction Algorithm.
http://www-ist.massey.ac.nz/dbailey/sprg/ivcnz/Proceedings/IVCNZ_73.pdf
http://mipav.cit.nih.gov/documentation/HTML%20Algorithms/BarrelDistortionCorrection.html

  \subsection subSectionTaylor Taylor series
http://www.ipcbee.com/vol13/34-R012.pdf
  */
