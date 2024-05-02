#pragma once
#include "DXLib_ref.h"

namespace DXLib_ref {
	class Vector3DX {
	public:
		float x{0};
		float y{0};
		float z{0};
	public:
		Vector3DX() noexcept : x(0), y(0), z(0) {}
		Vector3DX(VECTOR value) noexcept { this->Set(value.x, value.y, value.z); }
		VECTOR get(void) const noexcept { return DxLib::VGet(this->x, this->y, this->z); }					//ïœä∑
		static const Vector3DX vget(float x, float y, float z) noexcept { return DxLib::VGet(x, y, z); }	//ì¸óÕ
		//
		static const Vector3DX back(void) noexcept { return DxLib::VGet(0.f, 0.f, -1.f); }
		static const Vector3DX down(void) noexcept { return DxLib::VGet(0.f, -1.f, 0.f); }
		static const Vector3DX forward(void) noexcept { return DxLib::VGet(0.f, 0.f, 1.f); }
		static const Vector3DX left(void) noexcept { return DxLib::VGet(-1.f, 0.f, 0.f); }

		static const Vector3DX one(void) noexcept { return DxLib::VGet(1.f, 1.f, 1.f); }

		static const Vector3DX right(void) noexcept { return DxLib::VGet(1.f, 0.f, 0.f); }
		static const Vector3DX up(void) noexcept { return DxLib::VGet(0.f, 1.f, 0.f); }
		static const Vector3DX zero(void) noexcept { return DxLib::VGet(0.f, 0.f, 0.f); }
		//
		float magnitude(void) const noexcept { return DxLib::VSize(this->get()); }				//ÉTÉCÉY
		Vector3DX normalized(void) const noexcept { return DxLib::VNorm(this->get()); }		//ê≥ãKâª
		float sqrMagnitude(void) const noexcept { return DxLib::VSquareSize(this->get()); }		//ÉTÉCÉY
		//
		bool Equals(const Vector3DX& obj) const noexcept { return *this == obj; }
		void Set(float x1, float y1, float z1) noexcept {
			this->x = x1;
			this->y = y1;
			this->z = z1;
		}
		//
		static const float			Angle(const Vector3DX& A, const Vector3DX& B) noexcept { return std::acos(Vector3DX::Dot(A, B) / (A.magnitude() * B.magnitude())); }
		static const Vector3DX		ClampMagnitude(const Vector3DX& A, float Limit) noexcept {
			auto Len = A.magnitude();
			return A.normalized() * std::clamp(Len,0.f,Limit);
		}
		static const Vector3DX		Cross(const Vector3DX& A, const Vector3DX& B) noexcept { return DxLib::VCross(A.get(), B.get()); }
		static const float			Distance(const Vector3DX& A, const Vector3DX& B) noexcept { return (A - B).magnitude(); }
		static const float			Dot(const Vector3DX& A, const Vector3DX& B) noexcept { return DxLib::VDot(A.get(), B.get()); }
		//static const Vector3DX		Lerp(const Vector3DX& A, const Vector3DX& B, float Per) noexcept { return DXLib_ref::Lerp<Vector3DX>(A, B, Per); }
		//LerpUnclamped
		static const Vector3DX		Max(const Vector3DX& A, const Vector3DX& B) noexcept { return DxLib::VGet(std::max(A.x, B.x), std::max(A.y, B.y), std::max(A.z, B.z)); }
		static const Vector3DX		Min(const Vector3DX& A, const Vector3DX& B) noexcept { return DxLib::VGet(std::min(A.x, B.x), std::min(A.y, B.y), std::min(A.z, B.z)); }
		//MoveTowards
		static const Vector3DX		Normalize(const Vector3DX& A) noexcept { return A.normalized(); }
		//OrthoNormalize
		//Project
		//ProjectOnPlane
		static const Vector3DX		Reflect(const Vector3DX& inDirection, const Vector3DX& inNormal) noexcept { return inDirection + inNormal * (Vector3DX::Dot(inNormal, inDirection*-1.f))*2.f; }
		//RotateTowards
		static const Vector3DX		Scale(const Vector3DX& A, const Vector3DX& B) noexcept { return DxLib::VGet((A.x * B.x), (A.y * B.y), (A.z * B.z)); }
		//SignedAngle
		//Slerp
		//SlerpUnclamped
		//SmoothDamp

