#include "perlinNoise.h"

namespace corecvs {

void PerlinNoise::mapToArea(const Vector3dd &input, Vector3d32 &grid, Vector3dd &remainder)
{
    Vector3dd p = input ;
    p = p.mapF([](double in) {
        double a = (int)(in / SIZE) * SIZE;
        in -= a;
        if (in < 0) in += SIZE;
        return in;
    });

    grid = Vector3d32((int)p.x(), (int)p.y(), (int)p.z());

    remainder = hermit3(Vector3dd(p.x() - grid.x(), p.y() - grid.y(),p.z() - grid.z()));
}

double PerlinNoise::query(Vector3d32 &g, Vector3dd &r)
{
    Vector3dd mr = Vector3dd(1.0, 1.0, 1.0) - r;

    g = g.mapF([](int32_t in) {
        return in % SIZE;
    });


    Vector3d32 g1 = g.mapF([](int32_t in) {
        return (in + 1) % SIZE;
    });

    return
    mr.x() * (mr.y() * mr.z() * n->element(g.x() ,  g.y() ,  g.z()) +
              mr.y() *  r.z() * n->element(g.x() ,  g.y() , g1.z()) +
               r.y() * mr.z() * n->element(g.x() , g1.y() ,  g.z()) +
               r.y() *  r.z() * n->element(g.x() , g1.y() , g1.z())) +

     r.x() * (mr.y() * mr.z() * n->element(g1.x(),  g.y() ,  g.z()) +
              mr.y() *  r.z() * n->element(g1.x(),  g.y() , g1.z()) +
               r.y() * mr.z() * n->element(g1.x(), g1.y() ,  g.z()) +
               r.y() *  r.z() * n->element(g1.x(), g1.y() , g1.z()));
}


Vector3dd PerlinNoise::noise3d(const Vector3dd &v)
{
    Vector3d32 grid;
    Vector3dd  remainder;

    mapToArea(v, grid, remainder);

    Vector3dd  result;

//    cout << " V:" << v         << std::endl;
//    cout << " G:" << grid      << std::endl;
//    cout << " R:" << remainder << std::endl;

    result.x() = query(grid, remainder);
    grid += Vector3d32(1);
    result.y() = query(grid, remainder);
    grid += Vector3d32(1);
    result.z() = query(grid, remainder);
    return result;
}

double PerlinNoise::noise(const Vector3dd &v)
{
    Vector3d32 grid;
    Vector3dd  remainder;

    mapToArea(v, grid, remainder);

//    cout << " V:" << v         << std::endl;
//    cout << " G:" << grid      << std::endl;
//    cout << " R:" << remainder << std::endl;

    return query(grid, remainder);
}

double PerlinNoise::turbulence(const Vector3dd &v, int force)
{
  double koeff = 1.0;
  int i;
  double result = 0.0;
  Vector3dd pf = v;

  for (i = 0; i < force; i++, koeff /= 2)
   {
      result += noise(pf) * koeff;
      pf *= 2.0;
   }
  return result;
}



Vector3dd PerlinNoise::turbulence3d(const Vector3dd &v, int force)
{
  double koeff = 1;
  int i;
  Vector3dd result(0.0);
  Vector3dd pf = v;

  for (i = 0; i < force; i++, koeff /= 2)
  {
     result += noise3d(pf) * koeff;
     pf *= 2.0;
  }
  return result;
}


} //  namespace corecvs
