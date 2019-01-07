#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include <thread>
#include <mutex>
#include <functional>

namespace ___bezier_curve___
{
	template<typename T, typename T2>
	inline T& x( T* v, const T2& p ){ return *(v + p * 2); }
	template<typename T, typename T2>
	inline T& y( T* v, const T2& p ){ return *(v + p * 2 + 1); }
	template<typename T, typename T2>

	inline T& X( T* v, const T2& p ){ return *(v + p * 3); }
	template<typename T, typename T2>
	inline T& Y( T* v, const T2& p ){ return *(v + p * 3 + 1); }
	template<typename T, typename T2>
	inline T& Z( T* v, const T2& p ){ return *(v + p * 3 + 2); }

	const unsigned hardware_concurrency = std::thread::hardware_concurrency();
}


template<typename T, size_t N, size_t N2>
inline void getBezierCurveCoeff2d(
		T(&input_xy)[N ],
		T(&output_a)[N2],
		T(&output_b)[N2],
		T(&output_c)[N2] )
{
	using namespace ___bezier_curve___;
	static_assert(!(N % 2),"Error::getBezierCurveCoeff2d()::Size_Of_input_xy_Should_Be_Even");
	static_assert(N > 4,"Error::getBezierCurveCoeff2d()::Size_Of_input_xy_Should_Be_More_Than_4");
	static_assert(!(N2 % 2),"Error::getBezierCurveCoeff2d()::Size_Of_output_Should_Be_Even");
	static_assert(N-4==N2,"Error::getBezierCurveCoeff2d()::Size_Of_output_Should_Equal_To_(N-4)");
	const unsigned L = N/2;// количество заданных точек

	T r[2], p[3*2];
	T bluePoints[(L - 2)*2];
	T brPoint[(L - 3)*2];

	for (unsigned i = 0; i < L - 2; ++i) {
		x(bluePoints,i) = x(input_xy,i+1)*2 - (x(input_xy,i)+x(input_xy,i+2))/2;
		y(bluePoints,i) = y(input_xy,i+1)*2 - (y(input_xy,i)+y(input_xy,i+2))/2;
	}

	for (unsigned i = 0; i < L - 3; ++i) {
		x(brPoint, i) = ( x(bluePoints,i)+x(bluePoints,i+1) )/2;
		y(brPoint, i) = ( y(bluePoints,i)+y(bluePoints,i+1) )/2;
	}

	// Calculate coefficients a, b and c
	if (L>2)
	for(int i = 0; i < N2/2; ++i) {

		if ( i == 0 ) {
			x(p,0) = x(input_xy,0); y(p,0) = y(input_xy,0);
			x(p,1) = x(bluePoints,0); y(p,1) = y(bluePoints,0);
			if( L > 3 ) {
				x(p,2) = x(brPoint,0); y(p,2) = y(brPoint,0);
			}
		}

		if ( i>0 && i < N2/2-1 ) {
			x(p,0) = x(brPoint,i-1); y(p,0) = y(brPoint,i-1);
			x(p,1) = x(bluePoints,i); y(p,1) = y(bluePoints,i);
			if ( L>3 ) {
				x(p,2)=x(brPoint,i); y(p,2)=y(brPoint,i);
			}
		}

		if ( i == N2/2-1 ) {
			if (L > 3) {
				x(p,0) = x(brPoint,N2/2-2);
				y(p,0) = y(brPoint,N2/2-2);
			}
			x(p,1) = x(bluePoints, N2/2-1);
			y(p,1) = y(bluePoints, N2/2-1);
			x(p,2) = x(input_xy, L-1);
			y(p,2) = y(input_xy, L-1);
		}

		x(output_a,i) = x(p,2) - 2*x(p,1) + x(p,0);
		y(output_a,i) = y(p,2) - 2*y(p,1) + y(p,0);
		x(output_b,i) = 2*(x(p,1)-x(p,0));
		y(output_b,i) = 2*(y(p,1)-y(p,0));
		x(output_c,i) = x(p,0);
		y(output_c,i) = y(p,0);
	}
}

