#ifndef __VEC4_H__
#define __VEC4_H__

#include <xmmintrin.h>

class vec4
{	
	private:
			// Swizzle helper
		template <unsigned mask>
		struct _swzl
		{
			friend class vec4;

			private:
				template <unsigned target, unsigned m>
				struct _mask_merger
				{
					enum
					{
						ROW0 = ((target >> (((m >> 0) & 3) << 1)) & 3) << 0,
						ROW1 = ((target >> (((m >> 2) & 3) << 1)) & 3) << 2,
						ROW2 = ((target >> (((m >> 4) & 3) << 1)) & 3) << 4,
						ROW3 = ((target >> (((m >> 6) & 3) << 1)) & 3) << 6,

						MASK = ROW0 | ROW1 | ROW2 | ROW3,
					};

					private:
						_mask_merger();
				};

				template <unsigned m>
				struct _mask_reverser
				{
					enum
					{
						ROW0 = 0 << (((m >> 0) & 3) * 2),
						ROW1 = 1 << (((m >> 2) & 3) * 2),
						ROW2 = 2 << (((m >> 4) & 3) * 2),
						ROW3 = 3 << (((m >> 6) & 3) * 2),

						MASK = ROW0 | ROW1 | ROW2 | ROW3,
					};

					private:
						_mask_reverser();
				};

			public:
				inline operator const vec4 () const {
					return _mm_shuffle_ps(v.m, v.m, mask);
				}

					// Swizzle from vec4
				inline _swzl& operator = (const vec4 &r) {
					v.m = _mm_shuffle_ps(r.m, r.m, _mask_reverser<mask>::MASK);
					return *this;
				}
				 
					// Swizzle from same mask (v1.xyzw = v2.xyzw)
				inline _swzl& operator = (const _swzl &s) {
					v.m = s.v.m;
					return *this;
				}

					// Swizzle mask => other_mask (v1.zwxy = v2.xyxy)
				template<unsigned other_mask>
				inline _swzl& operator = (const _swzl<other_mask> &s) {
					typedef _mask_merger<other_mask, _mask_reverser<mask>::MASK> merged;
					v.m = _mm_shuffle_ps(s.v.m, s.v.m, merged::MASK);
					return *this;
				}

					// Swizzle of the swizzle, read only (v.xxxx.yyyy)
				template<unsigned other_mask>
				inline const vec4 shuffle4_ro() const {
					typedef _mask_merger<mask, other_mask> merged;
					return _mm_shuffle_ps(v.m, v.m, merged::MASK);
				}

					// Swizzle of the swizzle, read/write (v1.zyxw.wzyx = ...)
				template<unsigned other_mask>
				inline _swzl<_mask_merger<mask, other_mask>::MASK> shuffle4_rw() {
					return _swzl<_mask_merger<mask, other_mask>::MASK>(v);
				}

				float &x, &y, &z, &w;
				float &r, &g, &b, &a;
				float &s, &t, &p, &q;

			private:
					// This massive contructor maps a vector to references
				inline _swzl(vec4 &v):
					x(v[(mask >> 0) & 0x3]), y(v[(mask >> 2) & 0x3]),
					z(v[(mask >> 4) & 0x3]), w(v[(mask >> 6) & 0x3]),

					r(v[(mask >> 0) & 0x3]), g(v[(mask >> 2) & 0x3]),
					b(v[(mask >> 4) & 0x3]), a(v[(mask >> 6) & 0x3]),

					s(v[(mask >> 0) & 0x3]), t(v[(mask >> 2) & 0x3]),
					p(v[(mask >> 4) & 0x3]), q(v[(mask >> 6) & 0x3]),

					v(v) {
						// Empty
				}

					// Refrence to unswizzled self
				vec4 &v;
		};

		// ----------------------------------------------------------------- //

	public:
			// Empty constructor
		inline vec4() {
			m = _mm_setzero_ps();
		}

			// Fill constructor
		explicit inline vec4(float f) {
			m = _mm_set1_ps(f);
		}

			// 4 var init constructor
		inline vec4(float _x, float _y, float _z, float _w) {
			m = _mm_setr_ps(_x, _y, _z, _w);
		}

			// Float array constructor
		inline vec4(const float* fv) {
			m = _mm_loadu_ps(fv);
		}
			
			// Copy constructor
		inline vec4(const vec4 &v) {
			m = v.m;
		}

			// SSE compatible constructor
		inline vec4(const __m128 &_m) {
			m = _m;
		}		
		
		// ----------------------------------------------------------------- //

			// Read-write swizzle
		template<unsigned mask>
		inline _swzl<mask> shuffle4_rw() {
			return _swzl<mask>(*this);
		}

			// Read-write (actually read only) swizzle, const
		template<unsigned mask>
		inline const vec4 shuffle4_rw() const {
			return _mm_shuffle_ps(m, m, mask);
		}
		