		//î‰är
		bool operator==(const Vector3DX& obj) const noexcept { return (*this - obj).magnitude() <= 0.001f; }
		bool operator!=(const Vector3DX& obj) const noexcept { return !(*this == obj); }
		//â¡éZ
		Vector3DX operator+(const Vector3DX& obj) const noexcept { return DxLib::VAdd(this->get(), obj.get()); }
		void operator+=(const Vector3DX& obj) noexcept { *this = *this + obj; }
		//å∏éZ
		Vector3DX operator-(const Vector3DX& obj) const noexcept { return DxLib::VSub(this->get(), obj.get()); }
		void operator-=(const Vector3DX& obj) noexcept { *this = *this - obj; }
		//èÊéZ
		Vector3DX operator*(float p1) const noexcept { return DxLib::VScale(this->get(), p1); }
		void operator*=(float p1) noexcept { *this = *this * p1; }
		//èúéZ
		Vector3DX operator/(float p1) const noexcept { return DxLib::VScale(this->get(), 1.f / p1); }
		void operator/=(float p1) noexcept { *this = *this / p1; }
	};
	class Matrix4x4DX {
		MATRIX value;
	public:
		Matrix4x4DX() noexcept : value(DxLib::MGetIdent()) {}
		Matrix4x4DX(MATRIX value) noexcept { this->value = value; }
		MATRIX get(void) const noexcept { return this->value; }		//ïœä∑
		//
		static const Matrix4x4DX identity() noexcept { return DxLib::MGetIdent(); }

		static Matrix4x4DX Axis1(const Vector3DX& yvec, const Vector3DX& zvec, const Vector3DX& pos = Vector3DX::zero()) noexcept { return {DxLib::MGetAxis1(Vector3DX::Cross(yvec, zvec).get(),yvec.get(),zvec.get(),pos.get())}; }
		static Matrix4x4DX Axis2(const Vector3DX& yvec, const Vector3DX& zvec, const Vector3DX& pos = Vector3DX::zero()) noexcept { return {DxLib::MGetAxis2(Vector3DX::Cross(yvec, zvec).get(),yvec.get(),zvec.get(),pos.get())}; }
		static Matrix4x4DX RotAxis(const Vector3DX& p1, float p2) noexcept { return DxLib::MGetRotAxis(p1.get(), p2); }
		static Matrix4x4DX RotVec2(const Vector3DX& p1, const Vector3DX& p2) noexcept { return {DxLib::MGetRotVec2(p1.get(), p2.get())}; }
		static Matrix4x4DX GetScale(float scale) noexcept { return {DxLib::MGetScale(DxLib::VGet(scale,scale,scale))}; }
		static Matrix4x4DX GetScale(const Vector3DX& scale) noexcept { return {DxLib::MGetScale(scale.get())}; }
		static Matrix4x4DX Mtrans(const Vector3DX& p1) noexcept { return DxLib::MGetTranslate(p1.get()); }
		static Vector3DX Vtrans(const Vector3DX& p1, const Matrix4x4DX& p2) noexcept { return DxLib::VTransform(p1.get(), p2.get()); }
		//
		const Matrix4x4DX inverse(void) const noexcept { return DxLib::MInverse(this->get()); }		//ãt
		const bool isIdentity(void) const noexcept { return *this == identity(); }		//ãt
		//lossyScale
		const Vector3DX lossyScale(void) const noexcept { return DxLib::MGetSize(this->get()); }
		const Matrix4x4DX rotation(void) const noexcept { return DxLib::MGetRotElem(this->get()); }		//ãt
		const Matrix4x4DX transpose(void) const noexcept { return DxLib::MTranspose(this->get()); }		//ãt

		const Vector3DX pos(void) const noexcept { return Vtrans(Vector3DX::zero(), this->get()); }
		const Vector3DX xvec(void) const noexcept { return Vtrans(Vector3DX::right(), rotation()); }
		const Vector3DX yvec(void) const noexcept { return Vtrans(Vector3DX::up(), rotation()); }
		const Vector3DX zvec(void) const noexcept { return Vtrans(Vector3DX::forward(), rotation()); }
		//î‰är
		bool operator==(const Matrix4x4DX& obj) const noexcept {
			for (int x = 0;x < 4;x++) {
				for (int y = 0;y < 4;y++) {
					if (this->get().m[x][y] != obj.get().m[x][y]) {
						return false;
					}
				}
			}
			return true;
		}
		bool operator!=(const Matrix4x4DX& obj) const noexcept { return !(*this == obj); }
		//èÊéZ
		Matrix4x4DX operator*(const Matrix4x4DX& obj)  const noexcept { return DxLib::MMult(this->get(), obj.get()); }
		void operator*=(const Matrix4x4DX& obj) noexcept { *this = *this * obj; }
	};
}