template<typename T, typename T2, size_t N>
inline void getPointsOfBezierCurve2d(
		T(&point_xy)[N],
		const T2& step,
		std::function<void(float&&,float&&,float&&,float&&)> draw )
{
	using namespace ___bezier_curve___;
	static_assert(!(N % 2),"Error::getPointsOfBezierCurve2d()::Size_Of_input_xy_Should_Be_Even");
	if (!(step < 1.0)) throw("Error::getRealCubicBezierCurve2d()::Size_Of_Step_Is_Too_Big");
	if (!(step > 0.0)) throw("Error::getRealCubicBezierCurve2d()::Size_Of_Step_Is_Too_Small");
	const unsigned L = N/2;
	T a[(L-2)*2], b[(L-2)*2], c[(L-2)*2];
	getBezierCurveCoeff2d(point_xy, a, b, c);

	for ( unsigned i = 0; i < (L-2); ++i ) {
		for ( T2 t = 0; t < 1.f-step; t += step ) {
			const T2 tt = t*t;
			const T2 t2 = t + step;
			const T2 tt2 = t2*t2;
			draw(	(tt*x(a,i) + t*x(b,i) + x(c,i)),
						(tt*y(a,i) + t*y(b,i) + y(c,i)),
						(tt2*x(a,i) + t2*x(b,i) + x(c,i)),
						(tt2*y(a,i) + t2*y(b,i) + y(c,i)) );
		}
	}
}

//=////////////////////////////////=///////////////////////////////////=//

template<typename T, size_t N, size_t N2>
inline void getBezierCurveCoeff3d(
		T(&input_xy)[N ],
		T(&output_a)[N2],
		T(&output_b)[N2],
		T(&output_c)[N2] )
{
	using namespace ___bezier_curve___;
	static_assert(!(N % 3),"Error::getBezierCurveCoeff3d()::Size_Of_input_xy_Should_Be_Multiple_Of_3");
	static_assert(N > 9,"Error::getBezierCurveCoeff3d()::Size_Of_input_xy_Should_Be_More_Than_9");
	static_assert(!(N2 % 3),"Error::getBezierCurveCoeff3d()::Size_Of_output_Should_Be_Multiple_Of_3");
	static_assert(N-6==N2,"Error::getBezierCurveCoeff3d()::Size_Of_output_Should_Equal_To_(N-6)");
	const unsigned L = N/3;
	T r[3], p[3*3];
	T bluePoints[(L - 2)*3];
	T brPoint[(L - 3)*3];

	for (unsigned i = 0; i < L - 2; ++i) {
		X(bluePoints,i) = X(input_xy,i+1)*2 - (X(input_xy,i)+X(input_xy,i+2))/2;
		Y(bluePoints,i) = Y(input_xy,i+1)*2 - (Y(input_xy,i)+Y(input_xy,i+2))/2;
		Z(bluePoints,i) = Z(input_xy,i+1)*2 - (Z(input_xy,i)+Z(input_xy,i+2))/2;
	}

	for (unsigned i = 0; i < L - 3; ++i) {
		X(brPoint, i) = ( X(bluePoints,i)+X(bluePoints,i+1) )/2;
		Y(brPoint, i) = ( Y(bluePoints,i)+Y(bluePoints,i+1) )/2;
		Z(brPoint, i) = ( Z(bluePoints,i)+Z(bluePoints,i+1) )/2;
	}

	// Calculate coefficients a, b and c
	if (L>2)
	for(int i = 0; i < N2/3; ++i) {

		if ( i == 0 ) {
			X(p,0) = X(input_xy,0); Y(p,0) = Y(input_xy,0); Z(p,0) = Z(input_xy,0);
			X(p,1) = X(bluePoints,0); Y(p,1) = Y(bluePoints,0); Z(p,1) = Z(bluePoints,0);
			if( L > 3 ) {
				X(p,2) = X(brPoint,0); Y(p,2) = Y(brPoint,0); Z(p,2) = Z(brPoint,0);
			}
		}

		if ( i>0 && i < N2/3-1 ) {
			X(p,0) = X(brPoint,i-1);
			Y(p,0) = Y(brPoint,i-1);
			Z(p,0) = Z(brPoint,i-1);

			X(p,1) = X(bluePoints,i);
			Y(p,1) = Y(bluePoints,i);
			Z(p,1) = Z(bluePoints,i);
			if ( L>3 ) {
				X(p,2)=X(brPoint,i);
				Y(p,2)=Y(brPoint,i);
				Z(p,2)=Z(brPoint,i);
			}
		}

		if ( i == N2/3-1 ) {
			if (L > 3) {
				X(p,0) = X(brPoint,N2/3-2);
				Y(p,0) = Y(brPoint,N2/3-2);
				Z(p,0) = Z(brPoint,N2/3-2);
			}
			X(p,1) = X(bluePoints, N2/3-1);
			Y(p,1) = Y(bluePoints, N2/3-1);
			Z(p,1) = Z(bluePoints, N2/3-1);
			X(p,2) = X(input_xy, L-1);
			Y(p,2) = Y(input_xy, L-1);
			Z(p,2) = Z(input_xy, L-1);
		}

		X(output_a,i) = X(p,2) - 2*X(p,1) + X(p,0);
		Y(output_a,i) = Y(p,2) - 2*Y(p,1) + Y(p,0);
		Z(output_a,i) = Z(p,2) - 2*Z(p,1) + Z(p,0);
		X(output_b,i) = 2*(X(p,1)-X(p,0));
		Y(output_b,i) = 2*(Y(p,1)-Y(p,0));
		Z(output_b,i) = 2*(Z(p,1)-Z(p,0));
		X(output_c,i) = X(p,0);
		Y(output_c,i) = Y(p,0);
		Z(output_c,i) = Z(p,0);
	}
}

