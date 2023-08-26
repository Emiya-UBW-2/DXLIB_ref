#pragma once
#include "DXLib_ref.h"

//VECTOR_refに関して
/*=============================================================================
  Copyright (C) 2020 yumetodo <yume-wikijp@live.jp>
  Distributed under the Boost Software License, Version 1.0.
  (See https://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
namespace DXLib_ref {
	class VECTOR_ref {
		VECTOR value;
	public:
		VECTOR_ref() noexcept : value(DxLib::VGet(0, 0, 0)) {}
		VECTOR_ref(VECTOR value) { this->value = value; }
		//
		bool operator==(const VECTOR_ref& obj) const noexcept {
			return (*this - obj).Length() <= 0.001f;
		}
		bool operator!=(const VECTOR_ref& obj) const noexcept {
			return !(*this == obj);
		}
		VECTOR_ref operator+(const VECTOR_ref& obj) const noexcept { return VECTOR_ref(DxLib::VAdd(this->value, obj.value)); }
		VECTOR_ref operator+=(const VECTOR_ref& obj) {
			this->value = DxLib::VAdd(this->value, obj.value);
			return this->value;
		}
		//減算
		VECTOR_ref operator-(const VECTOR_ref& obj) const noexcept { return VECTOR_ref(DxLib::VSub(this->value, obj.value)); }
		VECTOR_ref operator-=(const VECTOR_ref& obj) {
			this->value = DxLib::VSub(this->value, obj.value);
			return this->value;
		}
		//乗算
		VECTOR_ref operator*(float p1) const noexcept { return VECTOR_ref(DxLib::VScale(this->value, p1)); }
		VECTOR_ref operator*=(float p1) {
			this->value = DxLib::VScale(this->value, p1);
			return this->value;
		}
		//除算
		VECTOR_ref operator/(float p1) const noexcept { return VECTOR_ref(DxLib::VScale(this->value, 1.f / p1)); }
		VECTOR_ref operator/=(float p1) {
			this->value = DxLib::VScale(this->value, 1.f / p1);
			return this->value;
		}
		//外積
		VECTOR_ref cross(const VECTOR_ref& obj) const noexcept { return VECTOR_ref(DxLib::VCross(this->value, obj.value)); }
		//内積
		float dot(const VECTOR_ref& obj) const noexcept { return DxLib::VDot(this->value, obj.value); }
		//正規化
		VECTOR_ref Norm(void) const noexcept { return VECTOR_ref(DxLib::VNorm(this->value)); }
		//リセット化
		void clear(void) noexcept { this->value = DxLib::VGet(0, 0, 0); }
		//サイズ
		float size(void) const noexcept { return DxLib::VSize(this->value); }
		float Length(void) const noexcept { return DxLib::VSize(this->value); }
		//出力
		VECTOR get(void) const noexcept { return this->value; }
		float x(void) const noexcept { return this->value.x; }
		float y(void) const noexcept { return this->value.y; }
		float z(void) const noexcept { return this->value.z; }
		//入力
		static const VECTOR_ref vget(float x, float y, float z) noexcept { return DxLib::VGet(x, y, z); }
		void Set(float x, float y, float z) noexcept { this->value = DxLib::VGet(x, y, z); }
		float x(float p) noexcept { this->value.x = p; return this->value.x; }
		float y(float p) noexcept { this->value.y = p; return this->value.y; }
		float z(float p) noexcept { this->value.z = p; return this->value.z; }

		float xadd(float p) noexcept { this->value.x += p; return this->value.x; }
		float yadd(float p) noexcept { this->value.y += p; return this->value.y; }
		float zadd(float p) noexcept { this->value.z += p; return this->value.z; }
		//ベーシック
		static const VECTOR_ref zero(void) noexcept { return DxLib::VGet(0.f, 0.f, 0.f); }

		static const VECTOR_ref right(void) noexcept { return DxLib::VGet(1.f, 0.f, 0.f); }
		static const VECTOR_ref up(void) noexcept { return DxLib::VGet(0.f, 1.f, 0.f); }
		static const VECTOR_ref front(void) noexcept { return DxLib::VGet(0.f, 0.f, 1.f); }
	};
	class MATRIX_ref {
		MATRIX value;
	public:
		MATRIX_ref() noexcept : value(DxLib::MGetIdent()) {}
		MATRIX_ref(MATRIX value) { this->value = value; }
		//加算
		MATRIX_ref operator+(MATRIX_ref obj)  const noexcept { return MATRIX_ref(DxLib::MAdd(this->value, obj.value)); }
		MATRIX_ref operator+=(MATRIX_ref obj) noexcept {
			this->value = DxLib::MAdd(this->value, obj.value);
			return this->value;
		}
		//乗算
		MATRIX_ref operator*(MATRIX_ref obj)  const noexcept { return MATRIX_ref(DxLib::MMult(this->value, obj.value)); }
		MATRIX_ref operator*=(MATRIX_ref obj) noexcept {
			this->value = DxLib::MMult(this->value, obj.value);
			return this->value;
		}
		//サイズ変更
		MATRIX_ref Scale(float p1) const noexcept { return MATRIX_ref(DxLib::MScale(this->value, p1)); }
		//逆
		MATRIX_ref Inverse(void) const noexcept { return MATRIX_ref(DxLib::MInverse(this->value)); }
		//リセット化
		void clear(void) noexcept { this->value = DxLib::MGetIdent(); }
		//出力
		MATRIX get(void) const noexcept { return this->value; }
		//
		static MATRIX_ref Axis1_YZ(const VECTOR_ref& yvec, const VECTOR_ref& zvec) noexcept { return { DxLib::MGetAxis1((yvec.cross(zvec)).get(),yvec.get(),zvec.get(),VGet(0,0,0)) }; }
		static MATRIX_ref Axis1_YZ(const VECTOR_ref& yvec, const VECTOR_ref& zvec, const VECTOR_ref& pos) noexcept { return { DxLib::MGetAxis1((yvec.cross(zvec)).get(),yvec.get(),zvec.get(),pos.get()) }; }

		static MATRIX_ref Axis1(const VECTOR_ref& xvec, const VECTOR_ref& yvec, const VECTOR_ref& zvec) noexcept { return { DxLib::MGetAxis1(xvec.get(),yvec.get(),zvec.get(),VGet(0,0,0)) }; }
		static MATRIX_ref Axis1(const VECTOR_ref& xvec, const VECTOR_ref& yvec, const VECTOR_ref& zvec, const VECTOR_ref& pos) noexcept { return { DxLib::MGetAxis1(xvec.get(),yvec.get(),zvec.get(),pos.get()) }; }
		static MATRIX_ref Axis2(const VECTOR_ref& xvec, const VECTOR_ref& yvec, const VECTOR_ref& zvec) noexcept { return { DxLib::MGetAxis2(xvec.get(),yvec.get(),zvec.get(),VGet(0,0,0)) }; }
		static MATRIX_ref Axis2(const VECTOR_ref& xvec, const VECTOR_ref& yvec, const VECTOR_ref& zvec, const VECTOR_ref& pos) noexcept { return { DxLib::MGetAxis2(xvec.get(),yvec.get(),zvec.get(),pos.get()) }; }
		static MATRIX_ref RotX(float rad) noexcept { return { DxLib::MGetRotX(rad) }; }
		static MATRIX_ref RotY(float rad) noexcept { return { DxLib::MGetRotY(rad) }; }
		static MATRIX_ref RotZ(float rad) noexcept { return { DxLib::MGetRotZ(rad) }; }
		static MATRIX_ref RotAxis(const VECTOR_ref& p1, float p2) { return DxLib::MGetRotAxis(p1.get(), p2); }
		static MATRIX_ref RotVec2(const VECTOR_ref& p1, const VECTOR_ref& p2) noexcept { return { DxLib::MGetRotVec2(p1.get(), p2.get()) }; }
		static MATRIX_ref GetScale(float scale) noexcept { return { DxLib::MGetScale(DxLib::VGet(scale,scale,scale)) }; }
		static MATRIX_ref GetScale(const VECTOR_ref& scale) noexcept { return { DxLib::MGetScale(scale.get()) }; }
		static MATRIX_ref Mtrans(const VECTOR_ref& p1) { return DxLib::MGetTranslate(p1.get()); }
		static VECTOR_ref Vtrans(const VECTOR_ref& p1, const MATRIX_ref& p2) { return DxLib::VTransform(p1.get(), p2.get()); }
		static MATRIX_ref zero() noexcept { return DxLib::MGetIdent(); }

		VECTOR_ref pos(void) const noexcept { return Vtrans(VECTOR_ref::vget(0.f, 0.f, 0.f), this->value); }
		MATRIX_ref GetRot(void) const noexcept { return MATRIX_ref(this->value) * Mtrans(this->pos()).Inverse(); }
		VECTOR_ref xvec(void) const noexcept { return Vtrans(VECTOR_ref::vget(1.f, 0.f, 0.f), GetRot()); }
		VECTOR_ref yvec(void) const noexcept { return Vtrans(VECTOR_ref::up(), GetRot()); }
		VECTOR_ref zvec(void) const noexcept { return Vtrans(VECTOR_ref::front(), GetRot()); }
	};
}