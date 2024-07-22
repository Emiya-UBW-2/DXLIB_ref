#pragma once
#include "DXLib_ref.h"

namespace DXLibRef {
	//---------------------------------------------------------------------------------------------
	//Vector
	//---------------------------------------------------------------------------------------------
	class Vector3DX {
	public:
		float x{0};
		float y{0};
		float z{0};
	public:
		Vector3DX(void) noexcept : x(0), y(0), z(0) {}
		Vector3DX(VECTOR value) noexcept { this->Set(value.x, value.y, value.z); }
		VECTOR get(void) const noexcept { return DxLib::VGet(this->x, this->y, this->z); }					//変換
		static const Vector3DX vget(float x, float y, float z) noexcept { return DxLib::VGet(x, y, z); }	//入力
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
		float magnitude(void) const noexcept { return DxLib::VSize(this->get()); }				//サイズ
		Vector3DX normalized(void) const noexcept { return DxLib::VNorm(this->get()); }		//正規化
		float sqrMagnitude(void) const noexcept { return DxLib::VSquareSize(this->get()); }		//サイズ
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

		//比較
		bool operator==(const Vector3DX& obj) const noexcept { return (*this - obj).magnitude() <= 0.001f; }
		bool operator!=(const Vector3DX& obj) const noexcept { return !(*this == obj); }
		//加算
		Vector3DX operator+(const Vector3DX& obj) const noexcept { return DxLib::VAdd(this->get(), obj.get()); }
		void operator+=(const Vector3DX& obj) noexcept { *this = *this + obj; }
		//減算
		Vector3DX operator-(const Vector3DX& obj) const noexcept { return DxLib::VSub(this->get(), obj.get()); }
		void operator-=(const Vector3DX& obj) noexcept { *this = *this - obj; }
		//乗算
		Vector3DX operator*(float p1) const noexcept { return DxLib::VScale(this->get(), p1); }
		void operator*=(float p1) noexcept { *this = *this * p1; }
		//除算
		Vector3DX operator/(float p1) const noexcept { return DxLib::VScale(this->get(), 1.f / p1); }
		void operator/=(float p1) noexcept { *this = *this / p1; }
	};
	//---------------------------------------------------------------------------------------------
	//Matrix 4x4バージョン
	//---------------------------------------------------------------------------------------------
	class Matrix4x4DX {
		MATRIX value;
	public:
		Matrix4x4DX(void) noexcept : value(DxLib::MGetIdent()) {}
		Matrix4x4DX(MATRIX value) noexcept { this->value = value; }
		MATRIX get(void) const noexcept { return this->value; }		//変換
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
		Matrix4x4DX inverse(void) const noexcept { return DxLib::MInverse(this->get()); }		//逆
		bool isIdentity(void) const noexcept { return *this == DxLib::MGetIdent(); }		//逆
		//lossyScale
		Vector3DX lossyScale(void) const noexcept { return DxLib::MGetSize(this->get()); }
		Matrix4x4DX rotation(void) const noexcept { return DxLib::MGetRotElem(this->get()); }		//逆
		Matrix4x4DX transpose(void) const noexcept { return DxLib::MTranspose(this->get()); }		//逆