template<typename T, typename T2, size_t N>
inline void getPointsOfBezierCurve3d(
		T(&point_xy)[N],
		const T2& step,
		std::function<void(float&&,float&&,float&&,float&&,float&&,float&&)> draw )
{
	using namespace ___bezier_curve___;
	static_assert(!(N % 3),"Error::getBezierCurveCoeff3d()::Size_Of_input_xy_Should_Be_Multiple_Of_3");
	if (!(step < 1.0))throw("Error::getPointsOfBezierCurve3d()::Size Of Step Is Too Big");
	if(!(step > 0.0)) throw("Error::getPointsOfBezierCurve3d()::Size Of Step Is Too Small");
	const unsigned L = N/3;
	T a[(L-2)*3], b[(L-2)*3], c[(L-2)*3];
	getBezierCurveCoeff3d(point_xy, a, b, c);
	for ( unsigned i = 0; i < (L-2); ++i ) {
		for ( T2 t = 0; t < 1.0-step; t += step ) {
			const T2 tt = t*t;
			const T2 t2 = t + step;
			const T2 tt2 = t2*t2;
			draw(	(tt*X(a,i) + t*X(b,i) + X(c,i)),
						(tt*Y(a,i) + t*Y(b,i) + Y(c,i)),
						(tt*Z(a,i) + t*Z(b,i) + Z(c,i)),
						(tt2*X(a,i) + t2*X(b,i) + X(c,i)),
						(tt2*Y(a,i) + t2*Y(b,i) + Y(c,i)),
						(tt2*Z(a,i) + t2*Z(b,i) + Z(c,i)) );
		}
	}
}

//=//////////////////////////=/////////////////////////////////=//
/*
	Example of usage:

			float p[] = {
				0.f,0.f,
				10.f,20.f,
				20.f,-20.f,
				30.f,40.f
			};
			getRealCubicBezierCurve2d( p, 0.01f, [&mesh]( float *arr ) {
				Vector3dd v1(arr[0],arr[1],0), v2(arr[2],arr[3],0);
				mesh->addLine(v1, v2);
			} );
*/