			// Read-only swizzle
		template<unsigned mask>
		inline const vec4 shuffle4_ro() const {
			return _mm_shuffle_ps(m, m, mask);
		}

		// ----------------------------------------------------------------- //

			// Write direct access operator
		inline float& operator[](int index) {
			return ((float*)this)[index];
		}
		
			// Read direct access operator
		inline const float& operator[](int index) const {
			return ((const float*)this)[index];
		}

			// Cast operator
		inline operator float* () {
			return (float*)this;
		}
		
			// Const cast operator
		inline operator const float* () const {
			return (const float*)this;
		}

		// ----------------------------------------------------------------- //

		friend inline vec4& operator += (vec4 &v, float f) {
			v.m = _mm_add_ps(v.m, _mm_set1_ps(f));
			return v;
		}
		
		friend inline vec4& operator += (vec4 &v0, const vec4 &v1) {
			v0.m = _mm_add_ps(v0.m, v1.m);
			return v0;
		}

		friend inline vec4& operator -= (vec4 &v, float f) {
			v.m = _mm_sub_ps(v.m, _mm_set1_ps(f));
			return v;
		}
		
		friend inline vec4& operator -= (vec4 &v0, const vec4 &v1) {
			v0.m = _mm_sub_ps(v0.m, v1.m);
			return v0;
		}

		friend inline vec4& operator *= (vec4 &v, float f) {	
			v.m = _mm_mul_ps(v.m, _mm_set1_ps(f));
			return v;
		}
					
		friend inline vec4& operator *= (vec4 &v0, const vec4 &v1) {
			v0.m = _mm_mul_ps(v0.m, v1.m);
			return v0;
		}

		friend inline vec4& operator /= (vec4 &v, float f) {
			v.m = _mm_div_ps(v.m, _mm_set1_ps(f));
			return v;
		}
		
		friend inline vec4& operator /= (vec4 &v0, const vec4 &v1) {
			v0.m = _mm_div_ps(v0.m, v1.m);
			return v0;
		}

		// ----------------------------------------------------------------- //

		friend inline const vec4 operator + (const vec4 &v, float f) {
			return _mm_add_ps(v.m, _mm_set1_ps(f));
		}
		
		friend inline const vec4 operator + (const vec4 &v0, const vec4 &v1) {
			return _mm_add_ps(v0.m, v1.m);
		}

		inline const vec4 operator - () {
			return _mm_xor_ps(m, _mm_set1_ps(-0.f));
		}

		friend inline const vec4 operator - (const vec4 &v) {
			return _mm_sub_ps(_mm_setzero_ps(), v.m);
		}

		friend inline const vec4 operator - (const vec4 &v, float f) {
			return _mm_sub_ps(v.m, _mm_set1_ps(f));
		}

		friend inline const vec4 operator - (const vec4 &v0, const vec4 &v1) {
			return _mm_sub_ps(v0.m, v1.m);
		}

		friend inline const vec4 operator * (const vec4 &v, float f) {
			return _mm_mul_ps(v.m, _mm_set1_ps(f));
		}

		friend inline const vec4 operator * (const vec4 &v0, const vec4 &v1) {
			return _mm_mul_ps(v0.m, v1.m);
		}

		friend inline const vec4 operator / (float f, const vec4 &v) {
			return _mm_div_ps(_mm_set1_ps(f), v.m);
		}

		friend inline const vec4 operator / (const vec4 &v, float f) {
			return _mm_div_ps(v.m, _mm_set1_ps(f));
		}

		friend inline const vec4 operator / (const vec4 &v0, const vec4 &v1) {
			return _mm_div_ps(v0.m, v1.m);
		}

		// ----------------------------------------------------------------- //
		
		friend inline const vec4 sqrt(const vec4 &v) {
			return _mm_sqrt_ps(v.m);
		}

		friend inline const vec4 inversesqrt(const vec4 &v) {
			return _mm_div_ps(_mm_set1_ps(1.0f), _mm_sqrt_ps(v.m));
		}

		// ----------------------------------------------------------------- //

		friend inline const vec4 abs(const vec4 &v) {
			return _mm_andnot_ps(_mm_set1_ps(-0.f), v.m);
		}

		friend inline const vec4 ceil(const vec4 &v) {
			__m128 t = _mm_or_ps(_mm_and_ps(_mm_set1_ps(-0.f), v.m),
			                     _mm_set1_ps(1 << 23));
			t = _mm_sub_ps(_mm_add_ps(v.m, t), t);
			return _mm_add_ps(t, _mm_and_ps(_mm_cmpgt_ps(v.m, t),
			                                _mm_set1_ps(1.0f)));
		}

