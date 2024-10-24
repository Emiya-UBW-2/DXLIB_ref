#pragma once
#include "DXLib_ref.h"
// リサイズ
#define EdgeSize	DXDraw::Instance()->GetUIY(2)
#define LineHeight	DXDraw::Instance()->GetUIY(18)

namespace DXLibRef {
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// 補完
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// イージング
	enum class EasingType {
		OutExpo,
	};
	extern float GetEasingRatio(EasingType EasingType, float ratio) noexcept;
	// 線形補完
	template <class T>
	inline T Lerp(const T& A, const T& B, float Per) noexcept {
		if (Per == 0.f) {
			return A;
		}
		else if (Per == 1.f) {
			return B;
		}
		else {
			return A + (T)((B - A) * Per);
		}
	}
	// Matrix版の線形補完
	inline Matrix4x4DX Lerp(const Matrix4x4DX& A, const Matrix4x4DX& B, float Per) noexcept {
		return Matrix4x4DX::Axis1(
			Lerp(A.yvec(), B.yvec(), Per).normalized(),
			Lerp(A.zvec(), B.zvec(), Per).normalized(),
			Lerp(A.pos(), B.pos(), Per));
	}
	inline Matrix3x3DX Lerp(const Matrix3x3DX& A, const Matrix3x3DX& B, float Per) noexcept {
		return Matrix3x3DX::Axis1(
			Lerp(A.yvec(), B.yvec(), Per).normalized(),
			Lerp(A.zvec(), B.zvec(), Per).normalized());
	}
	// 
	template <class T>
	static void Easing(T* A, const T& B, float ratio, EasingType EasingType) noexcept { *A = Lerp(*A, B, GetEasingRatio(EasingType, ratio)); }
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*関数																																		*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// クリップボードに画像をコピー
	static auto GetClipBoardGraphHandle(GraphHandle* RetHandle) noexcept {
		HWND  hwnd = GetMainWindowHandle();

		if (IsClipboardFormatAvailable(CF_BITMAP) == FALSE) {
			return false;
		}	// 指定した形式のデータがクリップボードにあるかを問い合わせる
		if (OpenClipboard(hwnd) == FALSE) {
			return false;
		}						// オープン
		HBITMAP hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);					// データ取得
		CloseClipboard();														// クローズ
		if (hBitmap == NULL) {
			return false;
		}

		BITMAP bitmap{};
		GetObject(hBitmap, sizeof(BITMAP), &bitmap);

		BITMAPINFO bmpInfo{};
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth = bitmap.bmWidth;
		bmpInfo.bmiHeader.biHeight = bitmap.bmHeight;
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biBitCount = 32;
		bmpInfo.bmiHeader.biCompression = BI_RGB;

		size_t Siz = static_cast<size_t>(bitmap.bmWidth * bitmap.bmHeight * (LONG)4);
		BYTE* bmpData = new BYTE[Siz];
		HDC hDC = GetDC(hwnd);
		GetDIBits(hDC, hBitmap, 0, static_cast<UINT>(bitmap.bmHeight), (void*)bmpData, &bmpInfo, DIB_RGB_COLORS);
		ReleaseDC(hwnd, hDC);
		DeleteObject(hBitmap);