template< typename T, typename T2, size_t N >
inline void getRealCubicBezierCurve2d(
		T(&point_xy)[N],
		const T2& step,
		std::function<void(float(&array)[4])> draw )
{
	using namespace ___bezier_curve___;
	static_assert(N == 4 * 2, "Error::getRealCubicBezierCurve2d()::Size_Of_point_xy_Should_Be_8");
	if (!(step < 1.0))throw("Error::getRealCubicBezierCurve2d()::Size_Of_Step_Is_Too_Big");
	if(!(step > 0.0)) throw("Error::getRealCubicBezierCurve2d()::Size_Of_Step_Is_Too_Small");

	const unsigned L = (unsigned)(1.0/step) - 1;

	unsigned start = 0;
	if ( L >= hardware_concurrency ) {
		const unsigned chunk_size = L / hardware_concurrency;
		start = chunk_size * (hardware_concurrency - 1);
		std::thread th[hardware_concurrency - 1];
		std::mutex mu;
		for ( unsigned i = 0; i < hardware_concurrency - 1; ++i ) {
			th[i] = std::thread([&point_xy, draw, &step, &chunk_size, i, &mu](){

				for ( unsigned j = chunk_size * i; j < chunk_size * (i + 1); ++j ) {

					T2 t = (T2)j * step, nt = 1.0 - t;
					T2 t_ = t + step, nt_ = 1.0 - t_;
					T2 nt2 = nt*nt, nt3 = nt2*nt;
					T2 nt2_ = nt_*nt_, nt3_ = nt2_*nt_;
					T2 t2 = t*t, t3 = t2*t;
					T2 t2_ = t_*t_, t3_ = t2_*t_;
					float array[4] = {
						nt3 * x(point_xy,0)
										+ 3 * (nt2 * t * x(point_xy,1) + nt * t2 * x(point_xy,2))
										+ t3 * x(point_xy,3),
						nt3 * y(point_xy,0)
										+ 3 * (nt2 * t * y(point_xy,1) + nt * t2 * y(point_xy,2))
										+ t3 * y(point_xy,3),

						nt3_ * x(point_xy,0)
										+ 3 * (nt2_ * t_ * x(point_xy,1) + nt_ * t2_ * x(point_xy,2))
										+ t3_ * x(point_xy,3),
						nt3_ * y(point_xy,0)
										+ 3 * (nt2_ * t_ * y(point_xy,1) + nt_ * t2_ * y(point_xy,2))
										+ t3_ * y(point_xy,3)
					};

					mu.lock();
					draw( array );
					mu.unlock();
				}
			});
		}
		for ( unsigned i = start; i < L; ++i ) {

			T2 t = (T2)i * step, nt = 1.0 - t;
			T2 t_ = t + step, nt_ = 1.0 - t_;
			T2 nt2 = nt*nt, nt3 = nt2*nt;
			T2 nt2_ = nt_*nt_, nt3_ = nt2_*nt_;
			T2 t2 = t*t, t3 = t2*t;
			T2 t2_ = t_*t_, t3_ = t2_*t_;

			float array[4] = {
				nt3 * x(point_xy,0)
								+ 3 * (nt2 * t * x(point_xy,1) + nt * t2 * x(point_xy,2))
								+ t3 * x(point_xy,3),
				nt3 * y(point_xy,0)
								+ 3 * (nt2 * t * y(point_xy,1) + nt * t2 * y(point_xy,2))
								+ t3 * y(point_xy,3),

				nt3_ * x(point_xy,0)
								+ 3 * (nt2_ * t_ * x(point_xy,1) + nt_ * t2_ * x(point_xy,2))
								+ t3_ * x(point_xy,3),
				nt3_ * y(point_xy,0)
								+ 3 * (nt2_ * t_ * y(point_xy,1) + nt_ * t2_ * y(point_xy,2))
								+ t3_ * y(point_xy,3)
			};

			mu.lock();
			draw( array );
			mu.unlock();
		}

		for ( unsigned i = 0; i < hardware_concurrency - 1; ++i ) { th[i].join(); }
	} else {

		for ( unsigned i = start; i < L; ++i ) {

			T2 t = (T2)i * step, nt = 1.0 - t;
			T2 t_ = t + step, nt_ = 1.0 - t_;
			T2 nt2 = nt*nt, nt3 = nt2*nt;
			T2 nt2_ = nt_*nt_, nt3_ = nt2_*nt_;
			T2 t2 = t*t, t3 = t2*t;
			T2 t2_ = t_*t_, t3_ = t2_*t_;

			float array[4] = {
				nt3 * x(point_xy,0)
								+ 3 * (nt2 * t * x(point_xy,1) + nt * t2 * x(point_xy,2))
								+ t3 * x(point_xy,3),
				nt3 * y(point_xy,0)
								+ 3 * (nt2 * t * y(point_xy,1) + nt * t2 * y(point_xy,2))
								+ t3 * y(point_xy,3),

				nt3_ * x(point_xy,0)
								+ 3 * (nt2_ * t_ * x(point_xy,1) + nt_ * t2_ * x(point_xy,2))
								+ t3_ * x(point_xy,3),
				nt3_ * y(point_xy,0)
								+ 3 * (nt2_ * t_ * y(point_xy,1) + nt_ * t2_ * y(point_xy,2))
								+ t3_ * y(point_xy,3)
			};

			draw( array );
		}
	}
}