		friend inline const vec4 clamp(const vec4 &v0, float f1, float f2) {
			return _mm_max_ps(_mm_set1_ps(f1),
			                  _mm_min_ps(_mm_set1_ps(f2), v0.m));
		}

		friend inline const vec4 clamp(const vec4 &v0,
		                               const vec4 &v1, const vec4 &v2) {
			return _mm_max_ps(v1.m, _mm_min_ps(v2.m, v0.m));
		}

		friend inline const vec4 floor(const vec4 &v) {
			__m128 t = _mm_or_ps(_mm_and_ps(_mm_set1_ps(-0.f), v.m),
			                     _mm_set1_ps(1 << 23));
			t = _mm_sub_ps(_mm_add_ps(v.m, t), t);
			return _mm_sub_ps(t, _mm_and_ps(_mm_cmplt_ps(v.m, t),
			                                _mm_set1_ps(1.0f)));
		}

		friend inline const vec4 fract(const vec4 &v) {
			__m128 t = _mm_or_ps(_mm_and_ps(_mm_set1_ps(-0.f), v.m),
			                     _mm_set1_ps(1 << 23));
			return _mm_sub_ps(v.m, _mm_sub_ps(_mm_add_ps(v.m, t), t));
		}

		friend inline const vec4 max(const vec4 &v, float f) {
			return _mm_max_ps(v.m, _mm_set1_ps(f));
		}
		
		friend inline const vec4 max(const vec4 &v0, const vec4 &v1) {
			return _mm_max_ps(v0.m, v1.m);
		}

		friend inline const vec4 min(const vec4 &v, float f) {
			return _mm_min_ps(v.m, _mm_set1_ps(f));
		}
		
		friend inline const vec4 min(const vec4 &v0, const vec4 &v1) {
			return _mm_min_ps(v0.m, v1.m);
		}

		friend inline const vec4 mix(const vec4 &v0, const vec4 &v1,
									 float f) {
			return _mm_sub_ps(v1.m, _mm_mul_ps(_mm_set1_ps(f),
			                                   _mm_add_ps(v0.m, v1.m)));
		}

		friend inline const vec4 mix(const vec4 &v0, const vec4 &v1,
		                             const vec4 &v2) {
			return _mm_sub_ps(v1.m, _mm_mul_ps(v2.m, _mm_add_ps(v0.m, v1.m)));
		}

		friend inline const vec4 mod(const vec4 &v, float f) {
			__m128 r = _mm_div_ps(v.m, _mm_set1_ps(f));
			__m128 t = _mm_or_ps(_mm_and_ps(_mm_set1_ps(-0.f), r),
			                     _mm_set1_ps(1 << 23));
			return _mm_sub_ps(v.m, _mm_mul_ps(_mm_sub_ps(_mm_add_ps(r, t), t),
			                                   _mm_set1_ps(f)));
		}

		friend inline const vec4 mod(const vec4 &v0, const vec4 &v1) {
			__m128 r = _mm_div_ps(v0.m, v1.m);
			__m128 t = _mm_or_ps(_mm_and_ps(_mm_set1_ps(-0.f), r),
			                     _mm_set1_ps(1 << 23));
			return _mm_sub_ps(v0.m, _mm_mul_ps(_mm_sub_ps(_mm_add_ps(r, t), t),
			                                   v1.m));
		}

		friend inline const vec4 sign(const vec4 &v) {
			return _mm_and_ps(_mm_or_ps(_mm_and_ps(v.m, _mm_set1_ps(-0.f)),
			                            _mm_set1_ps(1.0f)),
			                  _mm_cmpneq_ps(v.m, _mm_setzero_ps()));
		}

		friend inline const vec4 smoothstep(float f1, float f2,
		                                    const vec4 &v) {
			__m128 v0 = _mm_set1_ps(f1);
			__m128 c = _mm_max_ps(_mm_setzero_ps(),
			                      _mm_min_ps(_mm_set1_ps(1.0f),
			                                 _mm_sub_ps(_mm_sub_ps(v.m, v0),
											 _mm_sub_ps(_mm_set1_ps(f2), v0))));
			return _mm_mul_ps(_mm_mul_ps(c, c),
			                  _mm_sub_ps(_mm_set1_ps(3.0f),
			                             _mm_mul_ps(_mm_set1_ps(2.0f), c)));
		}
		
		friend inline const vec4 smoothstep(const vec4 &v0,
		                                    const vec4 &v1, const vec4 &v2) {
			__m128 c = _mm_max_ps(_mm_setzero_ps(),
			                      _mm_min_ps(_mm_set1_ps(1.0f),
			                                 _mm_sub_ps(_mm_sub_ps(v2.m, v0.m),
			                                            _mm_sub_ps(v1.m, v0.m))));
			return _mm_mul_ps(_mm_mul_ps(c, c),
			                  _mm_sub_ps(_mm_set1_ps(3.0f),
			                             _mm_mul_ps(_mm_set1_ps(2.0f), c)));
		}