		Vector3DX pos(void) const noexcept { return Vtrans(Vector3DX::zero(), this->get()); }
		Vector3DX xvec(void) const noexcept { return Vtrans(Vector3DX::right(), rotation()); }
		Vector3DX yvec(void) const noexcept { return Vtrans(Vector3DX::up(), rotation()); }
		Vector3DX zvec(void) const noexcept { return Vtrans(Vector3DX::forward(), rotation()); }
		//
		void GetRadian(float* angle_x, float* angle_y, float* angle_z) const noexcept {
			constexpr float threshold = 0.001f;
			if (std::abs(value.m[1][2] - 1.0f) < threshold) { // R(2,1) = sin(x) = 1の時
				if (angle_x) { *angle_x = DX_PI_F / 2.f; }
				if (angle_y) { *angle_y = 0.f; }
				if (angle_z) { *angle_z = std::atan2f(value.m[0][1], value.m[0][0]); }
			}
			else if (std::abs(value.m[1][2] + 1.0f) < threshold) { // R(2,1) = sin(x) = -1の時
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
		//比較
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
		//乗算
		Matrix4x4DX operator*(const Matrix4x4DX& obj)  const noexcept { return DxLib::MMult(this->get(), obj.get()); }
		void operator*=(const Matrix4x4DX& obj) noexcept { *this = *this * obj; }
	};
	//---------------------------------------------------------------------------------------------
	//Matrix 3x3バージョン
	//---------------------------------------------------------------------------------------------
	// 行列構造体
	struct MATRIX33
	{
		float					m[3][3];
	};
	//相互変換
	static void M33toMATRIX(MATRIX* pTarget, const MATRIX33& pAtr)
	{
		*pTarget = DxLib::MGetIdent();
		for (int x = 0; x < 3; x++) {
			for (int y = 0; y < 3; y++) {
				pTarget->m[x][y] = pAtr.m[x][y];
			}
		}
	}
	static void MATRIXtoM33(MATRIX33* pTarget, const MATRIX& pAtr)
	{
		for (int x = 0; x < 3; x++) {
			for (int y = 0; y < 3; y++) {
				pTarget->m[x][y] = pAtr.m[x][y];
			}
		}
	}

	// 単位行列を得る
	static MATRIX33 M33GetIdent(void)
	{
		static MATRIX33 Result =
		{
			{
				{ 1.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f },
				{ 0.0f, 0.0f, 1.0f }
			}
		};
		return Result;
	}
	// 指定の３軸ローカルのベクトルを基本軸上のベクトルに変換する行列を得る
	// x' = XAxis.x * x + YAixs.x * y + ZAxis.z * z + Pos.x
	// y' = XAxis.y * x + YAixs.y * y + ZAxis.y * z + Pos.y
	// z' = XAxis.z * x + YAixs.z * y + ZAxis.z * z + Pos.z
	static MATRIX33 M33GetAxis1(VECTOR XAxis, VECTOR YAxis, VECTOR ZAxis)
	{
		MATRIX33 Result =
		{
			{
				{ XAxis.x, XAxis.y, XAxis.z },
				{ YAxis.x, YAxis.y, YAxis.z },
				{ ZAxis.x, ZAxis.y, ZAxis.z }
			}
		};
		return Result;
	}
	// 基本軸上のベクトルを指定の３軸上に投影したベクトルに変換する行列を得る
	// x' = XAxis.x * ( x - Pos.x ) + XAxis.y * ( x - Pos.x ) + XAxis.z * ( x - Pos.x )
	// y' = YAxis.x * ( x - Pos.x ) + YAxis.y * ( x - Pos.x ) + YAxis.z * ( x - Pos.x )
	// z' = ZAxis.x * ( x - Pos.x ) + ZAxis.y * ( x - Pos.x ) + ZAxis.z * ( x - Pos.x )
	static MATRIX33 M33GetAxis2(VECTOR XAxis, VECTOR YAxis, VECTOR ZAxis)
	{
		MATRIX33 Result =
		{
			{
				{ XAxis.x, YAxis.x, ZAxis.x },
				{ XAxis.y, YAxis.y, ZAxis.y },
				{ XAxis.z, YAxis.z, ZAxis.z }
			}
		};
		return Result;
	}
	// 指定軸で指定角度回転する行列を得る
	static MATRIX33 M33GetRotAxis(VECTOR RotateAxis, float Rotate)
	{
		MATRIX33 Result{};
		VECTOR xv = { 1.0f, 0.0f, 0.0f }, yv = { 0.0f, 1.0f, 0.0f }, zv = { 0.0f, 0.0f, 1.0f };
		VECTOR xv2{}, yv2{}, zv2{}, xv3{}, yv3{}, zv3{};
		float f{}, Sin{}, Cos{};

		zv2 = VNorm(RotateAxis);
		yv2 = VGet(0.0f, 1.0f, 0.0f);
		if (VSquareSize(VCross(yv2, zv2)) < 0.00001f)
			yv2 = VGet(0.0f, 0.0f, 1.0f);
		xv2 = VCross(zv2, yv2);
		yv2 = VNorm(VCross(xv2, zv2));
		xv2 = VNorm(xv2);

		xv3.x = xv2.x * xv.x + xv2.y * xv.y + xv2.z * xv.z;
		xv3.y = yv2.x * xv.x + yv2.y * xv.y + yv2.z * xv.z;
		xv3.z = zv2.x * xv.x + zv2.y * xv.y + zv2.z * xv.z;

		yv3.x = xv2.x * yv.x + xv2.y * yv.y + xv2.z * yv.z;
		yv3.y = yv2.x * yv.x + yv2.y * yv.y + yv2.z * yv.z;
		yv3.z = zv2.x * yv.x + zv2.y * yv.y + zv2.z * yv.z;

		zv3.x = xv2.x * zv.x + xv2.y * zv.y + xv2.z * zv.z;
		zv3.y = yv2.x * zv.x + yv2.y * zv.y + yv2.z * zv.z;
		zv3.z = zv2.x * zv.x + zv2.y * zv.y + zv2.z * zv.z;

		Sin = std::sinf(Rotate);
		Cos = std::cosf(Rotate);
		//_SINCOS_PLATFORM(Rotate, &Sin, &Cos);
		f = xv3.x * Cos - xv3.y * Sin;
		xv3.y = xv3.x * Sin + xv3.y * Cos;
		xv3.x = f;

		f = yv3.x * Cos - yv3.y * Sin;
		yv3.y = yv3.x * Sin + yv3.y * Cos;
		yv3.x = f;

		f = zv3.x * Cos - zv3.y * Sin;
		zv3.y = zv3.x * Sin + zv3.y * Cos;
		zv3.x = f;

		xv.x = xv2.x * xv3.x + yv2.x * xv3.y + zv2.x * xv3.z;
		xv.y = xv2.y * xv3.x + yv2.y * xv3.y + zv2.y * xv3.z;
		xv.z = xv2.z * xv3.x + yv2.z * xv3.y + zv2.z * xv3.z;

		yv.x = xv2.x * yv3.x + yv2.x * yv3.y + zv2.x * yv3.z;
		yv.y = xv2.y * yv3.x + yv2.y * yv3.y + zv2.y * yv3.z;
		yv.z = xv2.z * yv3.x + yv2.z * yv3.y + zv2.z * yv3.z;

		zv.x = xv2.x * zv3.x + yv2.x * zv3.y + zv2.x * zv3.z;
		zv.y = xv2.y * zv3.x + yv2.y * zv3.y + zv2.y * zv3.z;
		zv.z = xv2.z * zv3.x + yv2.z * zv3.y + zv2.z * zv3.z;

		Result.m[0][0] = xv.x;
		Result.m[1][0] = xv.y;
		Result.m[2][0] = xv.z;

		Result.m[0][1] = yv.x;
		Result.m[1][1] = yv.y;
		Result.m[2][1] = yv.z;

		Result.m[0][2] = zv.x;
		Result.m[1][2] = zv.y;
		Result.m[2][2] = zv.z;

		return Result;
	}
	// In1 の向きから In2 の向きへ変換する回転行列を得る
	static MATRIX33 M33GetRotVec2(VECTOR In1, VECTOR In2)
	{
		VECTOR av;
		float rad;

		av = VCross(In1, In2);
		if (VSquareSize(av) < 0.0000001f)
			av = VGet(1.0f, 0.0f, 0.0f);
		rad = VRad(In1, In2);
		return M33GetRotAxis(av, rad);
	}
	// 逆行列を得る
	static MATRIX33 M33Inverse(MATRIX33 InM)
	{
		MATRIX In;
		MATRIX33 Result;
		M33toMATRIX(&In, InM);
		In = DxLib::MInverse(In);
		MATRIXtoM33(&Result, In);
		return Result;
	}
	// 転置行列を得る
	static MATRIX33 M33Transpose(MATRIX33 InM)
	{
		MATRIX33 Result =
		{
			{
				{ InM.m[0][0], InM.m[1][0], InM.m[2][0] },
				{ InM.m[0][1], InM.m[1][1], InM.m[2][1] },
				{ InM.m[0][2], InM.m[1][2], InM.m[2][2] }
			}
		};
		return Result;
	}
	// 行列の乗算を行う
	static MATRIX33 M33Mult(MATRIX33 In1, MATRIX33 In2)
	{
		MATRIX33 Result =
		{
			{
				{
					In1.m[0][0] * In2.m[0][0] + In1.m[0][1] * In2.m[1][0] + In1.m[0][2] * In2.m[2][0],
					In1.m[0][0] * In2.m[0][1] + In1.m[0][1] * In2.m[1][1] + In1.m[0][2] * In2.m[2][1],
					In1.m[0][0] * In2.m[0][2] + In1.m[0][1] * In2.m[1][2] + In1.m[0][2] * In2.m[2][2]
				},
				{
					In1.m[1][0] * In2.m[0][0] + In1.m[1][1] * In2.m[1][0] + In1.m[1][2] * In2.m[2][0],
					In1.m[1][0] * In2.m[0][1] + In1.m[1][1] * In2.m[1][1] + In1.m[1][2] * In2.m[2][1],
					In1.m[1][0] * In2.m[0][2] + In1.m[1][1] * In2.m[1][2] + In1.m[1][2] * In2.m[2][2],
				},
				{
					In1.m[2][0] * In2.m[0][0] + In1.m[2][1] * In2.m[1][0] + In1.m[2][2] * In2.m[2][0],
					In1.m[2][0] * In2.m[0][1] + In1.m[2][1] * In2.m[1][1] + In1.m[2][2] * In2.m[2][1],
					In1.m[2][0] * In2.m[0][2] + In1.m[2][1] * In2.m[1][2] + In1.m[2][2] * In2.m[2][2],
				}
			}
		};
		return Result;
	}
	// 行列を使った座標変換
	static VECTOR		V33Transform(const VECTOR& InV, const MATRIX33& InM)
	{
		VECTOR Result{};
		Result.x = InV.x * InM.m[0][0] + InV.y * InM.m[1][0] + InV.z * InM.m[2][0];
		Result.y = InV.x * InM.m[0][1] + InV.y * InM.m[1][1] + InV.z * InM.m[2][1];
		Result.z = InV.x * InM.m[0][2] + InV.y * InM.m[1][2] + InV.z * InM.m[2][2];
		return Result;
	}

	class Matrix3x3DX {
		MATRIX33 value;
	public:
		Matrix3x3DX(void) noexcept : value(M33GetIdent()) {}
		Matrix3x3DX(MATRIX33 value) noexcept { this->value = value; }
		MATRIX33 get(void) const noexcept { return this->value; }		//変換
		MATRIX Get44(void) const noexcept {
			MATRIX Result;
			M33toMATRIX(&Result, this->value);
			return Result;
		}//変換
		Matrix4x4DX Get44DX(void) const noexcept {
			MATRIX Result;
			M33toMATRIX(&Result, this->value);
			return Result;
		}//変換
		//
		static Matrix3x3DX identity(void) noexcept { return M33GetIdent(); }

		static Matrix3x3DX Axis1(const Vector3DX& yvec, const Vector3DX& zvec) noexcept { return { M33GetAxis1(Vector3DX::Cross(yvec, zvec).get(),yvec.get(),zvec.get()) }; }
		static Matrix3x3DX Axis2(const Vector3DX& yvec, const Vector3DX& zvec) noexcept { return { M33GetAxis2(Vector3DX::Cross(yvec, zvec).get(),yvec.get(),zvec.get()) }; }
		static Matrix3x3DX RotAxis(const Vector3DX& p1, float p2) noexcept { return M33GetRotAxis(p1.get(), p2); }
		static Matrix3x3DX RotVec2(const Vector3DX& p1, const Vector3DX& p2) noexcept { return { M33GetRotVec2(p1.get(), p2.get()) }; }
		static Vector3DX Vtrans(const Vector3DX& p1, const Matrix3x3DX& p2) noexcept { return V33Transform(p1.get(), p2.get()); }
		//
		Matrix3x3DX inverse(void) const noexcept { return M33Inverse(this->get()); }		//逆
		bool isIdentity(void) const noexcept { return *this == M33GetIdent(); }		//逆
		//lossyScale
		Matrix3x3DX transpose(void) const noexcept { return M33Transpose(this->get()); }		//逆

		Vector3DX xvec(void) const noexcept { return Vtrans(Vector3DX::right(), *this); }
		Vector3DX yvec(void) const noexcept { return Vtrans(Vector3DX::up(), *this); }
		Vector3DX zvec(void) const noexcept { return Vtrans(Vector3DX::forward(), *this); }
		//
		void GetRadian(float* angle_x, float* angle_y, float* angle_z) const noexcept {
			constexpr float threshold = 0.001f;
			if (std::abs(value.m[1][2] - 1.0f) < threshold) { // R(2,1) = sin(x) = 1の時
				if (angle_x) { *angle_x = DX_PI_F / 2.f; }
				if (angle_y) { *angle_y = 0.f; }
				if (angle_z) { *angle_z = std::atan2f(value.m[0][1], value.m[0][0]); }
			}
			else if (std::abs(value.m[1][2] + 1.0f) < threshold) { // R(2,1) = sin(x) = -1の時
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
		//比較
		bool operator==(const Matrix3x3DX& obj) const noexcept {
			for (int x = 0; x < 3; x++) {
				for (int y = 0; y < 3; y++) {
					if (this->get().m[x][y] != obj.get().m[x][y]) {
						return false;
					}
				}
			}
			return true;
		}
		bool operator!=(const Matrix3x3DX& obj) const noexcept { return !(*this == obj); }
		//乗算
		Matrix3x3DX operator*(const Matrix3x3DX& obj)  const noexcept { return M33Mult(this->get(), obj.get()); }
		void operator*=(const Matrix3x3DX& obj) noexcept { *this = *this * obj; }
	};
}