//=///////////////////////////=//////////////////////////////=//
/*
	Example of usage:


			float p[] = {
				0.f,0.f,0.f,
				10.f,20.f,-20.f,
				20.f,-20.f,30.f,
				30.f,40.f,-50.f
			};
			getRealCubicBezierCurve3d( p, 0.01f, [&mesh]( float *arr ) {
				Vector3dd v1(arr[0],arr[1],arr[2]), v2(arr[3],arr[4],arr[5]);
				mesh->addLine(v1, v2);
			} );

*/

template< typename T, typename T2, size_t N >
inline void getRealCubicBezierCurve3d(
		T(&point_xy)[N],
		const T2& step,
		std::function<void(float(&array)[6])> draw )
{
	using namespace ___bezier_curve___;
	static_assert(N == 4 * 3, "Error::getRealCubicBezierCurve3d()::Size_Of_point_xy_Should_Be_12");
	if (!(step < 1.0))throw("Error::getRealCubicBezierCurve3d()::Size_Of_Step_Is_Too_Big");
	if(!(step > 0.0)) throw("Error::getRealCubicBezierCurve3d()::Size_Of_Step_Is_Too_Small");

	const unsigned L = (unsigned)(1.0/step) - 1;

	unsigned start = 0;
	if ( L >= hardware_concurrency ) {
		const unsigned chunk_size = L / hardware_concurrency;
		start = chunk_size * (hardware_concurrency - 1);
		std::thread th[hardware_concurrency - 1];
		std::mutex mu;
		for ( unsigned i = 0; i < hardware_concurrency - 1; ++i ) {
			th[i] = std::thread([&point_xy, draw, &step, &chunk_size, i, &mu](){

				for ( unsigned j = chunk_size * i; j < chunk_size * (i + 1); ++j ) {

					T2 t = (T2)j * step, nt = 1.0 - t;
					T2 t_ = t + step, nt_ = 1.0 - t_;
					T2 nt2 = nt*nt, nt3 = nt2*nt;
					T2 nt2_ = nt_*nt_, nt3_ = nt2_*nt_;
					T2 t2 = t*t, t3 = t2*t;
					T2 t2_ = t_*t_, t3_ = t2_*t_;
					float array[6] = {
						nt3 * X(point_xy,0)
										+ 3 * (nt2 * t * X(point_xy,1) + nt * t2 * X(point_xy,2))
										+ t3 * X(point_xy,3),
						nt3 * Y(point_xy,0)
										+ 3 * (nt2 * t * Y(point_xy,1) + nt * t2 * Y(point_xy,2))
										+ t3 * Y(point_xy,3),
						nt3 * Z(point_xy,0)
										+ 3 * (nt2 * t * Z(point_xy,1) + nt * t2 * Z(point_xy,2))
										+ t3 * Z(point_xy,3),

						nt3_ * X(point_xy,0)
										+ 3 * (nt2_ * t_ * X(point_xy,1) + nt_ * t2_ * X(point_xy,2))
										+ t3_ * X(point_xy,3),
						nt3_ * Y(point_xy,0)
										+ 3 * (nt2_ * t_ * Y(point_xy,1) + nt_ * t2_ * Y(point_xy,2))
										+ t3_ * Y(point_xy,3),
						nt3_ * Z(point_xy,0)
										+ 3 * (nt2_ * t_ * Z(point_xy,1) + nt_ * t2_ * Z(point_xy,2))
										+ t3_ * Z(point_xy,3)
					};

					mu.lock();
					draw( array );
					mu.unlock();
				}
			});
		}
		for ( unsigned i = start; i < L; ++i ) {

			T2 t = (T2)i * step, nt = 1.0 - t;
			T2 t_ = t + step, nt_ = 1.0 - t_;
			T2 nt2 = nt*nt, nt3 = nt2*nt;
			T2 nt2_ = nt_*nt_, nt3_ = nt2_*nt_;
			T2 t2 = t*t, t3 = t2*t;
			T2 t2_ = t_*t_, t3_ = t2_*t_;

			float array[6] = {
				nt3 * X(point_xy,0)
								+ 3 * (nt2 * t * X(point_xy,1) + nt * t2 * X(point_xy,2))
								+ t3 * X(point_xy,3),
				nt3 * Y(point_xy,0)
								+ 3 * (nt2 * t * Y(point_xy,1) + nt * t2 * Y(point_xy,2))
								+ t3 * Y(point_xy,3),
				nt3 * Z(point_xy,0)
								+ 3 * (nt2 * t * Z(point_xy,1) + nt * t2 * Z(point_xy,2))
								+ t3 * Z(point_xy,3),

				nt3_ * X(point_xy,0)
								+ 3 * (nt2_ * t_ * X(point_xy,1) + nt_ * t2_ * X(point_xy,2))
								+ t3_ * X(point_xy,3),
				nt3_ * Y(point_xy,0)
								+ 3 * (nt2_ * t_ * Y(point_xy,1) + nt_ * t2_ * Y(point_xy,2))
								+ t3_ * Y(point_xy,3),
				nt3_ * Z(point_xy,0)
								+ 3 * (nt2_ * t_ * Z(point_xy,1) + nt_ * t2_ * Z(point_xy,2))
								+ t3_ * Z(point_xy,3)
			};

			mu.lock();
			draw( array );
			mu.unlock();
		}

		for ( unsigned i = 0; i < hardware_concurrency - 1; ++i ) { th[i].join(); }
	} else {

		for ( unsigned i = start; i < L; ++i ) {

			T2 t = (T2)i * step, nt = 1.0 - t;
			T2 t_ = t + step, nt_ = 1.0 - t_;
			T2 nt2 = nt*nt, nt3 = nt2*nt;
			T2 nt2_ = nt_*nt_, nt3_ = nt2_*nt_;
			T2 t2 = t*t, t3 = t2*t;
			T2 t2_ = t_*t_, t3_ = t2_*t_;

			float array[6] = {
				nt3 * X(point_xy,0)
								+ 3 * (nt2 * t * X(point_xy,1) + nt * t2 * X(point_xy,2))
								+ t3 * X(point_xy,3),
				nt3 * Y(point_xy,0)
								+ 3 * (nt2 * t * Y(point_xy,1) + nt * t2 * Y(point_xy,2))
								+ t3 * Y(point_xy,3),
				nt3 * Z(point_xy,0)
								+ 3 * (nt2 * t * Z(point_xy,1) + nt * t2 * Z(point_xy,2))
								+ t3 * Z(point_xy,3),

				nt3_ * X(point_xy,0)
								+ 3 * (nt2_ * t_ * X(point_xy,1) + nt_ * t2_ * X(point_xy,2))
								+ t3_ * X(point_xy,3),
				nt3_ * Y(point_xy,0)
								+ 3 * (nt2_ * t_ * Y(point_xy,1) + nt_ * t2_ * Y(point_xy,2))
								+ t3_ * Y(point_xy,3),
				nt3_ * Z(point_xy,0)
								+ 3 * (nt2_ * t_ * Z(point_xy,1) + nt_ * t2_ * Z(point_xy,2))
								+ t3_ * Z(point_xy,3)
			};

			draw( array );
		}
	}
}

#endif // BEZIERCURVE_H