		friend inline const vec4 step(float f, const vec4 &v) {
			return _mm_and_ps(_mm_cmple_ps(v.m, _mm_set1_ps(f)),
			                  _mm_set1_ps(1.0f));
		}

		friend inline const vec4 step(const vec4 &v0, const vec4 &v1) {
			return _mm_and_ps(_mm_cmple_ps(v0.m, v1.m), _mm_set1_ps(1.0f));
		}

		friend inline const vec4 trunc(const vec4 &v) {
			__m128 t = _mm_or_ps(_mm_and_ps(_mm_set1_ps(-0.f), v.m),
			                     _mm_set1_ps(1 << 23));
			return _mm_sub_ps(_mm_add_ps(v.m, t), t);
		}

		// ----------------------------------------------------------------- //

		friend inline float distance(const vec4 &v0, const vec4 &v1) {
			__m128 l = _mm_sub_ps(v0.m, v1.m);
			l = _mm_mul_ps(l, l);
			l = _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x4E));
			return _mm_cvtss_f32(_mm_sqrt_ss(_mm_add_ss(l,
			                                 _mm_shuffle_ps(l, l, 0x11))));
		}

		friend inline float dot(const vec4 &v0, const vec4 &v1) {
			__m128 l = _mm_mul_ps(v0.m, v1.m);
			l = _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x4E));
			return _mm_cvtss_f32(_mm_add_ss(l, _mm_shuffle_ps(l, l, 0x11)));

		}

		friend inline const vec4 faceforward(const vec4 &v0,
		                                     const vec4 &v1, const vec4 &v2) {
			__m128 l = _mm_mul_ps(v2.m, v1.m);
			l = _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x4E));
			return _mm_xor_ps(_mm_and_ps(_mm_cmpnlt_ps(
			        _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x11)),
			        _mm_setzero_ps()), _mm_set1_ps(-0.f)), v0.m);
		}

		friend inline float length(const vec4 &v) {
			__m128 l = _mm_mul_ps(v.m, v.m);
			l = _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x4E));
			return _mm_cvtss_f32(_mm_sqrt_ss(_mm_add_ss(l,
			                                 _mm_shuffle_ps(l, l, 0x11))));
		}

		friend inline const vec4 normalize(const vec4 &v) {
			__m128 l = _mm_mul_ps(v.m, v.m);
			l = _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x4E));
			return _mm_div_ps(v.m, _mm_sqrt_ps(_mm_add_ps(l,
			                                   _mm_shuffle_ps(l, l, 0x11))));
		}

		friend inline const vec4 reflect(const vec4 &v0, const vec4 &v1) {
			__m128 l = _mm_mul_ps(v1.m, v0.m);
			l = _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x4E));
			l = _mm_add_ps(l, _mm_shuffle_ps(l, l, 0x11));
			return _mm_sub_ps(v0.m, _mm_mul_ps(_mm_add_ps(l, l), v1.m));
		}

		friend inline const vec4 refract(const vec4 &v0, const vec4 &v1,
										 float f) {
			__m128 d = _mm_mul_ps(v1.m, v0.m);
			d = _mm_add_ps(d, _mm_shuffle_ps(d, d, 0x4E));
			d = _mm_add_ps(d, _mm_shuffle_ps(d, d, 0x11));
			__m128 e = _mm_set1_ps(f);
			__m128 k = _mm_sub_ps(_mm_set1_ps(1.0f),
			                      _mm_mul_ps(_mm_mul_ps(e, e),
			                                 _mm_sub_ps(_mm_set1_ps(1.0f),
			                                            _mm_mul_ps(d, d))));
			return _mm_and_ps(_mm_cmpnlt_ps(k, _mm_setzero_ps()),
			                  _mm_mul_ps(_mm_mul_ps(e, _mm_sub_ps(v0.m,
			                  _mm_mul_ps(_mm_mul_ps(e, d), _mm_sqrt_ps(k)))),
			                             v1.m));
		}

		// ----------------------------------------------------------------- //

		friend inline bool operator == (const vec4 &v0, const vec4 &v1) {
			return (_mm_movemask_ps(_mm_cmpeq_ps(v0.m, v1.m)) == 0xF);
		}

		friend inline bool operator != (const vec4 &v0, const vec4 &v1) {
			return (_mm_movemask_ps(_mm_cmpneq_ps(v0.m, v1.m)) == 0xF);
		}
		
		// ----------------------------------------------------------------- //
		
		union {
				// Vertex / Vector 
			struct {
				float x, y, z, w;
			};
				// Color
			struct {
				float r, g, b, a;
			};
				// Texture coordinates
			struct {
				float s, t, p, q;
			};
		
				// SSE register
			__m128 m;
		};
};	

#include "swizzle.h"

#endif