		RetHandle->CreateGraphFromBmp(&bmpInfo, bmpData);	// ハンドルに変換
		delete[] bmpData;
		return true;
	}

	// 点と矩形との2D判定
	static bool HitPointToRectangle(int xp, int yp, int x1, int y1, int x2, int y2) noexcept { return (xp >= x1 && xp <= x2 && yp >= y1 && yp <= y2); }
	// 矩形と矩形との2D判定
	static bool HitRectangleToRectangle(int xp1, int yp1, int xp2, int yp2, int x1, int y1, int x2, int y2) noexcept { return (xp1 < x2 && x1 < xp2) && (yp1 < y2 && y1 < yp2); }
	// 点と四角形との2D判定
	static bool HitPointToSquare(int xp, int yp, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) noexcept {
		if (0 > Vector2DX::Cross(Vector2DX::vget(static_cast<float>(x2 - x1), static_cast<float>(y2 - y1)), Vector2DX::vget(static_cast<float>(xp - x1), static_cast<float>(yp - y1)))) { return false; }
		if (0 > Vector2DX::Cross(Vector2DX::vget(static_cast<float>(x3 - x2), static_cast<float>(y3 - y2)), Vector2DX::vget(static_cast<float>(xp - x2), static_cast<float>(yp - y2)))) { return false; }
		if (0 > Vector2DX::Cross(Vector2DX::vget(static_cast<float>(x4 - x3), static_cast<float>(y4 - y3)), Vector2DX::vget(static_cast<float>(xp - x3), static_cast<float>(yp - y3)))) { return false; }
		if (0 > Vector2DX::Cross(Vector2DX::vget(static_cast<float>(x1 - x4), static_cast<float>(y1 - y4)), Vector2DX::vget(static_cast<float>(xp - x4), static_cast<float>(yp - y4)))) { return false; }
		return true;
	}

	// マウスと矩形の判定
	extern bool IntoMouse(int x1, int y1, int x2, int y2) noexcept;
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*DXLIBラッパー																																*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// 演算補助
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// 球と三角との判定
	static bool GetHitSphereToTriangle(const Vector3DX& pos, float size, const Vector3DX& tri_p1, const Vector3DX& tri_p2, const Vector3DX& tri_p3) noexcept {
		return HitCheck_Sphere_Triangle(pos.get(), size, tri_p1.get(), tri_p2.get(), tri_p3.get()) == TRUE;
	}
	// 三角とカプセルとの判定
	static bool GetHitCapsuleToTriangle(const Vector3DX& startpos, const Vector3DX& endpos, float size, const Vector3DX& tri_p1, const Vector3DX& tri_p2, const Vector3DX& tri_p3) noexcept {
		return HitCheck_Capsule_Triangle(startpos.get(), endpos.get(), size, tri_p1.get(), tri_p2.get(), tri_p3.get()) == TRUE;
	}
	// 球とカプセルとの判定
	static bool GetHitSphereToCapsule(const Vector3DX& pos, float size, const Vector3DX& tri_p1, const Vector3DX& tri_p2, float size2) noexcept {
		return HitCheck_Sphere_Capsule(pos.get(), size, tri_p1.get(), tri_p2.get(), size2) == TRUE;
	}
	// カプセルとカプセルとの判定
	static bool GetHitCheckToCapsule(const Vector3DX& pos1, const Vector3DX& pos2, float size, const Vector3DX& tri_p1, const Vector3DX& tri_p2, float size2) noexcept {
		return HitCheck_Capsule_Capsule(pos1.get(), pos2.get(), size, tri_p1.get(), tri_p2.get(), size2) == TRUE;
	}
	// 直線と直線の一番近い距離
	static float GetMinLenSegmentToSegment(const Vector3DX& startpos, const Vector3DX& endpos, const Vector3DX& tgtstartpos, const Vector3DX& tgtendpos) noexcept {
		return Segment_Segment_MinLength(startpos.get(), endpos.get(), tgtstartpos.get(), tgtendpos.get());
	}
	// 直線と点の一番近い点
	static float GetMinLenSegmentToPoint(const Vector3DX& startpos, const Vector3DX& endpos, const Vector3DX& tgt) noexcept {
		return Segment_Point_MinLength(startpos.get(), endpos.get(), tgt.get());
	}
	// 平面と点の一番近い点
	static Vector3DX GetMinPosSegmentToPoint(const Vector3DX& startpos, const Vector3DX& endpos, const Vector3DX& tgt) noexcept {
		return Plane_Point_MinLength_Position(startpos.get(), endpos.get(), tgt.get());
	}
	// ランダム
	static float GetRandf(float m_arg) noexcept { return -m_arg + static_cast<float>(GetRand(static_cast<int>(m_arg * 2.f * 10000.f))) / 10000.f; }
	// 線分同士の交差判定
	static bool GetSegmenttoSegment(const Vector3DX& SegmentAPos1, const Vector3DX& SegmentAPos2, const Vector3DX& SegmentBPos1, const Vector3DX& SegmentBPos2, SEGMENT_SEGMENT_RESULT* Result) noexcept {
		VECTOR Pos1t = SegmentAPos1.get();
		VECTOR Pos2t = SegmentAPos2.get();
		VECTOR PosAt = SegmentBPos1.get();
		VECTOR PosBt = SegmentBPos2.get();

		Segment_Segment_Analyse(&Pos1t, &Pos2t, &PosAt, &PosBt, Result);
		float len = 0.001f;
		return (Result->SegA_SegB_MinDist_Square <= (len * len));
	}
	// 線分と三角との交差判定
	static bool GetSegmenttoTriangle(const Vector3DX& SegmentAPos1, const Vector3DX& SegmentAPos2, const Vector3DX& SegmentBPos1, const Vector3DX& SegmentBPos2, const Vector3DX& SegmentBPos3, SEGMENT_TRIANGLE_RESULT* Result) noexcept {
		VECTOR Pos1t = SegmentAPos1.get();
		VECTOR Pos2t = SegmentAPos2.get();
		VECTOR PosAt = SegmentBPos1.get();
		VECTOR PosBt = SegmentBPos2.get();
		VECTOR PosCt = SegmentBPos3.get();

		Segment_Triangle_Analyse(&Pos1t, &Pos2t, &PosAt, &PosBt, &PosCt, Result);
		float len = 0.001f;
		return (Result->Seg_Tri_MinDist_Square <= (len * len));
	}
	// 線分と点との交差判定
	static bool GetSegmenttoPoint(const Vector3DX& SegmentAPos1, const Vector3DX& SegmentAPos2, const Vector3DX& PointPos, SEGMENT_POINT_RESULT* Result) noexcept {
		VECTOR Pos1t = SegmentAPos1.get();
		VECTOR Pos2t = SegmentAPos2.get();
		VECTOR PosAt = PointPos.get();

		Segment_Point_Analyse(&Pos1t, &Pos2t, &PosAt, Result);
		float len = 0.001f;
		return (Result->Seg_Point_MinDist_Square <= (len * len));
	}
	// ファイルが存在するかチェック
	static bool IsFileExist(const char* Path) noexcept {
		FILEINFO FileInfo;
		return (FileRead_findFirst(Path, &FileInfo) != (DWORD_PTR)InvalidID);
	}
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// IK
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	static void			IK_move_Arm(
		const Vector3DX& Localyvec,
		const Vector3DX& Localzvec,
		MV1* pObj,
		float XPer,
		int Arm, const Matrix4x4DX& FrameBaseLocalMatArm,
		int Arm2, const Matrix4x4DX& FrameBaseLocalMatArm2,
		int Wrist, const Matrix4x4DX& FrameBaseLocalMatWrist,
		const Vector3DX& DirPos, const Vector3DX& Diryvec, const Vector3DX& Dirzvec) noexcept {

		pObj->ResetFrameUserLocalMatrix(Arm);
		pObj->ResetFrameUserLocalMatrix(Arm2);
		pObj->ResetFrameUserLocalMatrix(Wrist);
		auto matBase = pObj->GetParentFrameWorldMatrix(Arm).rotation().inverse();

		// 基準
		auto vec_a1 = Matrix4x4DX::Vtrans((DirPos - pObj->GetFramePosition(Arm)).normalized(), matBase);// 基準
		auto vec_a1L1 = Vector3DX(Vector3DX::vget(XPer, -1.f, vec_a1.y / -abs(vec_a1.z))).normalized();// x=0とする
		float cos_t = GetCosFormula((Vector3DX(pObj->GetFramePosition(Wrist)) - pObj->GetFramePosition(Arm2)).magnitude(), (Vector3DX(pObj->GetFramePosition(Arm2)) - pObj->GetFramePosition(Arm)).magnitude(), (Vector3DX(pObj->GetFramePosition(Arm)) - DirPos).magnitude());
		auto vec_t = vec_a1 * cos_t + vec_a1L1 * std::sqrtf(1.f - cos_t * cos_t);
		// 上腕
		pObj->SetFrameLocalMatrix(Arm, Matrix4x4DX::identity() * FrameBaseLocalMatArm);
		Matrix4x4DX a1_inv = Matrix4x4DX::RotVec2(Matrix4x4DX::Vtrans(Vector3DX(pObj->GetFramePosition(Arm2)) - pObj->GetFramePosition(Arm), matBase), vec_t);
		pObj->SetFrameLocalMatrix(Arm, a1_inv * FrameBaseLocalMatArm);
		// 下腕
		matBase = pObj->GetParentFrameWorldMatrix(Arm2).rotation().inverse();
		pObj->SetFrameLocalMatrix(Arm2, Matrix4x4DX::identity() * FrameBaseLocalMatArm2);
		Matrix4x4DX a2_inv = Matrix4x4DX::RotVec2(
			Matrix4x4DX::Vtrans(Vector3DX(pObj->GetFramePosition(Wrist)) - pObj->GetFramePosition(Arm2), matBase),
			Matrix4x4DX::Vtrans(DirPos - pObj->GetFramePosition(Arm2), matBase));
		pObj->SetFrameLocalMatrix(Arm2, a2_inv * FrameBaseLocalMatArm2);
		// 手
		matBase = pObj->GetParentFrameWorldMatrix(Wrist).rotation().inverse();
		Matrix4x4DX mat1;
		{
			auto zvec = Matrix4x4DX::Vtrans(Localzvec, pObj->GetFrameLocalWorldMatrix(Wrist).rotation());
			mat1 = Matrix4x4DX::RotVec2(Matrix4x4DX::Vtrans(zvec, matBase), Matrix4x4DX::Vtrans(Dirzvec, matBase)) * mat1;
			pObj->SetFrameLocalMatrix(Wrist, mat1 * FrameBaseLocalMatWrist);
			auto xvec = Matrix4x4DX::Vtrans(Localyvec, pObj->GetFrameLocalWorldMatrix(Wrist).rotation());
			mat1 = Matrix4x4DX::RotAxis(Localzvec, Vector3DX::Angle(xvec, Vector3DX::Cross(Dirzvec, Diryvec) * -1.f) * ((Vector3DX::Dot(Diryvec, xvec) > 0.f) ? -1.f : 1.f)) * mat1;
		}
		pObj->SetFrameLocalMatrix(Wrist, mat1 * FrameBaseLocalMatWrist);
	}

	static void			IK_RightArm(MV1* pObj,
		int Arm, const Matrix4x4DX& FrameBaseLocalMatArm,
		int Arm2, const Matrix4x4DX& FrameBaseLocalMatArm2,
		int Wrist, const Matrix4x4DX& FrameBaseLocalMatWrist,
		const Vector3DX& DirPos, const Vector3DX& Diryvec, const Vector3DX& Dirzvec) noexcept {
		IK_move_Arm(
			Vector3DX::vget(0.f, 0.f, -1.f).normalized(),
			Vector3DX::vget(-1.f, -1.f, 0.f).normalized(),
			pObj, -0.5f,
			Arm,
			FrameBaseLocalMatArm,
			Arm2,
			FrameBaseLocalMatArm2,
			Wrist,
			FrameBaseLocalMatWrist,
			DirPos, Diryvec * -1.f, Dirzvec);
	}
	static void			IK_LeftArm(MV1* pObj,
		int Arm, const Matrix4x4DX& FrameBaseLocalMatArm,
		int Arm2, const Matrix4x4DX& FrameBaseLocalMatArm2,
		int Wrist, const Matrix4x4DX& FrameBaseLocalMatWrist,
		const Vector3DX& DirPos, const Vector3DX& Diryvec, const Vector3DX& Dirzvec) noexcept {

		IK_move_Arm(
			Vector3DX::vget(0.f, 0.f, -1.f).normalized(),
			Vector3DX::vget(1.f, -1.f, 0.f).normalized(),
			pObj, 1.5f,
			Arm,
			FrameBaseLocalMatArm,
			Arm2,
			FrameBaseLocalMatArm2,
			Wrist,
			FrameBaseLocalMatWrist,
			DirPos, Diryvec, Dirzvec);
	}
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// カメラから画面上の座標を取得
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	static Vector3DX GetScreenPos(const Vector3DX& campos, const Vector3DX& camvec, const Vector3DX& camup, float fov, float near_t, float far_t, const Vector3DX& worldpos) noexcept;
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// ウィンドウアクティブチェック付きキー操作
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	static int CheckHitKeyWithCheck(int KeyCode) noexcept {
		if (GetWindowActiveFlag()) {
			return CheckHitKey(KeyCode);
		}
		else {
			return 0;
		}
	}
	static int GetMouseInputWithCheck(void) noexcept {
		if (GetWindowActiveFlag()) {
			return GetMouseInput();
		}
		else {
			return 0;
		}
	}
	static int GetMouseWheelRotVolWithCheck(int CounterReset = TRUE) noexcept {
		if (GetWindowActiveFlag()) {
			return GetMouseWheelRotVol(CounterReset);
		}
		else {
			GetMouseWheelRotVol(CounterReset);
			return 0;
		}
	}
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// 描画
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// 縁ぬき四角
	static void DrawBoxLine_2D(int p1x, int p1y, int p2x, int p2y, const unsigned int& color, int thickness = 1) noexcept {
		if (thickness == 1) {
			DxLib::DrawBox(p1x, p1y, p2x, p2y, color, FALSE);
		}
		else {
			DxLib::DrawLine(p1x, p1y, p1x, p2y, color, thickness);
			DxLib::DrawLine(p1x, p1y, p2x, p1y, color, thickness);
			DxLib::DrawLine(p1x, p2y, p2x, p2y, color, thickness);
			DxLib::DrawLine(p2x, p1y, p2x, p2y, color, thickness);
		}
	}
	// グラデーションのある矩形を描画
	static void DrawGradationBox_2D(int x1, int y1, int x2, int y2, COLOR_U8 color1, COLOR_U8 color2, const unsigned char UorL = 255) noexcept {
		VERTEX2D Vertex[6]{};
		// 左上の頂点の情報をセット
		Vertex[0].pos.x = static_cast<float>(x1);
		Vertex[0].pos.y = static_cast<float>(y1);
		Vertex[0].pos.z = 0.0f;
		Vertex[0].rhw = 1.0f;
		Vertex[0].dif = color1;
		Vertex[0].u = 0.0f;
		Vertex[0].v = 0.0f;

		// 右上の頂点の情報をセット
		Vertex[1].pos.x = static_cast<float>(x2);
		Vertex[1].pos.y = static_cast<float>(y1);
		Vertex[1].pos.z = 0.0f;
		Vertex[1].rhw = 1.0f;
		Vertex[1].dif = color2;
		Vertex[1].u = 0.0f;
		Vertex[1].v = 0.0f;

		Vertex[5] = Vertex[1];

		// 左下の頂点の情報をセット
		Vertex[2].pos.x = static_cast<float>(x1);
		Vertex[2].pos.y = static_cast<float>(y2);
		Vertex[2].pos.z = 0.0f;
		Vertex[2].rhw = 1.0f;
		Vertex[2].dif = color1;
		Vertex[2].u = 0.0f;
		Vertex[2].v = 0.0f;

		Vertex[4] = Vertex[2];

		// 右下の頂点の情報をセット
		Vertex[3].pos.x = static_cast<float>(x2);
		Vertex[3].pos.y = static_cast<float>(y2);
		Vertex[3].pos.z = 0.0f;
		Vertex[3].rhw = 1.0f;
		Vertex[3].u = 0.0f;
		Vertex[3].v = 0.0f;

		switch (UorL) {
		case 3:// 横(右)
			// 左上の頂点の情報をセット
			Vertex[0].dif = color1;
			// 右上の頂点の情報をセット
			Vertex[1].dif = color2;
			Vertex[5] = Vertex[1];
			// 左下の頂点の情報をセット
			Vertex[2].dif = color1;
			Vertex[4] = Vertex[2];
			// 右下の頂点の情報をセット
			Vertex[3].dif = color2;
			break;
		case 2:// 縦(下)
			// 左上の頂点の情報をセット
			Vertex[0].dif = color1;
			// 右上の頂点の情報をセット
			Vertex[1].dif = color1;
			Vertex[5] = Vertex[1];
			// 左下の頂点の情報をセット
			Vertex[2].dif = color2;
			Vertex[4] = Vertex[2];
			// 右下の頂点の情報をセット
			Vertex[3].dif = color2;
			break;
		case 1:// 横(左)
			// 左上の頂点の情報をセット
			Vertex[0].dif = color2;
			// 右上の頂点の情報をセット
			Vertex[1].dif = color1;
			Vertex[5] = Vertex[1];
			// 左下の頂点の情報をセット
			Vertex[2].dif = color2;
			Vertex[4] = Vertex[2];
			// 右下の頂点の情報をセット
			Vertex[3].dif = color1;
			break;
		case 0:// 縦(上)
			// 左上の頂点の情報をセット
			Vertex[0].dif = color2;
			// 右上の頂点の情報をセット
			Vertex[1].dif = color2;
			Vertex[5] = Vertex[1];
			// 左下の頂点の情報をセット
			Vertex[2].dif = color1;
			Vertex[4] = Vertex[2];
			// 右下の頂点の情報をセット
			Vertex[3].dif = color1;
			break;
		default:
			// 左上の頂点の情報をセット
			Vertex[0].dif = color1;
			// 右上の頂点の情報をセット
			Vertex[1].dif = color1;
			Vertex[5] = Vertex[1];
			// 左下の頂点の情報をセット
			Vertex[2].dif = color1;
			Vertex[4] = Vertex[2];
			// 右下の頂点の情報をセット
			Vertex[3].dif = color1;
			break;
		}
		// ポリゴンを2個描画
		DrawPolygon2D(Vertex, 2, DX_NONE_GRAPH, FALSE);
	}
	// 	x1,y1 Angleが0の場合の左上座標
	// 	x2,y2 Angleが0の場合の右下座標
	// 	xminp,yminp 左上角からの固定長さ
	// 	xmaxp,ymaxp 右下角からの固定長さ
	// 	XCenter,YCenter　　: 画像を回転描画する画像上の中心座標(左上を(0.0f,0.0f)、右下を(1.0f,1.0f)とした割合)
	// 	Angle　　　: 描画角度（ラジアン指定）
	// 	GrHandle　 : 描画するグラフィックの識別番号（グラフィックハンドル）
	// 	TransFlag　: 画像の透明度を有効にするかどうか( TRUE：有効にする　FALSE：無効にする )
	// 	TilingFlag : 角以外の部分をタイリングするか拡縮させるか( TRUE：タイリング　FALSE：拡縮 )
	static void Draw9SliceGraph(
		int x1, int y1, int x2, int y2,
		int xminp, int yminp, int xmaxp, int ymaxp,
		float XCenter, float YCenter, float Angle,
		int GrHandle, bool TransFlag, bool TilingFlag) noexcept {
		// 最低限のサイズを指定
		if (x2 < x1 + xminp + xmaxp) {
			x2 = x1 + xminp + xmaxp;
		}
		if (y2 < y1 + yminp + ymaxp) {
			y2 = y1 + yminp + ymaxp;
		}
		// 用意する頂点情報
		std::vector<VERTEX2D> Vertex;
		std::vector<unsigned short> Index;

		float xs = static_cast<float>(x2 - x1);
		float ys = static_cast<float>(y2 - y1);

		float CenterX = static_cast<float>(x1) + xs * XCenter;
		float CenterY = static_cast<float>(y1) + ys * YCenter;

		auto SetUpPoint = [&](float xper, float yper, int xc, int yc) {
			Vertex.resize(Vertex.size() + 1);
			Vertex.back().pos = VGet(
				static_cast<float>(x1) + xs * xper - CenterX,
				static_cast<float>(y1) + ys * yper - CenterY,
				0.f);

			Vertex.back().pos = VGet(
				CenterX + Vertex.back().pos.x * std::cos(Angle) - Vertex.back().pos.y * std::sin(Angle),
				CenterY + Vertex.back().pos.x * std::sin(Angle) + Vertex.back().pos.y * std::cos(Angle),
				0.f);

			Vertex.back().rhw = 1.0f;
			Vertex.back().dif = GetColorU8(255, 255, 255, 255);
			Vertex.back().u = static_cast<float>(xc) / 3.f;
			Vertex.back().v = static_cast<float>(yc) / 3.f;
			return (unsigned short)(Vertex.size() - 1);
			};
		auto SetUpBox = [&](float xmin, float ymin, float xmax, float ymax, int xc, int yc) {
			Index.emplace_back(SetUpPoint(xmin, ymin, xc, yc));// 左上の頂点の情報をセット
			auto RU = SetUpPoint(xmax, ymin, xc + 1, yc);
			auto LD = SetUpPoint(xmin, ymax, xc, yc + 1);
			Index.emplace_back(RU);// 右上の頂点の情報をセット
			Index.emplace_back(LD);// 左下の頂点の情報をセット
			Index.emplace_back(SetUpPoint(xmax, ymax, xc + 1, yc + 1));// 右下の頂点の情報をセット
			Index.emplace_back(LD);// 左下の頂点の情報をセット
			Index.emplace_back(RU);// 右上の頂点の情報をセット
			};

		float xminpt = static_cast<float>(xminp) / xs;
		float xmaxpt = static_cast<float>(xmaxp) / xs;
		float xmaxt = 1.f - xmaxpt;
		float xmidt = xmaxt - xminpt;

		float yminpt = static_cast<float>(yminp) / ys;
		float ymaxpt = static_cast<float>(ymaxp) / ys;
		float ymaxt = 1.f - ymaxpt;
		float ymidt = ymaxt - yminpt;

		int xtile = 1;
		int ytile = 1;
		// タイリング
		if (TilingFlag) {
			xtile = static_cast<int>(xmidt / ((xminpt + xmaxpt) / 2.f)) + 1;
			if (xtile <= 0) {
				xtile = 1;
			}
			ytile = static_cast<int>(ymidt / ((yminpt + ymaxpt) / 2.f)) + 1;
			if (ytile <= 0) {
				ytile = 1;
			}
		}

		Vertex.reserve(static_cast<size_t>(3 * 2 * ((xtile + 2) * (ytile + 2))));
		float xmin = 0.f;
		float xmax = xminpt;
		int xc = 0;
		for (int x : std::views::iota(0, xtile + 2)) {
			float ymin = 0.f;
			float ymax = yminpt;
			int yc = 0;
			for (int y : std::views::iota(0, ytile + 2)) {
				SetUpBox(xmin, ymin, xmax, ymax, xc, yc);
				// 次
				ymin = ymax;
				ymax = TilingFlag ? (ymin + ymidt / static_cast<float>(ytile)) : ymaxt;
				if (y == 0) {
					yc = 1;
				}
				if (y == ytile) {
					ymax = 1.f;
					yc = 2;
				}
			}
			// 次
			xmin = xmax;
			xmax = TilingFlag ? (xmin + xmidt / static_cast<float>(xtile)) : xmaxt;
			if (x == 0) {
				xc = 1;
			}
			if (x == xtile) {
				xmax = 1.f;
				xc = 2;
			}
		}
		DrawPolygonIndexed2D(Vertex.data(), static_cast<int>(Vertex.size()), Index.data(), static_cast<int>(Index.size()) / 3, GrHandle, TransFlag ? TRUE : FALSE);
	}
	// カプセル描画
	static bool DrawCapsule_3D(const Vector3DX& p1, const Vector3DX& p2, float range, const unsigned int& color, const unsigned int& speccolor) noexcept {
		return DxLib::DrawCapsule3D(p1.get(), p2.get(), range, 8, color, speccolor, TRUE) == TRUE;
	}
	// 球描画
	static bool DrawSphere_3D(const Vector3DX& p1, float range, const unsigned int& color, const unsigned int& speccolor) noexcept {
		return DxLib::DrawSphere3D(p1.get(), range, 8, color, speccolor, TRUE) == TRUE;
	}

	// 固定でA8R8G8B8を出す
	constexpr uint32_t GetColorFix(uint8_t Red, uint8_t Green, uint8_t Blue) {
		return 0xff000000 | (((uint32_t)Red) << 16) | (((uint32_t)Green) << 8) | ((uint32_t)Blue);
	}

	// カラー指定
	static constexpr unsigned int Red{ GetColorFix(255, 0, 0) };
	static constexpr unsigned int Red25{ GetColorFix(192, 0, 0) };
	static constexpr unsigned int Red50{ GetColorFix(128, 0, 0) };

	static constexpr unsigned int Green{ GetColorFix(0, 255, 0) };// GetColorFix(43, 255, 91)
	static constexpr unsigned int DarkGreen{ GetColorFix(0, 64, 0) };// GetColorFix(21, 128, 45)

	static constexpr unsigned int Blue{ GetColorFix(0, 0, 255) };

	static constexpr unsigned int Yellow{ GetColorFix(255, 255, 0) };

	static constexpr unsigned int WhiteSel{ GetColorFix(216, 255, 216) };

	static constexpr unsigned int White{ GetColorFix(255, 255, 255) };
	static constexpr unsigned int Gray05{ GetColorFix(255 * 95 / 100, 255 * 95 / 100, 255 * 95 / 100) };
	static constexpr unsigned int Gray10{ GetColorFix(255 * 90 / 100, 255 * 90 / 100, 255 * 90 / 100) };
	static constexpr unsigned int Gray15{ GetColorFix(255 * 85 / 100, 255 * 85 / 100, 255 * 85 / 100) };
	static constexpr unsigned int Gray20{ GetColorFix(255 * 80 / 100, 255 * 80 / 100, 255 * 80 / 100) };
	static constexpr unsigned int Gray25{ GetColorFix(255 * 75 / 100, 255 * 75 / 100, 255 * 75 / 100) };
	static constexpr unsigned int Gray30{ GetColorFix(255 * 70 / 100, 255 * 70 / 100, 255 * 70 / 100) };
	static constexpr unsigned int Gray35{ GetColorFix(255 * 65 / 100, 255 * 65 / 100, 255 * 65 / 100) };
	static constexpr unsigned int Gray40{ GetColorFix(255 * 60 / 100, 255 * 60 / 100, 255 * 60 / 100) };
	static constexpr unsigned int Gray45{ GetColorFix(255 * 55 / 100, 255 * 55 / 100, 255 * 55 / 100) };
	static constexpr unsigned int Gray50{ GetColorFix(255 * 50 / 100, 255 * 50 / 100, 255 * 50 / 100) };
	static constexpr unsigned int Gray55{ GetColorFix(255 * 45 / 100, 255 * 45 / 100, 255 * 45 / 100) };
	static constexpr unsigned int Gray60{ GetColorFix(255 * 40 / 100, 255 * 40 / 100, 255 * 40 / 100) };
	static constexpr unsigned int Gray65{ GetColorFix(255 * 35 / 100, 255 * 35 / 100, 255 * 35 / 100) };
	static constexpr unsigned int Gray70{ GetColorFix(255 * 30 / 100, 255 * 30 / 100, 255 * 30 / 100) };
	static constexpr unsigned int Gray75{ GetColorFix(255 * 25 / 100, 255 * 25 / 100, 255 * 25 / 100) };
	static constexpr unsigned int Gray80{ GetColorFix(255 * 20 / 100, 255 * 20 / 100, 255 * 20 / 100) };
	static constexpr unsigned int Gray85{ GetColorFix(255 * 15 / 100, 255 * 15 / 100, 255 * 15 / 100) };
	static constexpr unsigned int Gray90{ GetColorFix(255 * 10 / 100, 255 * 10 / 100, 255 * 10 / 100) };
	static constexpr unsigned int Gray95{ GetColorFix(255 *  5 / 100, 255 *  5 / 100, 255 *  5 / 100) };
	static constexpr unsigned int Black{ GetColorFix(0, 0, 0) };

	namespace WindowSystem {
		// 
		class Rect2D {
			int			m_PosX{ 0 };
			int			m_PosY{ 0 };
			int			m_SizeX{ 0 };
			int			m_SizeY{ 0 };
		public:
			const auto& GetPosX() const noexcept { return this->m_PosX; }
			const auto& GetPosY() const noexcept { return this->m_PosY; }
			void			Set(int posx, int posy, int sizex, int sizey) noexcept {
				m_PosX = posx;
				m_PosY = posy;
				m_SizeX = sizex;
				m_SizeY = sizey;
			}
		public:
			bool			IsHit(const Rect2D& target) const noexcept {
				return (
					((this->m_PosX >= target.m_PosX && this->m_PosX < (target.m_PosX + target.m_SizeX)) || (target.m_PosX > this->m_PosX && target.m_PosX <= (this->m_PosX + this->m_SizeX))) &&
					((this->m_PosY >= target.m_PosY && this->m_PosY < (target.m_PosY + target.m_SizeY)) || (target.m_PosY > this->m_PosY && target.m_PosY <= (this->m_PosY + this->m_SizeY)))
					);
			}
		};
		// 
		enum class DrawType : int {
			Alpha,
			Add,
			Bright,
			Box,
			Quadrangle,
			Circle,
			Line,
			String,
			StringAutoFit,
			RotaGraph,
			ExtendGraph,
			CircleGauge,
		};
		class DrawData {
			DrawType								m_type{ DrawType::Box };
			std::array<int, 8>						m_intParam{};
			std::array<unsigned int, 2>				m_UintParam{};
			std::array<float, 6>					m_floatParam{};
			std::array<bool, 1>						m_boolParam{};
			std::array<const GraphHandle*, 1>		m_GraphHandleParam{};
			std::string								m_string;
		public:
			void InputType(DrawType type) noexcept { this->m_type = type; }
			void InputintParam(int ID, int param) noexcept { this->m_intParam.at(ID) = param; }
			void InputUintParam(int ID, unsigned int param) noexcept { this->m_UintParam.at(ID) = param; }
			void InputfloatParam(int ID, float param) noexcept { this->m_floatParam.at(ID) = param; }
			void InputboolParam(int ID, bool param) noexcept { this->m_boolParam.at(ID) = param; }
			void InputGraphHandleParam(int ID, const GraphHandle* param) noexcept { this->m_GraphHandleParam.at(ID) = param; }
			void InputStringParam(std::string_view param) noexcept { this->m_string = param; }
		public:
			void Output() const noexcept;
		public:
			void		operator=(const DrawData& tgt) noexcept {
				this->m_type = tgt.m_type;
				this->m_intParam = tgt.m_intParam;
				this->m_UintParam = tgt.m_UintParam;
				this->m_floatParam = tgt.m_floatParam;
				this->m_boolParam = tgt.m_boolParam;
				this->m_GraphHandleParam = tgt.m_GraphHandleParam;
				this->m_string = tgt.m_string;
			}
			bool		operator==(const DrawData& tgt) const noexcept {
				return (
					this->m_type == tgt.m_type &&
					this->m_intParam == tgt.m_intParam &&
					this->m_UintParam == tgt.m_UintParam &&
					this->m_floatParam == tgt.m_floatParam &&
					this->m_boolParam == tgt.m_boolParam &&
					this->m_GraphHandleParam == tgt.m_GraphHandleParam &&
					this->m_string == tgt.m_string
					);
			}
		};
		// 
		enum class DrawLayer : int {
			BackGround,
			Back,
			Back2,
			Back3,
			Back4,
			Normal,
			Front,

			Max,
		};
		// 文字
		template <typename... Args>
		extern int GetMsgLen(int ySize, std::string_view String, Args&&... args) noexcept {
			return FontPool::Instance()->Get(FontPool::FontType::MS_Gothic, ySize, 3)->GetStringWidth(InvalidID, ((std::string)String).c_str(), args...);
		}
		// 
		class DrawControl : public SingletonBase<DrawControl> {
		private:
			friend class SingletonBase<DrawControl>;
		private:
			std::vector<std::vector<DrawData>>	m_DrawDatas;
			std::vector<std::vector<DrawData>>	m_PrevDrawDatas;

			GraphHandle				m_BufferScreen;
		private:
			DrawControl(void) noexcept;
			~DrawControl(void) noexcept {
				for (auto& d : this->m_DrawDatas) {
					d.clear();
				}
				this->m_DrawDatas.clear();

				for (auto& d : this->m_PrevDrawDatas) {
					d.clear();
				}
				this->m_PrevDrawDatas.clear();

			}

			DrawData* GetBack(DrawLayer Layer) noexcept {
				this->m_DrawDatas.at(static_cast<int>(Layer)).resize(this->m_DrawDatas.at(static_cast<int>(Layer)).size() + 1);
				return &this->m_DrawDatas.at(static_cast<int>(Layer)).back();
			}
		public:
			bool	IsDrawOnWindow(int x1, int y1, int x2, int y2) noexcept;
			// 
			void	SetAlpha(DrawLayer Layer, int Alpha) noexcept {
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::Alpha);
				Back->InputintParam(0, Alpha);
			}
			// 
			void	SetAdd(DrawLayer Layer, int Add) noexcept {
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::Add);
				Back->InputintParam(0, Add);
			}
			// 
			void	SetBright(DrawLayer Layer, int valueR, int valueG, int valueB) noexcept {
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::Bright);
				Back->InputintParam(0, valueR);
				Back->InputintParam(1, valueG);
				Back->InputintParam(2, valueB);
			}
			// 
			void	SetDrawBox(DrawLayer Layer, int x1, int y1, int x2, int y2, unsigned int color1, bool IsFill) noexcept {
				if (!IsDrawOnWindow(x1, y1, x2, y2)) { return; }				// 画面外は表示しない
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::Box);
				Back->InputintParam(0, x1);
				Back->InputintParam(1, y1);
				Back->InputintParam(2, x2);
				Back->InputintParam(3, y2);
				Back->InputUintParam(0, color1);
				Back->InputboolParam(0, IsFill);
			}
			// 
			void	SetDrawQuadrangle(DrawLayer Layer, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, unsigned int color1, bool IsFill) noexcept {
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::Quadrangle);
				Back->InputintParam(0, x1);
				Back->InputintParam(1, y1);
				Back->InputintParam(2, x2);
				Back->InputintParam(3, y2);
				Back->InputintParam(4, x3);
				Back->InputintParam(5, y3);
				Back->InputintParam(6, x4);
				Back->InputintParam(7, y4);
				Back->InputUintParam(0, color1);
				Back->InputboolParam(0, IsFill);
			}
			// 
			void	SetDrawCircle(DrawLayer Layer, int x1, int y1, int radius, unsigned int color1, bool IsFill = true, int LineThickness = 1) {
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::Circle);
				Back->InputintParam(0, x1);
				Back->InputintParam(1, y1);
				Back->InputintParam(2, radius);
				Back->InputUintParam(0, color1);
				Back->InputboolParam(0, IsFill);
				Back->InputintParam(3, LineThickness);
			}
			// 
			void	SetDrawLine(DrawLayer Layer, int x1, int y1, int x2, int y2, unsigned int color1, int   Thickness = 1) noexcept {
				if (!IsDrawOnWindow(x1, y1, x2, y2)) { return; }				// 画面外は表示しない
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::Line);
				Back->InputintParam(0, x1);
				Back->InputintParam(1, y1);
				Back->InputintParam(2, x2);
				Back->InputintParam(3, y2);
				Back->InputUintParam(0, color1);
				Back->InputintParam(4, Thickness);
			}
			// 
			void SetDrawRotaGraph(DrawLayer Layer, const GraphHandle* pGraphHandle, int posx, int posy, float Exrate, float rad, bool trns) noexcept {
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::RotaGraph);
				Back->InputGraphHandleParam(0, pGraphHandle);
				Back->InputintParam(0, posx);
				Back->InputintParam(1, posy);
				Back->InputfloatParam(0, Exrate);
				Back->InputfloatParam(1, rad);
				Back->InputboolParam(0, trns);
			}
			// 
			void SetDrawExtendGraph(DrawLayer Layer, const GraphHandle* pGraphHandle, int x1, int y1, int x2, int y2, bool trns) noexcept {
				if (!IsDrawOnWindow(x1, y1, x2, y2)) { return; }				// 画面外は表示しない
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::ExtendGraph);
				Back->InputGraphHandleParam(0, pGraphHandle);
				Back->InputintParam(0, x1);
				Back->InputintParam(1, y1);
				Back->InputintParam(2, x2);
				Back->InputintParam(3, y2);
				Back->InputboolParam(0, trns);
			}
			// 
			void SetDrawCircleGauge(DrawLayer Layer, const GraphHandle* pGraphHandle, int   CenterX, int   CenterY, float Percent, float StartPercent = 0.f, float Scale = 1.0f) noexcept {
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::CircleGauge);
				Back->InputGraphHandleParam(0, pGraphHandle);
				Back->InputintParam(0, CenterX);
				Back->InputintParam(1, CenterY);
				Back->InputfloatParam(0, Percent);
				Back->InputfloatParam(1, StartPercent);
				Back->InputfloatParam(2, Scale);
			}
			// 
			template <typename... Args>
			void	SetString(DrawLayer Layer, FontPool::FontType type, int fontSize, FontHandle::FontXCenter FontX, FontHandle::FontYCenter FontY, int x, int y, unsigned int Color, unsigned int EdgeColor, const std::string& Str, Args&&... args) noexcept {
				if (Str == "") { return; }
				int xSize = WindowSystem::GetMsgLen(fontSize, Str.c_str(), args...);
				switch (FontX) {
				case FontHandle::FontXCenter::LEFT:
					if (!IsDrawOnWindow((x), (y - fontSize), (x + xSize), (y + fontSize))) { return; }				// 画面外は表示しない
					break;
				case FontHandle::FontXCenter::MIDDLE:
					if (!IsDrawOnWindow((x - xSize / 2), (y - fontSize), (x + xSize / 2), (y + fontSize))) { return; }				// 画面外は表示しない
					break;
				case FontHandle::FontXCenter::RIGHT:
					if (!IsDrawOnWindow((x - xSize), (y - fontSize), (x), (y + fontSize))) { return; }				// 画面外は表示しない
					break;
				default:
					break;
				}

				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::String);

				Back->InputintParam(0, static_cast<int>(type));
				Back->InputintParam(1, fontSize);
				Back->InputintParam(2, static_cast<int>(FontX));
				Back->InputintParam(3, static_cast<int>(FontY));
				Back->InputintParam(4, x);
				Back->InputintParam(5, y);
				Back->InputUintParam(0, Color);
				Back->InputUintParam(1, EdgeColor);

				char ptr[1024] = "";
				snprintfDx(ptr, 1024, Str.c_str(), args...);
				Back->InputStringParam(ptr);
			}
			// 
			const auto	SetStringAutoFit(DrawLayer Layer, FontPool::FontType type, int fontSize, int x1, int y1, int x2, int y2, unsigned int Color, unsigned int EdgeColor, const std::string& Str) noexcept {
				if (Str == "") { return 0.f; }
				DrawData* Back = GetBack(Layer);
				Back->InputType(DrawType::StringAutoFit);

				Back->InputintParam(0, static_cast<int>(type));
				Back->InputintParam(1, fontSize);
				Back->InputintParam(2, x1);
				Back->InputintParam(3, y1);
				Back->InputintParam(4, x2);
				Back->InputintParam(5, y2);
				Back->InputUintParam(0, Color);
				Back->InputUintParam(1, EdgeColor);

				Back->InputStringParam(Str);

				return FontPool::Instance()->Get((FontPool::FontType)type, fontSize, 3)->DrawStringAutoFit(
					x1 + BaseScreenWidth, y1 + BaseScreenHeight,
					x2 + BaseScreenWidth, y2 + BaseScreenHeight,
					Color,
					EdgeColor,
					Str
				);
			}
			// 
		public:
			void	ClearList(void) noexcept {
				for (size_t index = 0; auto & d : this->m_DrawDatas) {
					auto& pd = this->m_PrevDrawDatas.at(index);
					pd.clear();
					for (auto& d2 : d) {
						pd.resize(pd.size() + 1);
						pd.back() = d2;
					}
					index++;
				}
				for (auto& d : this->m_DrawDatas) {
					d.clear();
				}
			}
			void	Draw(void) noexcept {
				auto* DrawCtrls = WindowSystem::DrawControl::Instance();
				bool IsHit = false;
				// 同じかどうかチェック
				for (size_t index = 0; auto & d : this->m_DrawDatas) {
					auto& pd = this->m_PrevDrawDatas.at(index);
					if (pd.size() == d.size()) {
						for (size_t index2 = 0; auto & d2 : d) {
							auto& pd2 = pd.at(index2);
							if (!(pd2 == d2)) {
								IsHit = true;
								break;
							}
							index2++;
						}
					}
					else {
						IsHit = true;
					}
					index++;
				}
				// 
				if (IsHit) {
					{
						auto NowScreen = GetDrawScreen();
						m_BufferScreen.SetDraw_Screen(true);
						{
							for (auto& d : this->m_DrawDatas) {
								for (auto& d2 : d) {
									d2.Output();
								}
								DrawCtrls->SetAlpha(WindowSystem::DrawLayer::Normal, 255);
								SetDrawBright(255, 255, 255);
							}
						}
						GraphHandle::SetDraw_Screen(NowScreen, false);
					}
				}
				// 前に描画したものをそのまま出す
				m_BufferScreen.DrawGraph(0, 0, true);
			}
		};
		// 箱
		extern void SetBox(int xp1, int yp1, int xp2, int yp2, unsigned int colorSet) noexcept;
		// マウスでクリックできるボタン
		extern bool SetClickBox(int xp1, int yp1, int xp2, int yp2, unsigned int colorSet, bool IsRepeat, bool IsActive) noexcept;
		// 文字
		bool GetMsgPosOn(int* xp1, int* yp1, int ySize, int xSize, FontHandle::FontXCenter FontX) noexcept;
		// 文字の長さを取得
		template <typename... Args>
		extern void SetMsg(int xp1, int yp1, int ySize, FontHandle::FontXCenter FontX, unsigned int Color, unsigned int EdleColor, std::string_view String, Args&&... args) noexcept {
			if (String == "") { return; }
			//if (!GetMsgPosOn(&xp1, &yp1, ySize, GetMsgLen(ySize, String, args...), FontX)) { return; }
			DrawControl::Instance()->SetString(DrawLayer::Normal, FontPool::FontType::MS_Gothic, ySize,
				FontX, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, EdleColor, ((std::string)String).c_str(), args...);
		}
		// クリックできる文字付のボックス
		template <typename... Args>
		extern bool SetMsgClickBox(int xp1, int yp1, int xp2, int yp2, int StringYSizeMax, unsigned int defaultcolor, bool IsRepeat, bool IsActive, std::string_view String, Args&&... args) noexcept {
			bool ret = SetClickBox(xp1, yp1, xp2, yp2, defaultcolor, IsRepeat, IsActive);
			SetMsg((xp1 + xp2) / 2, (yp1 + yp2) / 2, std::min(StringYSizeMax, yp2 - yp1), FontHandle::FontXCenter::MIDDLE, White, Black, String, args...);
			return ret;
		}
		// 文字付のボックス
		template <typename... Args>
		extern void SetMsgBox(int xp1, int yp1, int xp2, int yp2, int StringYSizeMax, unsigned int defaultcolor, std::string_view String, Args&&... args) noexcept {
			SetBox(xp1, yp1, xp2, yp2, defaultcolor);
			SetMsg((xp1 + xp2) / 2, (yp1 + yp2) / 2, std::min(StringYSizeMax, yp2 - yp1), FontHandle::FontXCenter::MIDDLE, White, Black, String, args...);
		}
		// オンオフできるボタン
		extern bool CheckBox(int xp1, int yp1, bool switchturn) noexcept;
		// スライダー
		extern int UpDownBar(int xmin, int xmax, int yp, int value, int valueMin, int valueMax) noexcept;
		// 0~valueMaxの選択制ボタン集
		extern int UpDownBox(int xmin, int xmax, int yp, int value, int valueMax) noexcept;
		// 
		/*
		class ScrollBoxClass {
			bool		m_IsChangeScrollY{ false }
			int			m_BaseScrollY{ 0 }
			float		m_NowScrollYPer{ 0.f }
		public:
			const auto&		GetNowScrollYPer(void) const noexcept { return this->m_NowScrollYPer; }
			void			ScrollBox(int xp1, int yp1, int xp2, int yp2, float TotalPer, bool IsActive) noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto* Pad = PadControl::Instance();
				unsigned int color = Gray25;

				int length = static_cast<int>(static_cast<float>(yp2 - yp1) / TotalPer);
				float Total = static_cast<float>(yp2 - yp1 - length);
				int Yp_t = static_cast<int>(Total * this->m_NowScrollYPer);
				int Yp_s = std::max(yp1, yp1 + Yp_t);
				int Yp_e = std::min(yp2, Yp_s + length);

				if (IsActive)
		{
					if (IntoMouse(xp1, yp1, xp2, yp2))
		{
						if (Pad->GetWheelAdd() != 0.f)
		{
							m_NowScrollYPer = std::clamp(m_NowScrollYPer + static_cast<float>(-Pad->GetWheelAdd() * 3) / Total, 0.f, 1.f);
						}
					}
					if (IntoMouse(xp2 - DrawParts->GetUIY(24), yp1, xp2, yp2))
		{
						if (Pad->GetINTERACTKey().trigger())
		{
							m_IsChangeScrollY = true;
						}

						if (!m_IsChangeScrollY)
		{
							HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
							SetCursor(hCursor);
						}
					}
					if (m_IsChangeScrollY)
		{
						if (Pad->GetINTERACTKey().press())
		{
							color = White;
							m_NowScrollYPer = std::clamp(static_cast<float>(Pad->GetMouseY() - this->m_BaseScrollY) / Total, 0.f, 1.f);

							HCURSOR hCursor = LoadCursor(NULL, IDC_SIZENS);
							SetCursor(hCursor);
						}
						else {
							m_IsChangeScrollY = false;
						}
					}
					else {
						m_BaseScrollY = Pad->GetMouseY() - Yp_t;
						if (Pad->GetMouseY() < Yp_s)
		{
							m_BaseScrollY += Yp_s - Pad->GetMouseY();
						}

						if (Pad->GetMouseY() > Yp_e)
		{
							m_BaseScrollY += Yp_e - Pad->GetMouseY();
						}
					}
				}
				SetBox(xp2 - DrawParts->GetUIY(24), yp1, xp2, yp2, Gray50);
				SetBox(xp2 - DrawParts->GetUIY(24) + DrawParts->GetUIY(1), Yp_s, xp2 - DrawParts->GetUIY(1), Yp_e, color);
			}
		}
		//*/
	}

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*クラス																																	*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// 文字列から数値を取り出す
	class getparams {
		static const int charLength = 512;
	public:
		// 文字列から=より右の値取得
		static std::string getleft(std::string tempname) noexcept { return tempname.substr(0, tempname.find('=')); }
		// 文字列から=より右の値取得
		static std::string getright(std::string tempname) noexcept { return tempname.substr(tempname.find('=') + 1); }
	public:
		// 左辺の情報もとる
		static std::string		getcmd(int p1, int* p2) noexcept {
			char mstr[charLength]; /*tank*/
			FileRead_gets(mstr, charLength, p1);
			*p2 = std::stoi(getright(mstr));
			return getleft(mstr);
		}
		// 1行そのまま取得
		static auto						Getstr(int p1) noexcept {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return std::string(mstr);
		}
		// =より右の値取得
		static std::string		_str(int p1) noexcept {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return getright(mstr);
		}
		static int			_int(int p1) noexcept {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return std::stoi(getright(mstr));
		}
		static long int			_long(int p1) noexcept {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return std::stol(getright(mstr));
		}
		static unsigned long int	_ulong(int p2) noexcept {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p2);
			return std::stoul(getright(mstr));
		}
		static float				_float(int p1) noexcept {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return std::stof(getright(mstr));
		}
		static bool				_bool(int p1) noexcept {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return (getright(mstr).find("true") != std::string::npos);
		}
	};
	// モデルのフレーム情報保持
	class frames {
		int			m_FrameID{ InvalidID };
		Matrix4x4DX	m_WorldPos;
		Matrix4x4DX	m_LocalPos;
	public:
		void			operator=(const frames& tgt) noexcept {
			this->m_FrameID = tgt.m_FrameID;
			this->m_WorldPos = tgt.m_WorldPos;
			this->m_LocalPos = tgt.m_LocalPos;
		}
		void			Set(int i, const MV1& obj) noexcept {
			m_FrameID = i;
			m_WorldPos = obj.GetFrameLocalWorldMatrix(i);
			if (obj.GetFrameParent(i) >= 0) {
				m_LocalPos = obj.GetFrameLocalMatrix(i);
			}
			else {
				m_LocalPos = obj.GetFrameLocalWorldMatrix(i);// 
			}
		}
		const auto& GetFrameID(void) const noexcept { return m_FrameID; }
		const auto& GetFrameWorldPosition(void) const noexcept { return m_WorldPos; }
		const auto& GetFrameLocalPosition(void) const noexcept { return m_LocalPos; }
	};

	// 位置情報
	class moves {
		Vector3DX pos;		// 反映用座標
		Vector3DX posbuf;	// 演算用座標
		Vector3DX repos;	// 前フレームの座標
		Vector3DX vec;		// 加速
		Matrix3x3DX mat;	// 回転
		Matrix3x3DX matbuf;	// 回転
	public:
		const auto& GetPos(void) const noexcept { return pos; }
		const auto& GetRePos(void) const noexcept { return repos; }
		const auto& GetVec(void) const noexcept { return vec; }
		const auto& GetMat(void) const noexcept { return mat; }

		const auto& GetPosBuf(void) const noexcept { return posbuf; }// 演算以外では使うな
		const auto& GetMatBuf(void) const noexcept { return matbuf; }// 演算以外では使うな
	public:
		void			SetPos(const Vector3DX& tgt) noexcept { this->posbuf = tgt; }
		void			SetVec(const Vector3DX& tgt) noexcept { this->vec = tgt; }
		void			SetMat(const Matrix3x3DX& tgt) noexcept { this->matbuf = tgt; }
	public:
		moves(void) noexcept {}
		moves(const moves& tgt) noexcept { *this = tgt; }
		moves(moves&& tgt) noexcept { *this = tgt; }
		// moves& operator=(const moves&) = delete;
		// moves& operator=(moves&& o) = delete;
		~moves(void) noexcept {}

		void			operator=(const moves& tgt) noexcept {
			this->pos = tgt.pos;
			this->posbuf = tgt.posbuf;
			this->repos = tgt.repos;
			this->vec = tgt.vec;
			this->mat = tgt.mat;
			this->matbuf = tgt.matbuf;
		}
	public:
		auto LerpMove(const moves& o, float Per) const noexcept {
			moves tmp;
			tmp.pos = Lerp(this->pos, o.pos, Per);
			tmp.posbuf = Lerp(this->posbuf, o.posbuf, Per);
			tmp.repos = Lerp(this->repos, o.repos, Per);
			tmp.vec = Lerp(this->vec, o.vec, Per);
			tmp.mat = Lerp(this->mat, o.mat, Per);
			tmp.matbuf = Lerp(this->matbuf, o.matbuf, Per);
			return tmp;
		}
	public:
		void			Init(const Vector3DX& Pos_t, const Vector3DX& Vec_t, const Matrix3x3DX& Mat_t) noexcept {
			this->pos = Pos_t;
			this->posbuf = Pos_t;
			this->repos = Pos_t;
			this->vec = Vec_t;
			this->mat = Mat_t;
			this->matbuf = Mat_t;
		}
		void			Update(float posper, float matper) noexcept {
			Easing(&this->pos, this->posbuf, posper, EasingType::OutExpo);
			this->repos = this->posbuf;
			Easing(&this->mat, this->matbuf, matper, EasingType::OutExpo);
		}
		void			SetAll(const Vector3DX& Pos_t, const Vector3DX& PosBuf_t, const Vector3DX& RePos_t, const Vector3DX& Vec_t, const Matrix3x3DX& Mat_t, const Matrix3x3DX& MatBuf_t) noexcept {
			this->pos = Pos_t;
			this->posbuf = PosBuf_t;
			this->repos = RePos_t;
			this->vec = Vec_t;
			this->mat = Mat_t;
			this->matbuf = MatBuf_t;
		}
	};
	// キー押し判定
	class switchs {
		bool		m_on{ false };// オンオフ判定
		bool		m_press{ false };// オンオフ判定
		bool		m_repeat{ false };// オンオフ判定
		int8_t		m_presscount{ 0 };// プッシュ判定
		float		m_repeatcount{ 0.5f };// プッシュ判定
	public:
		switchs(void) noexcept {
			Set(false);
			m_presscount = 0;
			m_repeatcount = 0.5f;
			m_press = false;
		}
		~switchs(void) noexcept {}
		// 使用前の用意
		void			Set(bool on) noexcept { m_on = on; }
		// 更新
		void			Execute(bool key) noexcept;
		// オンオフの取得
		bool			on(void) const noexcept { return m_on; }
		// 押した瞬間
		bool			trigger(void) const noexcept { return m_press && (m_presscount == 1); }
		// 押している間
		bool			press(void) const noexcept { return m_press; }
		// 押している間(30F+2F毎)
		bool			repeat(void) const noexcept { return m_repeat; }
		// 離した瞬間
		bool			release_trigger(void) const noexcept { return (!m_press) && (m_presscount == 1); }
		// 離している間
		bool			release(void) const noexcept { return !m_press; }
	};
	// 2次元振り子演算
	class Pendulum2D {
		float	m_PendulumLength = 10.f;
		float	m_PendulumMass = 2.f;
		float	m_drag_coeff = 2.02f;

		float	m_rad = deg2rad(12.f);
		float	m_vel = 0.f;
	public:
		void Init(float Length, float N, float rad) noexcept {
			m_PendulumLength = Length;
			m_PendulumMass = N;
			m_rad = rad;
			m_vel = 0.f;
		}
		void Update(void) noexcept;
	public:
		auto GetRad(void) const noexcept { return this->m_rad; }
		void AddRad(float value) noexcept { this->m_rad += value; }
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*シェーダー																																*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// シェーダーを使用する際の補助クラス
	class ShaderUseClass {
	public:
		// 2Dにシェーダーを適用する際に使用する画面サイズの頂点情報
		class ScreenVertex {
			VERTEX3DSHADER Screen_vertex[6] = {};
		public:
			// 頂点データの取得
			const auto* GetScreenVertex(void) noexcept { return Screen_vertex; }
			// 頂点データの準備
			void			SetScreenVertex(int dispx, int dispy) noexcept {
				int xp1 = 0;
				int yp1 = dispy;
				int xp2 = dispx;
				int yp2 = 0;
				Screen_vertex[0].pos = VGet(static_cast<float>(xp1), static_cast<float>(yp1), 0.0f);
				Screen_vertex[1].pos = VGet(static_cast<float>(xp2), static_cast<float>(yp1), 0.0f);
				Screen_vertex[2].pos = VGet(static_cast<float>(xp1), static_cast<float>(yp2), 0.0f);
				Screen_vertex[3].pos = VGet(static_cast<float>(xp2), static_cast<float>(yp2), 0.0f);
				Screen_vertex[0].dif = GetColorU8(255, 255, 255, 255);
				Screen_vertex[1].dif = GetColorU8(255, 255, 255, 255);
				Screen_vertex[2].dif = GetColorU8(255, 255, 255, 255);
				Screen_vertex[3].dif = GetColorU8(255, 255, 255, 255);
				Screen_vertex[0].u = 0.0f; Screen_vertex[0].v = 0.0f;
				Screen_vertex[1].u = 1.0f; Screen_vertex[1].v = 0.0f;
				Screen_vertex[2].u = 0.0f; Screen_vertex[3].v = 1.0f;
				Screen_vertex[3].u = 1.0f; Screen_vertex[2].v = 1.0f;
				Screen_vertex[4] = Screen_vertex[2];
				Screen_vertex[5] = Screen_vertex[1];
			}
		};

		// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列
		struct LIGHTCAMERA_MATRIX {
			MATRIX ViewMatrix;
			MATRIX ProjectionMatrix;
		};
	private:
		// DXLIBから引っ張ってきたシェーダー用の定義
		typedef float DX_D3D11_SHADER_FLOAT2[2];
		typedef float DX_D3D11_SHADER_FLOAT4[4];

		struct DX_D3D11_GS_CONST_BUFFER_BASE {
			DX_D3D11_SHADER_FLOAT4		ProjectionMatrix[4]{};											// ビュー　→　プロジェクション行列
			DX_D3D11_SHADER_FLOAT4		ViewMatrix[3]{};												// ワールド　→　ビュー行列
		};
		struct Wave {
			DX_D3D11_SHADER_FLOAT2 dir{};
			float amplitude{};
			float waveLength{};
		};
		struct ImmutableCB {
			Wave waves[20]{};
		};
	private:
		// シェーダーハンドル
		int m_VertexShaderhandle{ InvalidID };
		int m_GeometryShaderhandle{ InvalidID };
		int m_PixelShaderhandle{ InvalidID };
		// シェーダーに渡す追加パラメーターを配するハンドル
		std::array<int, 4> LightCameraMatrixConstantBufferHandle{};	// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列を設定するための定数バッファ
		std::array<int, 4> m_VertexShadercbhandle{};
		int m_GeometryShaderMatcbhandle{ InvalidID };
		int m_PixelShaderSendDispSizeHandle{ InvalidID };
		std::array<int, 4> m_PixelShadercbhandle{};
		ImmutableCB WaveData{};
		int m_VertexShadercbWaveDataHandle{ InvalidID };
	public:
		ShaderUseClass(void) noexcept {
			// シェーダーハンドル
			m_VertexShaderhandle = InvalidID;
			m_GeometryShaderhandle = InvalidID;
			m_PixelShaderhandle = InvalidID;
			// シェーダーに渡す追加パラメーターを配するハンドル
			for (auto& h : m_VertexShadercbhandle) {
				h = InvalidID;
			}
			m_GeometryShaderMatcbhandle = InvalidID;
			m_PixelShaderSendDispSizeHandle = InvalidID;
			for (auto& h : m_PixelShadercbhandle) {
				h = InvalidID;
			}
		}
		~ShaderUseClass(void) noexcept {
			Dispose();
		}
	public:
		// 初期化
		void			Init(const char* VertexShader, const char* PixelShader) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// 頂点シェーダー周り
			for (auto& h : m_VertexShadercbhandle) {
				h = CreateShaderConstantBuffer(sizeof(float) * 4);
			}
			m_VertexShadercbWaveDataHandle = CreateShaderConstantBuffer(sizeof(ImmutableCB));
			// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列を設定するための定数バッファの作成
			for (auto& h : LightCameraMatrixConstantBufferHandle) {
				h = CreateShaderConstantBuffer(sizeof(LIGHTCAMERA_MATRIX));
			}
			this->m_VertexShaderhandle = LoadVertexShader(VertexShader);		// 頂点シェーダーバイナリコードの読み込み
			// ピクセルシェーダ―周り
			this->m_PixelShaderSendDispSizeHandle = CreateShaderConstantBuffer(sizeof(float) * 4);
			for (auto& h : m_PixelShadercbhandle) {
				h = CreateShaderConstantBuffer(sizeof(float) * 4);
			}
			this->m_PixelShaderhandle = LoadPixelShader(PixelShader);			// ピクセルシェーダーバイナリコードの読み込み
		}
		void			AddGeometryShader(const char* GeometryShader) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			this->m_GeometryShaderMatcbhandle = CreateShaderConstantBuffer(sizeof(DX_D3D11_GS_CONST_BUFFER_BASE));
			this->m_GeometryShaderhandle = LoadGeometryShader(GeometryShader);
		}
		// 後始末
		void			Dispose(void) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// 頂点シェーダー周り
			for (auto& h : LightCameraMatrixConstantBufferHandle) {
				DeleteShaderConstantBuffer(h);
			}
			for (auto& h : m_VertexShadercbhandle) {
				DeleteShaderConstantBuffer(h);
			}
			DeleteShaderConstantBuffer(m_VertexShadercbWaveDataHandle);
			DeleteShader(this->m_VertexShaderhandle);
			// 
			DeleteShaderConstantBuffer(this->m_GeometryShaderMatcbhandle);
			DeleteShader(this->m_GeometryShaderhandle);
			// ピクセルシェーダ―周り
			DeleteShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);
			for (auto& h : m_PixelShadercbhandle) {
				DeleteShaderConstantBuffer(h);
			}
			DeleteShader(this->m_PixelShaderhandle);
		}
	public:
		// 頂点シェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetVertexCameraMatrix(int Slot, const Matrix4x4DX& View, const Matrix4x4DX& Projection) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// 設定したカメラのビュー行列と射影行列を取得しておく
			LIGHTCAMERA_MATRIX* LightCameraMatrixConst = (LIGHTCAMERA_MATRIX*)GetBufferShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(static_cast<size_t>(Slot - 4)));
			LightCameraMatrixConst->ViewMatrix = View.get();
			LightCameraMatrixConst->ProjectionMatrix = Projection.get();

			UpdateShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(static_cast<size_t>(Slot - 4)));
			SetShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(static_cast<size_t>(Slot - 4)), DX_SHADERTYPE_VERTEX, Slot);		// 影用深度記録画像を描画したときのカメラのビュー行列と射影行列を定数に設定する
		}
		// 頂点シェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetVertexParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(this->m_VertexShadercbhandle[0]);		// 頂点シェーダー用の定数バッファのアドレスを取得
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(this->m_VertexShadercbhandle[0]);								// 頂点シェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(this->m_VertexShadercbhandle[0], DX_SHADERTYPE_VERTEX, Slot);		// 頂点シェーダーの定数バッファを定数バッファレジスタ４にセット
		}
		// 
		void CalcGWave() {
			for (int i : std::views::iota(0, 20)) {
				Wave& w = WaveData.waves[i];
				float randomRad = static_cast<float>(GetRand(30) * DX_PI_F * 2 * 0.3f);
				w.dir[0] = sinf(randomRad);
				w.dir[1] = cosf(randomRad);
				w.amplitude = (0.03f + powf(2.0f, static_cast<float>(GetRand(3)) * 2.0f) * 0.05f) * 0.05f * Scale3DRate;
				w.waveLength = 1.0f + powf(2.f, 1.f + static_cast<float>(GetRand(3))) * 10.f;
			}
		}
		// 頂点シェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetVertexWave(void) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			ImmutableCB* f4 = (ImmutableCB*)GetBufferShaderConstantBuffer(this->m_VertexShadercbWaveDataHandle);		// 頂点シェーダー用の定数バッファのアドレスを取得
			*f4 = WaveData;
			UpdateShaderConstantBuffer(this->m_VertexShadercbWaveDataHandle);								// 頂点シェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(this->m_VertexShadercbWaveDataHandle, DX_SHADERTYPE_VERTEX, 5);		// 頂点シェーダーの定数バッファを定数バッファレジスタ４にセット
		}
		// シェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetGeometryCONSTBUFFER(int Slot, const MATRIX* ViewMatrix, const MATRIX* ProjectionMatrix) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			if (this->m_GeometryShaderhandle == InvalidID) { return; }
			DX_D3D11_GS_CONST_BUFFER_BASE* LightCameraMatrixConst = (DX_D3D11_GS_CONST_BUFFER_BASE*)GetBufferShaderConstantBuffer(this->m_GeometryShaderMatcbhandle);

			// ビュー変換用行列をセットする
			LightCameraMatrixConst->ViewMatrix[0][0] = ViewMatrix->m[0][0];
			LightCameraMatrixConst->ViewMatrix[0][1] = ViewMatrix->m[1][0];
			LightCameraMatrixConst->ViewMatrix[0][2] = ViewMatrix->m[2][0];
			LightCameraMatrixConst->ViewMatrix[0][3] = ViewMatrix->m[3][0];
			LightCameraMatrixConst->ViewMatrix[1][0] = ViewMatrix->m[0][1];
			LightCameraMatrixConst->ViewMatrix[1][1] = ViewMatrix->m[1][1];
			LightCameraMatrixConst->ViewMatrix[1][2] = ViewMatrix->m[2][1];
			LightCameraMatrixConst->ViewMatrix[1][3] = ViewMatrix->m[3][1];
			LightCameraMatrixConst->ViewMatrix[2][0] = ViewMatrix->m[0][2];
			LightCameraMatrixConst->ViewMatrix[2][1] = ViewMatrix->m[1][2];
			LightCameraMatrixConst->ViewMatrix[2][2] = ViewMatrix->m[2][2];
			LightCameraMatrixConst->ViewMatrix[2][3] = ViewMatrix->m[3][2];
			// 投影変換用行列をセットする
			LightCameraMatrixConst->ProjectionMatrix[0][0] = ProjectionMatrix->m[0][0];
			LightCameraMatrixConst->ProjectionMatrix[0][1] = ProjectionMatrix->m[1][0];
			LightCameraMatrixConst->ProjectionMatrix[0][2] = ProjectionMatrix->m[2][0];
			LightCameraMatrixConst->ProjectionMatrix[0][3] = ProjectionMatrix->m[3][0];
			LightCameraMatrixConst->ProjectionMatrix[1][0] = ProjectionMatrix->m[0][1];
			LightCameraMatrixConst->ProjectionMatrix[1][1] = ProjectionMatrix->m[1][1];
			LightCameraMatrixConst->ProjectionMatrix[1][2] = ProjectionMatrix->m[2][1];
			LightCameraMatrixConst->ProjectionMatrix[1][3] = ProjectionMatrix->m[3][1];
			LightCameraMatrixConst->ProjectionMatrix[2][0] = ProjectionMatrix->m[0][2];
			LightCameraMatrixConst->ProjectionMatrix[2][1] = ProjectionMatrix->m[1][2];
			LightCameraMatrixConst->ProjectionMatrix[2][2] = ProjectionMatrix->m[2][2];
			LightCameraMatrixConst->ProjectionMatrix[2][3] = ProjectionMatrix->m[3][2];
			LightCameraMatrixConst->ProjectionMatrix[3][0] = ProjectionMatrix->m[0][3];
			LightCameraMatrixConst->ProjectionMatrix[3][1] = ProjectionMatrix->m[1][3];
			LightCameraMatrixConst->ProjectionMatrix[3][2] = ProjectionMatrix->m[2][3];
			LightCameraMatrixConst->ProjectionMatrix[3][3] = ProjectionMatrix->m[3][3];

			UpdateShaderConstantBuffer(this->m_GeometryShaderMatcbhandle);
			SetShaderConstantBuffer(this->m_GeometryShaderMatcbhandle, DX_SHADERTYPE_GEOMETRY, Slot);		// 影用深度記録画像を描画したときのカメラのビュー行列と射影行列を定数に設定する
		}
		// ピクセルシェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetPixelCameraMatrix(int Slot, const Matrix4x4DX& View, const Matrix4x4DX& Projection) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// 設定したカメラのビュー行列と射影行列を取得しておく
			LIGHTCAMERA_MATRIX* LightCameraMatrixConst = (LIGHTCAMERA_MATRIX*)GetBufferShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(static_cast<size_t>(Slot - 4)));
			LightCameraMatrixConst->ViewMatrix = View.get();
			LightCameraMatrixConst->ProjectionMatrix = Projection.get();

			UpdateShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(static_cast<size_t>(Slot - 4)));
			SetShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(static_cast<size_t>(Slot - 4)), DX_SHADERTYPE_PIXEL, Slot);		// 影用深度記録画像を描画したときのカメラのビュー行列と射影行列を定数に設定する
		}
		// ピクセルシェーダ―の2番目のレジスタに画面サイズの情報をセット
		void			SetPixelDispSize(int dispx, int dispy) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			FLOAT2* dispsize = (FLOAT2*)GetBufferShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);	// ピクセルシェーダー用の定数バッファのアドレスを取得
			dispsize->u = static_cast<float>(dispx);
			dispsize->v = static_cast<float>(dispy);
			UpdateShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);									// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle, DX_SHADERTYPE_PIXEL, 2);				// ピクセルシェーダー用の定数バッファを定数バッファレジスタ2にセット
		}
		// ピクセルシェーダ―のSlot番目のレジスタに情報をセット(Slot>=3)
		void			SetPixelParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(this->m_PixelShadercbhandle.at(static_cast<size_t>(Slot - 3)));				// ピクセルシェーダー用の定数バッファのアドレスを取得
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(this->m_PixelShadercbhandle.at(static_cast<size_t>(Slot - 3)));											// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(this->m_PixelShadercbhandle.at(static_cast<size_t>(Slot - 3)), DX_SHADERTYPE_PIXEL, Slot);					// ピクセルシェーダー用の定数バッファを定数バッファレジスタ3にセット
		}
		// 3D空間に適用する場合の関数(引数に3D描画のラムダ式を代入)
		void			Draw_lamda(std::function<void()> doing) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				doing();
				return;
			}
			SetUseVertexShader(this->m_VertexShaderhandle);											// 使用する頂点シェーダーをセット
			SetUsePixelShader(this->m_PixelShaderhandle);											// 使用するピクセルシェーダーをセット
			SetUseGeometryShader(this->m_GeometryShaderhandle);										// 使用するジオメトリシェーダーをセット
			MV1SetUseOrigShader(TRUE);
			doing();
			MV1SetUseOrigShader(FALSE);
			SetUseVertexShader(InvalidID);
			SetUsePixelShader(InvalidID);
			SetUseGeometryShader(InvalidID);
		}
		// 2D画像に適用する場合の関数
		void			Draw(ScreenVertex& Screenvertex) const noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			Draw_lamda([&] {DrawPolygon3DToShader(Screenvertex.GetScreenVertex(), 2); });
		}
	};
	// キューブマップ生成
	class RealTimeCubeMap {
	private:
		GraphHandle dynamicCubeTex;		// 周囲を回る小さいモデルたちを映りこませるための描画対象にできるキューブマップテクスチャ
		Vector3DX lookAt[6]{};	// 映りこむ周囲の環境を描画する際のカメラの注視点
		Vector3DX up[6]{};		// 移りこむ周囲の環境を描画する際のカメラの上方向
		int MIPLEVEL = 2;
	public:
		RealTimeCubeMap(void) noexcept {}
		RealTimeCubeMap(const RealTimeCubeMap&) = delete;
		RealTimeCubeMap(RealTimeCubeMap&& o) = delete;
		RealTimeCubeMap& operator=(const RealTimeCubeMap&) = delete;
		RealTimeCubeMap& operator=(RealTimeCubeMap&& o) = delete;

		~RealTimeCubeMap(void) noexcept {}
	public:
		void Init(void) noexcept {
			// 描画対象にできるキューブマップテクスチャを作成
			SetCreateDrawValidGraphMipLevels(MIPLEVEL);
			SetCubeMapTextureCreateFlag(TRUE);
			dynamicCubeTex.Make(512, 512, true);
			SetCubeMapTextureCreateFlag(FALSE);
			SetCreateDrawValidGraphMipLevels(0);
			// 映りこむ環境を描画する際に使用するカメラの注視点とカメラの上方向を設定
			lookAt[0] = Vector3DX::right();
			lookAt[1] = Vector3DX::left();
			lookAt[2] = Vector3DX::up();
			lookAt[3] = Vector3DX::down();
			lookAt[4] = Vector3DX::forward();
			lookAt[5] = Vector3DX::back();
			up[0] = Vector3DX::up();
			up[1] = Vector3DX::up();
			up[2] = Vector3DX::back();
			up[3] = Vector3DX::forward();
			up[4] = Vector3DX::up();
			up[5] = Vector3DX::up();
		}

		void ReadyDraw(const Vector3DX& Pos, const std::function<void()>& Doing) noexcept {
			for (int i : std::views::iota(0, 6)) {		// 映りこむ環境を描画する面の数だけ繰り返し
				for (int j : std::views::iota(0, MIPLEVEL)) {			// ミップマップの数だけ繰り返し
					dynamicCubeTex.SetRenderTargetToShader(0, i, j);		// 描画先番号０番の描画対象を描画対象にできるキューブマップのi番目の面に設定
					ClearDrawScreen();										// クリア
					{
						SetupCamera_Perspective(90.0f / 180.0f * DX_PI_F);								// カメラの画角は90度に設定
						SetCameraNearFar(0.5f * Scale3DRate, 1000.0f * Scale3DRate);									// Nearクリップ面とFarクリップ面の距離を設定
						SetCameraPositionAndTargetAndUpVec(Pos.get(), (Pos + lookAt[static_cast<size_t>(i)]).get(), up[static_cast<size_t>(i)].get());	// カメラの位置と注視点、カメラの上方向を設定
						Doing();
					}
				}
			}
		}

		void Dispose(void) noexcept {
			dynamicCubeTex.Dispose();
		}

		const auto& GetCubeMapTex(void) const noexcept { return dynamicCubeTex; }
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// 汎用セーブデータ
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	typedef std::pair<std::string, int64_t> SaveParam;
	class SaveDataClass : public SingletonBase<SaveDataClass> {
	private:
		friend class SingletonBase<SaveDataClass>;
	private:
		std::vector<SaveParam> m_data;
	private:
		SaveDataClass(void) noexcept {
			Load();
		}
		SaveDataClass(const SaveDataClass&) = delete;
		SaveDataClass(SaveDataClass&& o) = delete;
		SaveDataClass& operator=(const SaveDataClass&) = delete;
		SaveDataClass& operator=(SaveDataClass&& o) = delete;

		~SaveDataClass(void) noexcept {}
	public:
		SaveParam* GetData(std::string_view Name) noexcept {
			for (auto& d : m_data) {
				if (d.first == Name) {
					return &d;
				}
			}
			return nullptr;
		}
	public:
		void SetParam(std::string_view Name, int64_t value) noexcept {
			auto* Data = GetData(Name);
			if (Data) {
				Data->second = value;
			}
			else {
				m_data.emplace_back(std::make_pair((std::string)Name, value));
			}
		}
		auto GetParam(std::string_view Name) noexcept {
			auto* Data = GetData(Name);
			if (Data) {
				return Data->second;
			}
			return (int64_t)InvalidID;
		}
	public:
		void Save(void) noexcept {
			std::ofstream outputfile("Save/new.svf");
			for (auto& d : m_data) {
				outputfile << d.first + "=" + std::to_string(d.second) + "\n";
			}
			outputfile.close();
		}
		bool Load(void) noexcept {

			m_data.clear();

			std::ifstream inputputfile("Save/new.svf");
			if (!inputputfile) {
				return false;
			}
			std::string line;
			while (std::getline(inputputfile, line)) {
				auto Start = line.find("=");
				if (Start != std::string::npos) {
					m_data.emplace_back(std::make_pair(line.substr(0, Start), std::stoi(line.substr(Start + 1))));
				}
			}
			inputputfile.close();
			return true;
		}
		void Reset(void) noexcept {
			m_data.clear();
		}
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// サイドログ
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class SideLog : public SingletonBase<SideLog> {
	private:
		friend class SingletonBase<SideLog>;
	private:
		class SideLogData {
			unsigned int m_Color{ 0 };
			char m_Message[64]{};
			float m_TimeMax{ -1.f };
			float m_Time{ -1.f };
			float m_Flip{ 0.f };
			float m_Flip_Y{ 0.f };
		public:
			void AddFlip(float value) noexcept { m_Flip += value; }
			template <typename... Args>
			void SetData(float second, unsigned int Color, const char* Mes, Args&&... args) noexcept {
				snprintfDx(m_Message, 64, Mes, args...);
				m_TimeMax = second;
				m_Time = m_TimeMax;
				m_Flip = 0.f;
				m_Flip_Y = -1.f;
				m_Color = Color;
			}
			void UpdateActive(void) noexcept;
		public:
			float GetFlip(void) const noexcept { return m_Flip_Y; }
			float ActivePer(void) const noexcept { return (m_Time > 1.f) ? std::clamp((m_TimeMax - m_Time) * 5.f + 0.1f, 0.f, 1.f) : std::clamp(m_Time, 0.f, 1.f); }
			const char* GetMsg(void) const noexcept { return m_Message; }
			unsigned int GetMsgColor(void) const noexcept { return m_Color; }
		};
	private:
		std::array<SideLogData, 16> data;
		size_t		m_LastSel{ 0 };

	private:
		SideLog(void) noexcept {}
		SideLog(const SideLog&) = delete;
		SideLog(SideLog&& o) = delete;
		SideLog& operator=(const SideLog&) = delete;
		SideLog& operator=(SideLog&& o) = delete;

		~SideLog(void) noexcept {}
	public:
		template <typename... Args>
		void Add(float second, unsigned int Color, const char* Mes, Args&&... args) noexcept {
			for (auto& d : data) {
				d.AddFlip(1.f);
			}
			data.at(m_LastSel).SetData(second, Color, Mes, args...);
			++m_LastSel %= data.size();
		}
		void Update(void) noexcept {
			for (auto& d : data) {
				if (d.ActivePer() > 0.f) {
					d.UpdateActive();
				}
			}
		}
		void Draw(void) noexcept;
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// ポップアップ
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class PopUp : public SingletonBase<PopUp> {
	private:
		friend class SingletonBase<PopUp>;
	private:
		class PopUpDrawClass {
		private:
			bool m_Active{ false };
			bool m_ActiveSwitch{ false };
			float m_ActivePer{ 0.f };
			char m_WindwoName[64]{};

			int WinSizeX{ 720 };
			int WinSizeY{ 720 };

			std::function<void(int xmin, int ymin, int xmax, int ymax, bool EndSwitch)> m_Doing{ nullptr };
			std::function<void()> m_ExitDoing{ nullptr };
			std::function<void()> m_GuideDoing{ nullptr };
		public:
			PopUpDrawClass(void) noexcept {}
			PopUpDrawClass(const PopUpDrawClass&) = delete;
			PopUpDrawClass(PopUpDrawClass&& o) = delete;
			PopUpDrawClass& operator=(const PopUpDrawClass&) = delete;
			PopUpDrawClass& operator=(PopUpDrawClass&& o) = delete;

			~PopUpDrawClass(void) noexcept {}
		public:
			void			Set(const char* WindowName, int sizex, int sizey,
				std::function<void(int xmin, int ymin, int xmax, int ymax, bool EndSwitch)> doing,
				std::function<void()> ExitDoing,
				std::function<void()> GuideDoing
			) noexcept {
				strcpy_sDx(m_WindwoName, 64, WindowName);
				WinSizeX = sizex;
				WinSizeY = sizey;
				m_Doing = doing;
				m_ExitDoing = ExitDoing;
				m_GuideDoing = GuideDoing;
			}
			void			Start(void) noexcept;
			void			End(void) noexcept;
			void			Update(void) noexcept;
			void			Draw(int xcenter, int ycenter) noexcept;
		public:
			auto IsEnd(void) const noexcept { return !m_Active && !(m_ActivePer > 1.f / 255.f); }
		};
	private:
		std::array<PopUpDrawClass, 24> que;
		size_t NowSel{ 0 };
		size_t LastSel{ 0 };
		bool PrevPause{ false };
	private:
		PopUp(void) noexcept {}
		PopUp(const PopUp&) = delete;
		PopUp(PopUp&& o) = delete;
		PopUp& operator=(const PopUp&) = delete;
		PopUp& operator=(PopUp&& o) = delete;

		~PopUp(void) noexcept {}
	public:
		auto IsActivePop(void) const noexcept { return (NowSel != LastSel); }
	public:
		void Add(const char* WindowName, int sizex, int sizey,
			std::function<void(int xmin, int ymin, int xmax, int ymax, bool EndSwitch)> doing,
			std::function<void()> ExitDoing,
			std::function<void()> GuideDoing,
			bool IsInsert = false) noexcept;
		void EndAll(void) noexcept;
		void Update(void) noexcept;
		void Draw(int xcenter, int ycenter) noexcept {
			if (!IsActivePop()) {
				return;
			}
			que.at(static_cast<size_t>(NowSel)).Draw(xcenter, ycenter);
		}
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// PCの情報を走査
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class CheckPCSpec {
		struct MatchScore {
			std::string		m_Name;
			int				m_Score{ 0 };
			int				m_HitCount{ 0 };
		};
	private:
		bool IsEnd = false;
		std::vector<MatchScore> CPUResult;
		std::vector<MatchScore> GPUResult;

		TCHAR CPUString[256]{ "" };
		double FreeMemorySize{ 0 };
		double TotalMemorySize{ 0 };
		TCHAR GPUString[256]{ "" };

		std::array<std::string, 12> CPUStr;
		std::array<std::string, 12> GPUStr;
		std::thread m_thread;
	public:
		static void GetOnlyNumber(const char* Target, std::array<std::string, 12>* String) noexcept {
			int NumCount = 0;
			for (auto& s : *String) {
				s = "";
			}
			for (int i : std::views::iota(0, static_cast<int>(strlenDx(Target)))) {
				if (!
					(
						Target[static_cast<size_t>(i)] == ' ' ||
						Target[static_cast<size_t>(i)] == '-'
						)
					) {
					String->at(static_cast<size_t>(NumCount)) += Target[static_cast<size_t>(i)];
				}
				else {
					if (String->at(static_cast<size_t>(NumCount)) != "") {
						++NumCount;
						String->at(static_cast<size_t>(NumCount)) = "";
					}
				}
			}
		}
	public:
		void FindCPU(void) noexcept {
			IsEnd = false;
			std::vector<MatchScore> Tmp; Tmp.reserve(64);
			{
				CPUResult.clear();
				std::ifstream inputputfile("CommonData/PassMarkCPU.txt");
				if (inputputfile) {
					Tmp.clear();
					std::string ALL;
					std::array<std::string, 12> TmpString;
					int HitCount = 0;
					int border = 0;
					while (std::getline(inputputfile, ALL)) {
						if (ALL == "") {
							continue;
						}
						auto LEFT = getparams::getleft(ALL);
						auto RIGHT = getparams::getright(ALL);
						GetOnlyNumber(LEFT.c_str(), &TmpString);
						HitCount = 0;
						for (auto& s : CPUStr) {
							if (s == "") {
								continue;
							}
							for (auto& t : TmpString) {
								if (t == "") {
									continue;
								}
								if ((s.find(t) != std::string::npos) || (t.find(s) != std::string::npos)) {
									++HitCount;
								}
							}
						}
						if (HitCount > 0) {
							border = std::max(HitCount, border);
							Tmp.resize(Tmp.size() + 1);
							Tmp.back().m_Name = LEFT;
							Tmp.back().m_Score = std::stoi(RIGHT);
							Tmp.back().m_HitCount = HitCount;
						}
					}
					inputputfile.close();
					for (auto& n : Tmp) {
						if (n.m_HitCount >= border) {
							CPUResult.emplace_back(n);
						}
					}
				}
			}
			{
				GPUResult.clear();
				std::ifstream inputputfile("CommonData/PassMarkGPU.txt");
				if (inputputfile) {
					Tmp.clear();
					std::string ALL;
					std::array<std::string, 12> TmpString;
					int HitCount = 0;
					int border = 0;
					while (std::getline(inputputfile, ALL)) {
						if (ALL == "") {
							continue;
						}
						auto LEFT = getparams::getleft(ALL);
						auto RIGHT = getparams::getright(ALL);
						GetOnlyNumber(LEFT.c_str(), &TmpString);
						HitCount = 0;
						for (auto& s : GPUStr) {
							if (s == "") {
								continue;
							}
							for (auto& t : TmpString) {
								if (t == "") {
									continue;
								}
								if ((s.find(t) != std::string::npos) || (t.find(s) != std::string::npos)) {
									++HitCount;
								}
							}
						}
						if (HitCount > 0) {
							border = std::max(HitCount, border);
							Tmp.resize(Tmp.size() + 1);
							Tmp.back().m_Name = LEFT;
							Tmp.back().m_Score = std::stoi(RIGHT);
							Tmp.back().m_HitCount = HitCount;
						}
					}
					inputputfile.close();
					for (auto& n : Tmp) {
						if (n.m_HitCount >= border) {
							GPUResult.emplace_back(n);
						}
					}
				}
			}
			IsEnd = true;
		}
	public:
	public:
		CheckPCSpec(void) noexcept {}
		CheckPCSpec(const CheckPCSpec&) = delete;
		CheckPCSpec(CheckPCSpec&& o) = delete;
		CheckPCSpec& operator=(const CheckPCSpec&) = delete;
		CheckPCSpec& operator=(CheckPCSpec&& o) = delete;
		~CheckPCSpec(void) noexcept {
			if (m_thread.joinable()) {
				m_thread.detach();
			}
		}
	public:
		const auto* GetCPUDatas(void) const noexcept { return IsEnd ? &CPUResult : nullptr; }
		const auto* GetGPUDatas(void) const noexcept { return IsEnd ? &GPUResult : nullptr; }
		const auto& GetFreeMemorySize(void) const noexcept { return FreeMemorySize; }
		const auto& GetTotalMemorySize(void) const noexcept { return TotalMemorySize; }
	public:
		void Set(void) noexcept {
			GetPcInfo(NULL, NULL, CPUString, NULL, &FreeMemorySize, &TotalMemorySize, NULL, GPUString, NULL, NULL);
			GetOnlyNumber(CPUString, &CPUStr);
			GetOnlyNumber(GPUString, &GPUStr);
		}
		void StartSearch(void) noexcept {
			if (m_thread.joinable()) {
				m_thread.detach();
			}
			std::thread newThead(&CheckPCSpec::FindCPU, this);
			m_thread.swap(newThead);
		}
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// 統一UI
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	namespace UniversalUI {
		enum class EnumUIPartsType :int {
			Zero = 0,
			Box = Zero,
			Msg,
			Max,
		};
		static const char* g_UIPartsString[static_cast<int>(EnumUIPartsType::Max)] = {
			"Box",
			"Msg",
		};
		enum class LerpType : size_t {
			linear,
			pow2,
			Max
		};
		static const char* g_LerpTypeStr[static_cast<int>(LerpType::Max)] = {
			"linear",
			"pow2",
		};

		static const int g_UIColorPalletNum = 13;
		static const unsigned int g_UIColorPallet[g_UIColorPalletNum] = {
			Red,
			Green,
			DarkGreen,
			Blue,
			Yellow,
			WhiteSel,
			White,
			Gray15,
			Gray25,
			Gray50,
			Gray65,
			Gray75,
			Black,
		};
		static const char* g_UIColorPalletString[g_UIColorPalletNum] = {
			"Red",
			"Green",
			"DarkGreen",
			"Blue",
			"Yellow",
			"WhiteSel",
			"White",
			"Gray15",
			"Gray25",
			"Gray50",
			"Gray65",
			"Gray75",
			"Black",
		};
		enum class UIXCenter : int {
			LEFT,
			MIDDLE,
			RIGHT,
		};
		static const char* g_UIXCenterString[] = {
			"Left",
			"Middle",
			"Right",
		};
		enum class UIYCenter : int {
			TOP,
			MIDDLE,
			BOTTOM,
		};
		static const char* g_UIYCenterString[] = {
			"Top",
			"Middle",
			"Bottom",
		};

		class UISystem : public SingletonBase<UISystem> {
		private:
			friend class SingletonBase<UISystem>;
		private:
			struct FrameInfo {
				int			m_framepoint{ 0 };	// 前回〜現在のフレーム数
				float		m_Alpha{ 1.f };		// 上記のフレーム数の段階でのXXX
				float		m_XScale{ 1.f };
				float		m_YScale{ 1.f };
				float		m_XOfs{ 0.f };
				float		m_YOfs{ 0.f };
				float		m_ZRotOfs{ 0.f };
				LerpType	m_LerpType{ LerpType::linear };// 前フレームとのつなぎ方
			};
			class UI_CommonParts {
			private:// 固定のパラメーター類
				int				m_UniqueID{ 0 };
				std::string		m_Name{};

				EnumUIPartsType	m_EnumUIPartsType{ EnumUIPartsType::Zero };
				int				m_Layer{ 0 };
				// 
				bool			m_IsMouseClickActive{ false };
				// 
				int				m_XPos{ 0 };
				int				m_YPos{ 0 };
				int				m_XSize{ 0 };
				int				m_YSize{ 0 };
				float			m_ZRotate{ 0.f };
				UIXCenter		m_UIXCenter{ UIXCenter::LEFT };
				UIYCenter		m_UIYCenter{ UIYCenter::TOP };
				// 
				unsigned int	m_PressColor{ Gray25 };
				unsigned int	m_IntoColor{ White };
				unsigned int	m_BaseColor{ Black };
				unsigned int	m_EdgeColor{ Gray75 };
				// 
				int				m_TextID{ 0 };
			private:// 変動する値
				bool			m_MouseOver{ false };
				bool			m_MousePress{ false };
				int				m_DrawXCenter{ 0 };
				int				m_DrawYCenter{ 0 };
			public:// 外からいじられるもの
				FrameInfo					m_FrameInfo{};
				std::array<std::string, 3>	m_TextEX0{};
			public:
				const auto& GetUniqueID(void) const noexcept { return m_UniqueID; }
				const auto& GetName(void) const noexcept { return m_Name; }
				const auto& GetLayer(void) const noexcept { return m_Layer; }

				const auto& GetMousePress(void) const noexcept { return m_MousePress; }
			public:
				void SetUniqueID(int value) noexcept { m_UniqueID = value; }
				void SetFrameInfo(const FrameInfo& value) noexcept { m_FrameInfo = value; }
			public:
				void SetParts(const nlohmann::json& pJson) noexcept;
				void Update(void) noexcept;
				void Draw(void) noexcept;
			};
			class UI_CommonAnimes {
			private:// 固定のパラメーター類
				std::vector<int> m_TargetID{};// 対照のパーツ
				std::vector<FrameInfo> m_AnimeFrame{};
			private:// 変動する値
				int m_NowAnim{ 0 };
			public:// 外からいじられるもの
				int m_Frame{ 0 };
			public:
			public:
				void SetParts(const nlohmann::json& pJson, const std::vector<std::unique_ptr<UI_CommonParts>>& Parts) noexcept;
				void Update(std::vector<std::unique_ptr<UI_CommonParts>>* Parts) noexcept;
			};

			class UI_OneLayer {
			private:
				std::vector<std::unique_ptr<UI_CommonParts>> m_CommonParts;
				std::vector<std::unique_ptr<UI_CommonAnimes>> m_CommonAnimes;
				int UniqueIDNum{ InvalidID };
				bool m_IsEnd{ false };// アニメーション側からの終了命令
			public:
				auto IsActive(void) const noexcept { return UniqueIDNum != InvalidID; }
			public:
				void Load(const char* path) noexcept;
				void Update(void) noexcept;
				void Draw(void) noexcept;
				void Dispose(void) noexcept;
			};
		private:
			std::array<UI_OneLayer, 5> m_Layer;
		public:
			UISystem(void) noexcept {}
			UISystem(const UISystem&) = delete;
			UISystem(UISystem&& o) = delete;
			UISystem& operator=(const UISystem&) = delete;
			UISystem& operator=(UISystem&& o) = delete;
			~UISystem(void) noexcept {}
		public:
			int AddUI(const char* path) noexcept;
			void DelUI(int layer) noexcept;

			void Update(void) noexcept;
			void Draw(void) noexcept;
			void DisposeAll(void) noexcept;
		};
	}
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	// カメラシェイク
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class CameraShake : public SingletonBase<CameraShake> {
	private:
		friend class SingletonBase<CameraShake>;
	private:
		bool						m_SendCamShake{ false };
		float						m_SendCamShakeTime{ 1.f };
		float						m_SendCamShakePower{ 1.f };
		float						m_CamShake{ 0.f };
		Vector3DX					m_CamShake1;
		Vector3DX					m_CamShake2;
	public:
		const auto& GetCamShake(void) const noexcept { return m_CamShake2; }
	public:
		void			SetCamShake(float time, float power) noexcept {
			this->m_SendCamShake = true;
			this->m_SendCamShakeTime = time;
			this->m_SendCamShakePower = power;
		}
	private:
		CameraShake(void) noexcept {}
		CameraShake(const CameraShake&) = delete;
		CameraShake(CameraShake&& o) = delete;
		CameraShake& operator=(const CameraShake&) = delete;
		CameraShake& operator=(CameraShake&& o) = delete;

		~CameraShake(void) noexcept {}
	public:
		void Update(void) noexcept;
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*UDP通信																																	*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	class UDPNetWorkDX {
		int			m_Handle{ InvalidID };				// ネットワークハンドル
		IPDATA		m_SendIP{ 127,0,0,1 };		// 送信用ＩＰアドレスデータ
		int			m_SendPort{ InvalidID };				// 通信用ポート
		IPDATA		m_RecvIp{ 127,0,0,1 };			// 受信用ＩＰアドレスデータ
		int			m_RecvPort{ 0 };				// 受信用ポート
	public:
		auto			IsActive(void) const noexcept { return (m_Handle != InvalidID); }
	public:
		void			SetServerIP(const IPDATA& pIP) noexcept { m_SendIP = pIP; }// クライアントは必ず行う
		bool			Init(bool IsServer, int PORT = InvalidID) noexcept {
			if (!IsActive()) {
				m_SendPort = PORT;
				m_Handle = MakeUDPSocket(IsServer ? m_SendPort : InvalidID);
			}
			return m_Handle != InvalidID;
		}
		void			Dispose(void) noexcept {
			if (IsActive()) {
				DeleteUDPSocket(m_Handle);	// ＵＤＰソケットハンドルの削除
				m_Handle = InvalidID;
				m_SendPort = InvalidID;
			}
		}
	private:
		template<class T>
		int				SendData(IPDATA& Ip, int SendPort, const T& Data) noexcept {
			if (IsActive()) {
				return NetWorkSendUDP(m_Handle, Ip, SendPort, &Data, sizeof(T));
			}
			return InvalidID;
		}
	public:
		// 送信
		template<class T>
		int				SendData(const T& Data) noexcept { return SendData(m_SendIP, m_SendPort, Data); }
		// 受信
		template<class T>
		bool			RecvData(T* Data, int* RecvReturn, bool IsPeek) noexcept {
			*RecvReturn = InvalidID;
			if (IsActive()) {
				switch (CheckNetWorkRecvUDP(m_Handle)) {
				case TRUE:
					*RecvReturn = NetWorkRecvUDP(m_Handle, &m_RecvIp, &m_RecvPort, Data, sizeof(T), IsPeek ? TRUE : FALSE);		// 受信
					return true;
				case FALSE:// 待機
					break;
				default:// error
					break;
				}
			}
			return false;
		}
		// 返送
		template<class T>
		int				ReturnData(const T& Data) noexcept { return SendData(m_RecvIp, m_RecvPort, Data); }
	};
}
