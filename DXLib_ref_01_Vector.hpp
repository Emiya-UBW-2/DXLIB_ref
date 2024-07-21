#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	class Vector3DX {
	public:
		float x{0};
		float y{0};
		float z{0};
	public:
		Vector3DX(void) noexcept : x(0), y(0), z(0) {}
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
		static float			Angle(const Vector3DX& A, const Vector3DX& B) noexcept { return std::acos(Vector3DX::Dot(A, B) / (A.magnitude() * B.magnitude())); }
		static Vector3DX		ClampMagnitude(const Vector3DX& A, float Limit) noexcept {
			auto Len = A.magnitude();
			return A.normalized() * std::clamp(Len,0.f,Limit);
		}
		static Vector3DX		Cross(const Vector3DX& A, const Vector3DX& B) noexcept { return DxLib::VCross(A.get(), B.get()); }
		static float			Distance(const Vector3DX& A, const Vector3DX& B) noexcept { return (A - B).magnitude(); }
		static float			Dot(const Vector3DX& A, const Vector3DX& B) noexcept { return DxLib::VDot(A.get(), B.get()); }
		//static Vector3DX		Lerp(const Vector3DX& A, const Vector3DX& B, float Per) noexcept { return Lerp<Vector3DX>(A, B, Per); }
		//LerpUnclamped
		static Vector3DX		Max(const Vector3DX& A, const Vector3DX& B) noexcept { return DxLib::VGet(std::max(A.x, B.x), std::max(A.y, B.y), std::max(A.z, B.z)); }
		static Vector3DX		Min(const Vector3DX& A, const Vector3DX& B) noexcept { return DxLib::VGet(std::min(A.x, B.x), std::min(A.y, B.y), std::min(A.z, B.z)); }
		//MoveTowards
		static Vector3DX		Normalize(const Vector3DX& A) noexcept { return A.normalized(); }
		//OrthoNormalize
		//Project
		//ProjectOnPlane
		static Vector3DX		Reflect(const Vector3DX& inDirection, const Vector3DX& inNormal) noexcept { return inDirection + inNormal * (Vector3DX::Dot(inNormal, inDirection*-1.f))*2.f; }
		//RotateTowards
		static Vector3DX		Scale(const Vector3DX& A, const Vector3DX& B) noexcept { return DxLib::VGet((A.x * B.x), (A.y * B.y), (A.z * B.z)); }
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
		Matrix4x4DX(void) noexcept : value(DxLib::MGetIdent()) {}
		Matrix4x4DX(MATRIX value) noexcept { this->value = value; }
		MATRIX get(void) const noexcept { return this->value; }		//ïœä∑
		//
		static Matrix4x4DX identity(void) noexcept { return DxLib::MGetIdent(); }

		static Matrix4x4DX Axis1(const Vector3DX& yvec, const Vector3DX& zvec, const Vector3DX& pos = Vector3DX::zero()) noexcept { return {DxLib::MGetAxis1(Vector3DX::Cross(yvec, zvec).get(),yvec.get(),zvec.get(),pos.get())}; }
		static Matrix4x4DX Axis2(const Vector3DX& yvec, const Vector3DX& zvec, const Vector3DX& pos = Vector3DX::zero()) noexcept { return {DxLib::MGetAxis2(Vector3DX::Cross(yvec, zvec).get(),yvec.get(),zvec.get(),pos.get())}; }
		static Matrix4x4DX RotAxis(const Vector3DX& p1, float p2) noexcept { return DxLib::MGetRotAxis(p1.get(), p2); }
		static Matrix4x4DX RotVec2(const Vector3DX& p1, const Vector3DX& p2) noexcept { return {DxLib::MGetRotVec2(p1.get(), p2.get())}; }
		static Matrix4x4DX GetScale(float scale) noexcept { return {DxLib::MGetScale(DxLib::VGet(scale,scale,scale))}; }
		static Matrix4x4DX GetScale(const Vector3DX& scale) noexcept { return {DxLib::MGetScale(scale.get())}; }
		static Matrix4x4DX Mtrans(const Vector3DX& p1) noexcept { return DxLib::MGetTranslate(p1.get()); }
		static Vector3DX Vtrans(const Vector3DX& p1, const Matrix4x4DX& p2) noexcept { return DxLib::VTransform(p1.get(), p2.get()); }
		//
		Matrix4x4DX inverse(void) const noexcept { return DxLib::MInverse(this->get()); }		//ãt
		bool isIdentity(void) const noexcept { return *this == DxLib::MGetIdent(); }		//ãt
		//lossyScale
		Vector3DX lossyScale(void) const noexcept { return DxLib::MGetSize(this->get()); }
		Matrix4x4DX rotation(void) const noexcept { return DxLib::MGetRotElem(this->get()); }		//ãt
		Matrix4x4DX transpose(void) const noexcept { return DxLib::MTranspose(this->get()); }		//ãt

		Vector3DX pos(void) const noexcept { return Vtrans(Vector3DX::zero(), this->get()); }
		Vector3DX xvec(void) const noexcept { return Vtrans(Vector3DX::right(), rotation()); }
		Vector3DX yvec(void) const noexcept { return Vtrans(Vector3DX::up(), rotation()); }
		Vector3DX zvec(void) const noexcept { return Vtrans(Vector3DX::forward(), rotation()); }
		//
		void GetRadian(float* angle_x, float* angle_y, float* angle_z) const noexcept {
			constexpr float threshold = 0.001f;
			if (std::abs(value.m[1][2] - 1.0f) < threshold) { // R(2,1) = sin(x) = 1ÇÃéû
				if (angle_x) { *angle_x = DX_PI_F / 2.f; }
				if (angle_y) { *angle_y = 0.f; }
				if (angle_z) { *angle_z = std::atan2f(value.m[0][1], value.m[0][0]); }
			}
			else if (std::abs(value.m[1][2] + 1.0f) < threshold) { // R(2,1) = sin(x) = -1ÇÃéû
				if (angle_x) { *angle_x = -DX_PI_F / 2.f; }
				if (angle_y) { *angle_y = 0.f; }
				if (angle_z) { *angle_z = std::atan2f(value.m[0][1], value.m[0][0]); }
			}
			else {
				if (angle_x) { *angle_x = std::asinf(value.m[1][2]); }
				if (angle_y) { *angle_y = std::atan2f(-value.m[0][2], value.m[2][2]); }
				if (angle_z) { *angle_z = std::atan2f(-value.m[1][0], value.m[1][1]); }
			}
		}
		void SetRadian(float x, float y, float z) noexcept {
			value.m[0][0] = std::cos(y) * std::cos(z) - std::sin(x) * std::sin(y) * std::sin(z);
			value.m[1][0] = -std::cos(x) * std::sin(z);
			value.m[2][0] = std::sin(y) * std::cos(z) + std::sin(x) * std::cos(y) * std::sin(z);
			value.m[0][1] = std::cos(y) * std::sin(z) + std::sin(x) * std::sin(y) * std::cos(z);
			value.m[1][1] = std::cos(x) * std::cos(z);
			value.m[2][1] = std::sin(y) * std::sin(z) - std::sin(x) * std::cos(y) * std::cos(z);
			value.m[0][2] = -std::cos(x) * std::sin(y);
			value.m[1][2] = std::sin(x);
			value.m[2][2] = std::cos(x) * std::cos(y);
		}
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