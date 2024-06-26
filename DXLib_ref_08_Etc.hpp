#pragma once
#include "DXLib_ref.h"

//リサイズ
#define x_r(p1) (int(p1) * DXDraw::Instance()->GetDispXSizeMax() / DXDraw::Instance()->GetDispXSizeMin())
#define y_r(p1) (int(p1) * DXDraw::Instance()->GetDispYSizeMax() / DXDraw::Instance()->GetDispYSizeMin())

#define EdgeSize	y_UI(2)
#define LineHeight	y_UI(18)

namespace DXLibRef {
	extern int y_UI(int p1);
	extern int y_UIMs(int p1);
	//--------------------------------------------------------------------------------------------------
	// 補完
	//--------------------------------------------------------------------------------------------------
	//イージング
	enum class EasingType {
		OutExpo,
	};
	extern float GetEasingRatio(EasingType EasingType, float ratio);
	//線形補完
	template <class T>
	inline T Lerp(const T& A, const T& B, float Per) noexcept {
		if (Per == 0.f) {
			return A;
		}
		else if (Per == 1.f) {
			return B;
		}
		else {
			return A + (T)((B - A)*Per);
		}
	}
	//Matrix版の線形補完
	template <>
	inline Matrix4x4DX Lerp(const Matrix4x4DX& A, const Matrix4x4DX& B, float Per) noexcept {
		return Matrix4x4DX::Axis1(
			Lerp(A.yvec(), B.yvec(), Per).normalized(),
			Lerp(A.zvec(), B.zvec(), Per).normalized(),
			Lerp(A.pos(), B.pos(), Per));
	}
	//
	template <class T>
	static void Easing(T* A, const T& B, float ratio, EasingType EasingType) { *A = Lerp(*A, B, GetEasingRatio(EasingType, ratio)); };
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*関数																																		*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	//クリップボードに画像をコピー
	static const auto GetClipBoardGraphHandle(GraphHandle* RetHandle) noexcept {
		HWND   hwnd = GetMainWindowHandle();

		if (IsClipboardFormatAvailable(CF_BITMAP) == FALSE) { return false; }	// 指定した形式のデータがクリップボードにあるかを問い合わせる
		if (OpenClipboard(hwnd) == FALSE) { return false; }						// オープン
		HBITMAP hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);					// データ取得
		CloseClipboard();														// クローズ
		if (hBitmap == NULL) { return false; }

		BITMAP bitmap;
		GetObject(hBitmap, sizeof(BITMAP), &bitmap);

		BITMAPINFO bmpInfo;
		bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.bmiHeader.biWidth = bitmap.bmWidth;
		bmpInfo.bmiHeader.biHeight = bitmap.bmHeight;
		bmpInfo.bmiHeader.biPlanes = 1;
		bmpInfo.bmiHeader.biBitCount = 32;
		bmpInfo.bmiHeader.biCompression = BI_RGB;

		size_t Siz = (size_t)(bitmap.bmWidth * bitmap.bmHeight * (LONG)4);
		BYTE *bmpData = new BYTE[Siz];
		HDC hDC = GetDC(hwnd);
		GetDIBits(hDC, hBitmap, 0, bitmap.bmHeight, (void*)bmpData, &bmpInfo, DIB_RGB_COLORS);
		ReleaseDC(hwnd, hDC);
		DeleteObject(hBitmap);

		int graphhandle = CreateGraphFromBmp(&bmpInfo, bmpData);	// ハンドルに変換
		delete[] bmpData;
		*RetHandle = graphhandle;
		return true;
	}
	//矩形と点との2D判定
	static bool HitPointToRectangle(int xp, int  yp, int x1, int  y1, int  x2, int  y2) { return (xp >= x1 && xp <= x2 && yp >= y1 && yp <= y2); }

	static bool HitPointToSquare(int xp, int  yp, int x1, int  y1, int  x2, int  y2, int  x3, int  y3, int  x4, int  y4)
	{
		if (0 > Vector3DX::Cross(VGet((float)(x2 - x1), (float)(y2 - y1), 0.f), VGet((float)(xp - x1), (float)(yp - y1), 0.f)).z) { return false; }
		if (0 > Vector3DX::Cross(VGet((float)(x3 - x2), (float)(y3 - y2), 0.f), VGet((float)(xp - x2), (float)(yp - y2), 0.f)).z) { return false; }
		if (0 > Vector3DX::Cross(VGet((float)(x4 - x3), (float)(y4 - y3), 0.f), VGet((float)(xp - x3), (float)(yp - y3), 0.f)).z) { return false; }
		if (0 > Vector3DX::Cross(VGet((float)(x1 - x4), (float)(y1 - y4), 0.f), VGet((float)(xp - x4), (float)(yp - y4), 0.f)).z) { return false; }
		return true;
	}

	//マウスと矩形の判定
	extern bool IntoMouse(int x1, int  y1, int  x2, int  y2);
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*DXLIBラッパー																																*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	//--------------------------------------------------------------------------------------------------
	// 演算補助
	//--------------------------------------------------------------------------------------------------
	//三角とカプセルとの判定
	static bool GetHitCapsuleToTriangle(const Vector3DX& startpos, const Vector3DX& endpos, float size, const Vector3DX& tri_p1, const Vector3DX& tri_p2, const Vector3DX& tri_p3) {
		return HitCheck_Capsule_Triangle(startpos.get(), endpos.get(), size, tri_p1.get(), tri_p2.get(), tri_p3.get()) == TRUE;
	}
	//直線と直線の一番近い距離
	static float GetMinLenSegmentToSegment(const Vector3DX& startpos, const Vector3DX& endpos, const Vector3DX& tgtstartpos, const Vector3DX& tgtendpos) {
		return Segment_Segment_MinLength(startpos.get(), endpos.get(), tgtstartpos.get(), tgtendpos.get());
	}
	//直線と点の一番近い点
	static float GetMinLenSegmentToPoint(const Vector3DX& startpos, const Vector3DX& endpos, const Vector3DX& tgt) {
		return Segment_Point_MinLength(startpos.get(), endpos.get(), tgt.get());
	}
	//平面と点の一番近い点
	static Vector3DX GetMinPosSegmentToPoint(const Vector3DX& startpos, const Vector3DX& endpos, const Vector3DX& tgt) {
		return Plane_Point_MinLength_Position(startpos.get(), endpos.get(), tgt.get());
	}

	//ランダム
	static float GetRandf(float m_arg) noexcept { return -m_arg + (float)(GetRand((int)(m_arg * 2.f * 10000.f))) / 10000.f; }
	//線分同士の交差判定
	static bool GetSegmenttoSegment(const Vector3DX &SegmentAPos1, const Vector3DX &SegmentAPos2, const Vector3DX &SegmentBPos1, const Vector3DX &SegmentBPos2, SEGMENT_SEGMENT_RESULT *Result) noexcept {
		VECTOR Pos1t = SegmentAPos1.get();
		VECTOR Pos2t = SegmentAPos2.get();
		VECTOR PosAt = SegmentBPos1.get();
		VECTOR PosBt = SegmentBPos2.get();

		Segment_Segment_Analyse(&Pos1t, &Pos2t, &PosAt, &PosBt, Result);
		float len = 0.001f;
		return (Result->SegA_SegB_MinDist_Square <= (len*len));
	}

	static bool GetSegmenttoTriangle(const Vector3DX &SegmentAPos1, const Vector3DX &SegmentAPos2, const Vector3DX &SegmentBPos1, const Vector3DX &SegmentBPos2, const Vector3DX &SegmentBPos3, SEGMENT_TRIANGLE_RESULT *Result) noexcept {
		VECTOR Pos1t = SegmentAPos1.get();
		VECTOR Pos2t = SegmentAPos2.get();
		VECTOR PosAt = SegmentBPos1.get();
		VECTOR PosBt = SegmentBPos2.get();
		VECTOR PosCt = SegmentBPos3.get();

		Segment_Triangle_Analyse(&Pos1t, &Pos2t, &PosAt, &PosBt, &PosCt, Result);
		float len = 0.001f;
		return (Result->Seg_Tri_MinDist_Square <= (len*len));
	}
	//線分と点の最接近点情報を解析する
	static bool GetSegmenttoPoint(const Vector3DX &SegmentAPos1, const Vector3DX &SegmentAPos2, const Vector3DX &PointPos, SEGMENT_POINT_RESULT *Result) noexcept {
		VECTOR Pos1t = SegmentAPos1.get();
		VECTOR Pos2t = SegmentAPos2.get();
		VECTOR PosAt = PointPos.get();

		Segment_Point_Analyse(&Pos1t, &Pos2t, &PosAt, Result);
		float len = 0.001f;
		return (Result->Seg_Point_MinDist_Square <= (len*len));
	}

	//カメラから画面上の座標を取得
	static Vector3DX GetScreenPos(const Vector3DX&campos, const Vector3DX&camvec, const Vector3DX&camup, float fov, float near_t, float far_t, const Vector3DX&worldpos) noexcept;
	//--------------------------------------------------------------------------------------------------
	// ウィンドウアクティブチェック付きキー操作
	//--------------------------------------------------------------------------------------------------
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
	//--------------------------------------------------------------------------------------------------
	// 描画
	//--------------------------------------------------------------------------------------------------
	//線の描画
	static bool DrawLine_2D(int p1x, int p1y, int p2x, int p2y, const unsigned int& color, int thickness = 1) noexcept {
		return DxLib::DrawLine(p1x, p1y, p2x, p2y, color, thickness) == TRUE;
	}
	//縁付き四角
	static bool DrawBox_2D(int p1x, int p1y, int p2x, int p2y, const unsigned int& color, bool IsFill) noexcept {
		return DxLib::DrawBox(p1x, p1y, p2x, p2y, color, IsFill ? TRUE : FALSE) == TRUE;
	}
	//縁ぬき四角
	static void DrawBoxLine_2D(int p1x, int p1y, int p2x, int p2y, const unsigned int& color, int thickness = 1) noexcept {
		DrawLine_2D(p1x, p1y, p1x, p2y, color, thickness);
		DrawLine_2D(p1x, p1y, p2x, p1y, color, thickness);
		DrawLine_2D(p1x, p2y, p2x, p2y, color, thickness);
		DrawLine_2D(p2x, p1y, p2x, p2y, color, thickness);
	}
	//グラデーションのある矩形を描画
	static void DrawGradationBox_2D(int x1, int y1, int x2, int y2, COLOR_U8 color1, COLOR_U8 color2, const unsigned char UorL = 255) noexcept {
		VERTEX2D Vertex[6];
		// 左上の頂点の情報をセット
		Vertex[0].pos.x = (float)x1;
		Vertex[0].pos.y = (float)y1;
		Vertex[0].pos.z = 0.0f;
		Vertex[0].rhw = 1.0f;
		Vertex[0].dif = color1;
		Vertex[0].u = 0.0f;
		Vertex[0].v = 0.0f;

		// 右上の頂点の情報をセット
		Vertex[1].pos.x = (float)x2;
		Vertex[1].pos.y = (float)y1;
		Vertex[1].pos.z = 0.0f;
		Vertex[1].rhw = 1.0f;
		Vertex[1].dif = color2;
		Vertex[1].u = 0.0f;
		Vertex[1].v = 0.0f;

		Vertex[5] = Vertex[1];

		// 左下の頂点の情報をセット
		Vertex[2].pos.x = (float)x1;
		Vertex[2].pos.y = (float)y2;
		Vertex[2].pos.z = 0.0f;
		Vertex[2].rhw = 1.0f;
		Vertex[2].dif = color1;
		Vertex[2].u = 0.0f;
		Vertex[2].v = 0.0f;

		Vertex[4] = Vertex[2];

		// 右下の頂点の情報をセット
		Vertex[3].pos.x = (float)x2;
		Vertex[3].pos.y = (float)y2;
		Vertex[3].pos.z = 0.0f;
		Vertex[3].rhw = 1.0f;
		Vertex[3].u = 0.0f;
		Vertex[3].v = 0.0f;

		switch (UorL)
		{
		case 3://横(右)
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
		case 2://縦(下)
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
		case 1://横(左)
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
		case 0://縦(上)
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
	//	x1,y1 Angleが0の場合の左上座標
	//	x2,y2 Angleが0の場合の右下座標
	//	xminp,yminp 左上角からの固定長さ
	//	xmaxp,ymaxp 右下角からの固定長さ
	//	XCenter,YCenter　　: 画像を回転描画する画像上の中心座標(左上を(0.0f,0.0f)、右下を(1.0f,1.0f)とした割合)
	//	Angle　　　: 描画角度（ラジアン指定）
	//	GrHandle　 : 描画するグラフィックの識別番号（グラフィックハンドル）
	//	TransFlag　: 画像の透明度を有効にするかどうか( TRUE：有効にする　FALSE：無効にする )
	//	TilingFlag : 角以外の部分をタイリングするか拡縮させるか( TRUE：タイリング　FALSE：拡縮 )
	static void Draw9SliceGraph(
		int x1, int y1, int x2, int y2,
		int xminp, int yminp, int xmaxp, int ymaxp,
		float XCenter, float YCenter, float Angle,
		int GrHandle, bool TransFlag, bool TilingFlag) noexcept {
		//最低限のサイズを指定
		if (x2 < x1 + xminp + xmaxp) { x2 = x1 + xminp + xmaxp; }
		if (y2 < y1 + yminp + ymaxp) { y2 = y1 + yminp + ymaxp; }
		//用意する頂点情報
		std::vector<VERTEX2D> Vertex;
		std::vector<unsigned short> Index;

		float xs = (float)(x2 - x1);
		float ys = (float)(y2 - y1);

		float CenterX = (float)x1 + xs * XCenter;
		float CenterY = (float)y1 + ys * YCenter;

		auto SetPoint = [&](float xper, float yper, int xc, int yc) {
			Vertex.resize(Vertex.size() + 1);
			Vertex.back().pos = VGet(
				(float)x1 + xs * xper - CenterX,
				(float)y1 + ys * yper - CenterY,
				0.f);

			Vertex.back().pos = VGet(
				CenterX + Vertex.back().pos.x*std::cos(Angle) - Vertex.back().pos.y*std::sin(Angle),
				CenterY + Vertex.back().pos.x*std::sin(Angle) + Vertex.back().pos.y*std::cos(Angle),
				0.f);

			Vertex.back().rhw = 1.0f;
			Vertex.back().dif = GetColorU8(255, 255, 255, 255);
			Vertex.back().u = (float)xc / 3.f;
			Vertex.back().v = (float)yc / 3.f;
			return (unsigned short)(Vertex.size() - 1);
		};
		auto SetBox = [&](float xmin, float ymin, float xmax, float ymax, int xc, int yc) {
			Index.emplace_back(SetPoint(xmin, ymin, xc, yc));// 左上の頂点の情報をセット
			auto RU = SetPoint(xmax, ymin, xc + 1, yc);
			auto LD = SetPoint(xmin, ymax, xc, yc + 1);
			Index.emplace_back(RU);// 右上の頂点の情報をセット
			Index.emplace_back(LD);// 左下の頂点の情報をセット
			Index.emplace_back(SetPoint(xmax, ymax, xc + 1, yc + 1));// 右下の頂点の情報をセット
			Index.emplace_back(LD);// 左下の頂点の情報をセット
			Index.emplace_back(RU);// 右上の頂点の情報をセット
		};

		float xminpt = (float)xminp / xs;
		float xmaxpt = (float)xmaxp / xs;
		float xmaxt = 1.f - xmaxpt;
		float xmidt = xmaxt - xminpt;

		float yminpt = (float)yminp / ys;
		float ymaxpt = (float)ymaxp / ys;
		float ymaxt = 1.f - ymaxpt;
		float ymidt = ymaxt - yminpt;

		int xtile = 1;
		int ytile = 1;
		//タイリング
		if (TilingFlag) {
			xtile = (int)(xmidt / ((xminpt + xmaxpt) / 2.f)) + 1;
			if (xtile <= 0) { xtile = 1; }
			ytile = (int)(ymidt / ((yminpt + ymaxpt) / 2.f)) + 1;
			if (ytile <= 0) { ytile = 1; }
		}

		Vertex.reserve((size_t)(3 * 2 * ((xtile + 2) * (ytile + 2))));
		float xmin = 0.f;
		float xmax = xminpt;
		int xc = 0;
		for (int x = 0;x < xtile + 2;x++) {
			float ymin = 0.f;
			float ymax = yminpt;
			int yc = 0;
			for (int y = 0;y < ytile + 2;y++) {
				SetBox(xmin, ymin, xmax, ymax, xc, yc);
				//次
				ymin = ymax;
				ymax = TilingFlag ? (ymin + ymidt / ytile) : ymaxt;
				if (y == 0) {
					yc = 1;
				}
				if (y == ytile) {
					ymax = 1.f;
					yc = 2;
				}
			}
			//次
			xmin = xmax;
			xmax = TilingFlag ? (xmin + xmidt / xtile) : xmaxt;
			if (x == 0) {
				xc = 1;
			}
			if (x == xtile) {
				xmax = 1.f;
				xc = 2;
			}
		}
		DrawPolygonIndexed2D(Vertex.data(), (int)Vertex.size(), Index.data(), (int)Index.size() / 3, GrHandle, TransFlag ? TRUE : FALSE);
	}
	//カプセル描画
	static bool DrawCapsule_3D(const Vector3DX& p1, const Vector3DX& p2, float range, const unsigned int& color, const unsigned int& speccolor) noexcept {
		return DxLib::DrawCapsule3D(p1.get(), p2.get(), range, 8, color, speccolor, TRUE) == TRUE;
	}
	//球描画
	static bool DrawSphere_3D(const Vector3DX& p1, float range, const unsigned int& color, const unsigned int& speccolor) noexcept {
		return DxLib::DrawSphere3D(p1.get(), range, 8, color, speccolor, TRUE) == TRUE;
	}

	//カラー指定
	static const unsigned int Red{GetColor(255, 0, 0)};

	static const unsigned int Green{GetColor(0, 255, 0)};//GetColor(43, 255, 91)
	static const unsigned int DarkGreen{GetColor(0, 64, 0)};//GetColor(21, 128, 45)

	static const unsigned int Blue{GetColor(0, 0, 255)};

	static const unsigned int Yellow{GetColor(255, 255, 0)};

	static const unsigned int WhiteSel{GetColor(216, 255, 216)};

	static const unsigned int White{GetColor(255, 255, 255)};
	static const unsigned int Gray15{GetColor(216, 216, 216)};
	static const unsigned int Gray25{GetColor(192, 192, 192)};
	static const unsigned int Gray50{GetColor(128, 128, 128)};
	static const unsigned int Gray65{GetColor(96, 96, 96)};
	static const unsigned int Gray75{GetColor(64, 64, 64)};
	static const unsigned int Black{GetColor(0, 0, 0)};

	namespace WindowSystem {
		//箱
		extern void SetBox(int xp1, int yp1, int xp2, int yp2, unsigned int colorSet);
		extern bool SetClickBox(int xp1, int yp1, int xp2, int yp2, unsigned int colorSet);
		//文字
		template <typename... Args>
		extern const int GetMsgLen(int size, std::string_view String, Args&&... args) {
			auto* Fonts = FontPool::Instance();
			return Fonts->Get(FontPool::FontType::Nomal_EdgeL, (int)(size)).GetStringWidth(-1, ((std::string)String).c_str(), args...) + y_UI(8);//エッジ分:
		}

		const bool GetMsgPos(int* xp1, int *yp1, int xp2, int yp2, int size, int xSize, FontHandle::FontXCenter FontX);

		template <typename... Args>
		extern const int SetMsg(int xp1, int yp1, int xp2, int yp2, int size, FontHandle::FontXCenter FontX, unsigned int Color, unsigned int EdleColor, std::string_view String, Args&&... args) {
			if (String == "") { return 0; }
			auto* Fonts = FontPool::Instance();
			int xSize = GetMsgLen(size, String, args...);
			if (!GetMsgPos(&xp1, &yp1, xp2, yp2, size, xSize, FontX)) {
				return 0;
			}
			Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString((int)(size), FontX, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, EdleColor, ((std::string)String).c_str(), args...);
			return xSize;//エッジ分
		};
		//
		template <typename... Args>
		extern const int GetMsgLenWW(int size, std::string_view String, Args&&... args) {
			auto* Fonts = FontPool::Instance();
			return Fonts->Get(FontPool::FontType::WW_Gothic, (int)(size)).GetStringWidth(-1, ((std::string)String).c_str(), args...) + y_UI(8);//エッジ分:
		}

		template <typename... Args>
		extern const int SetMsgWW(int xp1, int yp1, int xp2, int yp2, int size, FontHandle::FontXCenter FontX, unsigned int Color, unsigned int EdleColor, std::string_view String, Args&&... args) {
			if (String == "") { return 0; }
			auto* Fonts = FontPool::Instance();
			int xSize = GetMsgLenWW(size, String, args...);
			if (!GetMsgPos(&xp1, &yp1, xp2, yp2, size, xSize, FontX)) {
				return 0;
			}
			Fonts->Get(FontPool::FontType::WW_Gothic).DrawString((int)(size), FontX, FontHandle::FontYCenter::MIDDLE, xp1, yp1, Color, EdleColor, ((std::string)String).c_str(), args...);
			return xSize;//エッジ分
		};
		//
		template <typename... Args>
		extern bool SetMsgClickBox(int xp1, int yp1, int xp2, int yp2, unsigned int defaultcolor, std::string_view String, Args&&... args) {
			bool ret = SetClickBox(xp1, yp1, xp2, yp2, defaultcolor);
			SetMsgWW(xp1, yp1, xp2, yp2, std::min(LineHeight, yp2 - yp1), FontHandle::FontXCenter::MIDDLE, White, Black, String, args...);
			return ret;
		};
		template <typename... Args>
		extern void SetMsgBox(int xp1, int yp1, int xp2, int yp2, unsigned int defaultcolor, std::string_view String, Args&&... args) {
			SetBox(xp1, yp1, xp2, yp2, defaultcolor);
			SetMsg(xp1, yp1, xp2, yp2, std::min(LineHeight, yp2 - yp1), FontHandle::FontXCenter::MIDDLE, White, Black, String, args...);
		};

		extern bool CheckBox(int xp1, int yp1, bool switchturn);

		extern int UpDownBar(int xmin, int xmax, int yp, int value, int valueMin, int valueMax);

		extern int UpDownBox(int xmin, int xmax, int yp, int value, int valueMax);
		//
		/*
		class ScrollBoxClass {
			bool		m_IsChangeScrollY{ false };
			int			m_BaseScrollY{ 0 };
			float		m_NowScrollYPer{ 0.f };
		public:
			const auto&		GetNowScrollYPer(void) const noexcept { return this->m_NowScrollYPer; }
			void			ScrollBox(int xp1, int yp1, int xp2, int yp2, float TotalPer, bool IsActive) {
				auto* Pad = PadControl::Instance();
				unsigned int color = Gray25;

				int length = (int)((float)(yp2 - yp1) / TotalPer);
				float Total = (float)(yp2 - yp1 - length);
				int Yp_t = (int)(Total * this->m_NowScrollYPer);
				int Yp_s = std::max(yp1, yp1 + Yp_t);
				int Yp_e = std::min(yp2, Yp_s + length);

				if (IsActive) {
					if (IntoMouse(xp1, yp1, xp2, yp2)) {
						if (Pad->GetWheelAdd() != 0.f) {
							m_NowScrollYPer = std::clamp(m_NowScrollYPer + (float)(-Pad->GetWheelAdd() * 3) / Total, 0.f, 1.f);
						}
					}
					if (IntoMouse(xp2 - y_UI(24), yp1, xp2, yp2)) {
						if (Pad->GetINTERACTKey().trigger()) {
							m_IsChangeScrollY = true;
						}

						if (!m_IsChangeScrollY) {
							HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
							SetCursor(hCursor);
						}
					}
					if (m_IsChangeScrollY) {
						if (Pad->GetINTERACTKey().press()) {
							color = White;
							m_NowScrollYPer = std::clamp((float)(Pad->GetMouseY() - this->m_BaseScrollY) / Total, 0.f, 1.f);

							HCURSOR hCursor = LoadCursor(NULL, IDC_SIZENS);
							SetCursor(hCursor);
						}
						else {
							m_IsChangeScrollY = false;
						}
					}
					else {
						m_BaseScrollY = Pad->GetMouseY() - Yp_t;
						if (Pad->GetMouseY() < Yp_s) {
							m_BaseScrollY += Yp_s - Pad->GetMouseY();
						}

						if (Pad->GetMouseY() > Yp_e) {
							m_BaseScrollY += Yp_e - Pad->GetMouseY();
						}
					}
				}
				SetBox(xp2 - y_UI(24), yp1, xp2, yp2, Gray50);
				SetBox(xp2 - y_UI(24) + y_UI(1), Yp_s, xp2 - y_UI(1), Yp_e, color);
			};
		};
		//*/
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*クラス																																	*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	//文字列から数値を取り出す
	class getparams {
		static const int charLength = 512;
	public:
		//文字列から=より右の値取得
		static const std::string getleft(std::string tempname) { return tempname.substr(0, tempname.find('=')); }
		//文字列から=より右の値取得
		static const std::string getright(std::string tempname) { return tempname.substr(tempname.find('=') + 1); }
	public:
		//左辺の情報もとる
		static const std::string		getcmd(int p1, int *p2) {
			char mstr[charLength]; /*tank*/
			FileRead_gets(mstr, charLength, p1);
			*p2 = std::stoi(getright(mstr));
			return getleft(mstr);
		}
		//1行そのまま取得
		static auto						Getstr(int p1) {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return std::string(mstr);
		}
		//=より右の値取得
		static const std::string		_str(int p1) {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return getright(mstr);
		}
		static const long int			_int(int p1) {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return std::stoi(getright(mstr));
		}
		static const long int			_long(int p1) {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return std::stol(getright(mstr));
		}
		static const unsigned long int	_ulong(int p2) {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p2);
			return std::stoul(getright(mstr));
		}
		static const float				_float(int p1) {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return std::stof(getright(mstr));
		}
		static const bool				_bool(int p1) {
			char mstr[charLength] = "";
			FileRead_gets(mstr, charLength, p1);
			return (getright(mstr).find("true") != std::string::npos);
		}
	};
	//モデルのフレーム情報保持
	class frames {
		int			m_FrameID{ -1 };
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
			if (obj.frame_parent(i) >= 0) {
				m_LocalPos = obj.GetFrameLocalMatrix(i);
			}
			else {
				m_LocalPos = obj.GetFrameLocalWorldMatrix(i);//
			}
		}
		const auto&		GetFrameID() const noexcept { return m_FrameID; }
		const auto&		GetFrameWorldPosition() const noexcept { return m_WorldPos; }
		const auto&		GetFrameLocalPosition() const noexcept { return m_LocalPos; }
	};
	//位置情報
	class moves {
	public:
		Vector3DX pos;		//座標
		Vector3DX posbuf;	//座標
		Vector3DX repos;	//前フレームの座標
		float		Speed{0.f};
		Vector3DX vec;		//加速
		Matrix4x4DX mat;		//回転
		Vector3DX rad;		//回転

		const Matrix4x4DX MatIn(void) const noexcept { return mat * Matrix4x4DX::Mtrans(pos); }

		void			operator=(const moves&tgt) {
			this->pos = tgt.pos;
			this->repos = tgt.repos;
			this->vec = tgt.vec;
			this->mat = tgt.mat;
			this->rad = tgt.rad;
		}
		const auto operator+(const moves& o) const noexcept {
			//mat,reposに関しては必要に応じて1
			moves tmp;
			tmp.pos = this->pos + o.pos;
			tmp.vec = this->vec + o.vec;
			tmp.rad = this->rad + o.rad;
			return tmp;
		}
		const auto operator-(const moves& o) const noexcept {
			//mat,reposに関しては必要に応じて1
			moves tmp;
			tmp.pos = this->pos - o.pos;
			tmp.vec = this->vec - o.vec;
			tmp.rad = this->rad - o.rad;

			return tmp;
		}
		const auto operator*(float per) const noexcept {
			//mat,reposに関しては必要に応じて1
			moves tmp;
			tmp.pos = this->pos*per;
			tmp.vec = this->vec*per;
			tmp.rad = this->rad*per;
			return tmp;
		}


		void			UpdatePos(const Vector3DX& tgt) {
			this->repos = this->pos;
			this->pos = tgt;
		}
		void			UpdatePosBuf(const Vector3DX& tgt, float per) {
			this->posbuf = tgt;
			auto Repos = this->pos;
			Easing(&this->pos, this->posbuf, per, EasingType::OutExpo);
			this->Speed = (this->pos - Repos).magnitude();
			this->repos = this->posbuf;
		}

		void			Update_Physics(float speed_randam = 0.f, float rate = 1.f);

		void			HitGround(const MV1_COLL_RESULT_POLY& colres, float hight) {//0.005f
			this->pos = Vector3DX(colres.HitPosition) + Vector3DX(colres.Normal)*hight;
			this->mat *= Matrix4x4DX::RotVec2(this->mat.xvec(), Vector3DX(colres.Normal)*-1.f);
		}
	};
	//キー押し判定
	class switchs {
		bool		m_on{false};//オンオフ判定
		bool		m_press{false};//オンオフ判定
		bool		m_repeat{false};//オンオフ判定
		int8_t		m_presscount{0};//プッシュ判定
		float		m_repeatcount{30.f};//プッシュ判定
	public:
		switchs(void) noexcept {
			Set(false);
			m_presscount = 0;
			m_repeatcount = 30.f;
			m_press = false;
		};
		~switchs(void) noexcept {}
		//使用前の用意
		void			Set(bool on) noexcept { m_on = on; }
		//更新
		void			Execute(bool key) noexcept;
		//オンオフの取得
		const bool on(void) const noexcept { return m_on; }
		//押した瞬間
		const bool trigger(void) const noexcept { return m_press && (m_presscount == 1); }
		//押している間
		const bool press(void) const noexcept { return m_press; }
		//押している間
		const bool repeat(void) const noexcept { return m_repeat; }
		//離した瞬間
		const bool release_trigger(void) const noexcept { return (!m_press) && (m_presscount == 1); }
		//離している間
		const bool release(void) const noexcept { return !m_press; }
	};
	// 2次元振り子演算
	class Pendulum2D {
		float	m_PendulumLength = 10.f;
		float	m_PendulumMass = 2.f;
		float	m_drag_coeff = 2.02f;

		float	m_rad = deg2rad(12.f);
		float	m_vel = 0.f;
	public:
		void Init(float Length, float N, float rad) {
			m_PendulumLength = Length;
			m_PendulumMass = N;
			m_rad = rad;
			m_vel = 0.f;
		}
		void Update();
	public:
		const auto GetRad() const noexcept { return this->m_rad; }
		void AddRad(float value) noexcept { this->m_rad += value; }
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*シェーダー																																*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	//シェーダーを使用する際の補助クラス
	class ShaderUseClass {
	public:
		//2Dにシェーダーを適用する際に使用する画面サイズの頂点情報
		class ScreenVertex {
			VERTEX3DSHADER Screen_vertex[6] = {};
		public:
			// 頂点データの取得
			const auto*		GetScreenVertex() noexcept { return Screen_vertex; }
			// 頂点データの準備
			void			SetScreenVertex(int dispx, int dispy) noexcept {
				int xp1 = 0;
				int yp1 = dispy;
				int xp2 = dispx;
				int yp2 = 0;
				Screen_vertex[0].pos = VGet((float)xp1, (float)yp1, 0.0f);
				Screen_vertex[1].pos = VGet((float)xp2, (float)yp1, 0.0f);
				Screen_vertex[2].pos = VGet((float)xp1, (float)yp2, 0.0f);
				Screen_vertex[3].pos = VGet((float)xp2, (float)yp2, 0.0f);
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
		//DXLIBから引っ張ってきたシェーダー用の定義
		typedef float DX_D3D11_SHADER_FLOAT4[4];
		struct DX_D3D11_GS_CONST_BUFFER_BASE {
			DX_D3D11_SHADER_FLOAT4		ProjectionMatrix[4];											// ビュー　→　プロジェクション行列
			DX_D3D11_SHADER_FLOAT4		ViewMatrix[3];												// ワールド　→　ビュー行列
		};
	private:
		//シェーダーハンドル
		int m_VertexShaderhandle{-1};
		int m_GeometryShaderhandle{-1};
		int m_PixelShaderhandle{-1};
		//シェーダーに渡す追加パラメーターを配するハンドル
		std::array<int, 4> LightCameraMatrixConstantBufferHandle{-1};	// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列を設定するための定数バッファ
		std::array<int, 4> m_VertexShadercbhandle{-1};
		int m_GeometryShaderMatcbhandle{-1};
		int m_PixelShaderSendDispSizeHandle{-1};
		std::array<int, 4> m_PixelShadercbhandle{-1};

	public:
		ShaderUseClass() {
			//シェーダーハンドル
			m_VertexShaderhandle = -1;
			m_GeometryShaderhandle = -1;
			m_PixelShaderhandle = -1;
			//シェーダーに渡す追加パラメーターを配するハンドル
			for (auto& h : m_VertexShadercbhandle) { h = -1; }
			m_GeometryShaderMatcbhandle = -1;
			m_PixelShaderSendDispSizeHandle = -1;
			for (auto& h : m_PixelShadercbhandle) { h = -1; }
		}
		~ShaderUseClass() {
			Dispose();
		}
	public:
		//初期化
		void			Init(const char* VertexShader, const char* PixelShader) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			//頂点シェーダー周り
			for (auto& h : m_VertexShadercbhandle) {
				h = CreateShaderConstantBuffer(sizeof(float) * 4);
			}
			// 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列を設定するための定数バッファの作成
			for (auto& h : LightCameraMatrixConstantBufferHandle) {
				h = CreateShaderConstantBuffer(sizeof(LIGHTCAMERA_MATRIX));
			}
			this->m_VertexShaderhandle = LoadVertexShader(VertexShader);		// 頂点シェーダーバイナリコードの読み込み
			//ピクセルシェーダ―周り
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
		//後始末
		void			Dispose() noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			//頂点シェーダー周り
			for (auto& h : LightCameraMatrixConstantBufferHandle) {
				DeleteShaderConstantBuffer(h);
			}
			for (auto& h : m_VertexShadercbhandle) {
				DeleteShaderConstantBuffer(h);
			}
			DeleteShader(this->m_VertexShaderhandle);
			//
			DeleteShaderConstantBuffer(this->m_GeometryShaderMatcbhandle);
			DeleteShader(this->m_GeometryShaderhandle);
			//ピクセルシェーダ―周り
			DeleteShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);
			for (auto& h : m_PixelShadercbhandle) {
				DeleteShaderConstantBuffer(h);
			}
			DeleteShader(this->m_PixelShaderhandle);
		}
	public:
		//頂点シェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetVertexCameraMatrix(int Slot, const Matrix4x4DX& View, const Matrix4x4DX& Projection) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// 設定したカメラのビュー行列と射影行列を取得しておく
			LIGHTCAMERA_MATRIX* LightCameraMatrixConst = (LIGHTCAMERA_MATRIX*)GetBufferShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(Slot - 4));
			LightCameraMatrixConst->ViewMatrix = View.get();
			LightCameraMatrixConst->ProjectionMatrix = Projection.get();

			UpdateShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(Slot - 4));
			SetShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(Slot - 4), DX_SHADERTYPE_VERTEX, Slot);		// 影用深度記録画像を描画したときのカメラのビュー行列と射影行列を定数に設定する
		}
		//頂点シェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetVertexParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			FLOAT4* f4 = (FLOAT4 *)GetBufferShaderConstantBuffer(this->m_VertexShadercbhandle[0]);		// 頂点シェーダー用の定数バッファのアドレスを取得
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(this->m_VertexShadercbhandle[0]);								// 頂点シェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(this->m_VertexShadercbhandle[0], DX_SHADERTYPE_VERTEX, Slot);		// 頂点シェーダーの定数バッファを定数バッファレジスタ４にセット
		}
		//シェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetGeometryCONSTBUFFER(int Slot, const MATRIX* ViewMatrix, const MATRIX* ProjectionMatrix) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			if (this->m_GeometryShaderhandle == -1) { return; }
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
		//ピクセルシェーダ―のSlot番目のレジスタに情報をセット(Slot>=4)
		void			SetPixelCameraMatrix(int Slot, const Matrix4x4DX& View, const Matrix4x4DX& Projection) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			// 設定したカメラのビュー行列と射影行列を取得しておく
			LIGHTCAMERA_MATRIX* LightCameraMatrixConst = (LIGHTCAMERA_MATRIX*)GetBufferShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(Slot - 4));
			LightCameraMatrixConst->ViewMatrix = View.get();
			LightCameraMatrixConst->ProjectionMatrix = Projection.get();

			UpdateShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(Slot - 4));
			SetShaderConstantBuffer(LightCameraMatrixConstantBufferHandle.at(Slot - 4), DX_SHADERTYPE_PIXEL, Slot);		// 影用深度記録画像を描画したときのカメラのビュー行列と射影行列を定数に設定する
		}
		//ピクセルシェーダ―の2番目のレジスタに画面サイズの情報をセット
		void			SetPixelDispSize(int dispx, int dispy) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			FLOAT2* dispsize = (FLOAT2*)GetBufferShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);	// ピクセルシェーダー用の定数バッファのアドレスを取得
			dispsize->u = (float)dispx;
			dispsize->v = (float)dispy;
			UpdateShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle);									// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(this->m_PixelShaderSendDispSizeHandle, DX_SHADERTYPE_PIXEL, 2);				// ピクセルシェーダー用の定数バッファを定数バッファレジスタ2にセット
		}
		//ピクセルシェーダ―のSlot番目のレジスタに情報をセット(Slot>=3)
		void			SetPixelParam(int Slot, float param1, float param2, float param3, float param4) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(this->m_PixelShadercbhandle.at(Slot - 3));				// ピクセルシェーダー用の定数バッファのアドレスを取得
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(this->m_PixelShadercbhandle.at(Slot - 3));											// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(this->m_PixelShadercbhandle.at(Slot - 3), DX_SHADERTYPE_PIXEL, Slot);					// ピクセルシェーダー用の定数バッファを定数バッファレジスタ3にセット
		}
		//3D空間に適用する場合の関数(引数に3D描画のラムダ式を代入)
		void			Draw_lamda(std::function<void()> doing) noexcept {
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
			SetUseVertexShader(-1);
			SetUsePixelShader(-1);
			SetUseGeometryShader(-1);
		}
		//2D画像に適用する場合の関数
		void			Draw(ScreenVertex& Screenvertex) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) { return; }
			Draw_lamda([&] {DrawPolygon3DToShader(Screenvertex.GetScreenVertex(), 2); });
		}
	};
	//シェーダーパラメーター
	struct shaderparam {
		bool			use{ false };
		float			param[4]{ 0,0,0,0 };
	};
	//キューブマップ生成
	class RealTimeCubeMap {
	private:
		GraphHandle dynamicCubeTex;		// 周囲を回る小さいモデルたちを映りこませるための描画対象にできるキューブマップテクスチャ
		VECTOR lookAt[6]{};	// 映りこむ周囲の環境を描画する際のカメラの注視点
		VECTOR up[6]{};		// 移りこむ周囲の環境を描画する際のカメラの上方向
		int MIPLEVEL = 2;
	public:
		void Init() {
			// 描画対象にできるキューブマップテクスチャを作成
			SetCreateDrawValidGraphMipLevels(MIPLEVEL);
			SetCubeMapTextureCreateFlag(TRUE);
			dynamicCubeTex = GraphHandle::Make(512, 512, true);
			SetCubeMapTextureCreateFlag(FALSE);
			SetCreateDrawValidGraphMipLevels(0);
			// 映りこむ環境を描画する際に使用するカメラの注視点とカメラの上方向を設定
			lookAt[0] = VGet(1.0f, 0.0f, 0.0f);
			lookAt[1] = VGet(-1.0f, 0.0f, 0.0f);
			lookAt[2] = VGet(0.0f, 1.0f, 0.0f);
			lookAt[3] = VGet(0.0f, -1.0f, 0.0f);
			lookAt[4] = VGet(0.0f, 0.0f, 1.0f);
			lookAt[5] = VGet(0.0f, 0.0f, -1.0f);
			up[0] = VGet(0.0f, 1.0f, 0.0f);
			up[1] = VGet(0.0f, 1.0f, 0.0f);
			up[2] = VGet(0.0f, 0.0f, -1.0f);
			up[3] = VGet(0.0f, 0.0f, 1.0f);
			up[4] = VGet(0.0f, 1.0f, 0.0f);
			up[5] = VGet(0.0f, 1.0f, 0.0f);
		}

		void ReadyDraw(const Vector3DX& Pos, const std::function<void()>& Doing) {
			for (int i = 0; i < 6; i++) {		// 映りこむ環境を描画する面の数だけ繰り返し
				for (int j = 0; j < MIPLEVEL; j++) {			// ミップマップの数だけ繰り返し
					SetRenderTargetToShader(0, dynamicCubeTex.get(), i, j);		// 描画先番号０番の描画対象を描画対象にできるキューブマップのi番目の面に設定
					ClearDrawScreen();										// クリア
					{
						SetupCamera_Perspective(90.0f / 180.0f * DX_PI_F);								// カメラの画角は90度に設定
						SetCameraNearFar(0.5f*12.5f, 1000.0f*12.5f);									// Nearクリップ面とFarクリップ面の距離を設定
						SetCameraPositionAndTargetAndUpVec(Pos.get(), (Pos + lookAt[i]).get(), up[i]);	// カメラの位置と注視点、カメラの上方向を設定
						Doing();
					}
				}
			}
		}

		void Dispose() {
			dynamicCubeTex.Dispose();
		}

		const auto& GetCubeMapTex() const noexcept { return dynamicCubeTex; }
	};


	//--------------------------------------------------------------------------------------------------
	// 汎用セーブデータ
	//--------------------------------------------------------------------------------------------------
	typedef std::pair<std::string, int64_t> SaveParam;
	class SaveDataClass : public SingletonBase<SaveDataClass> {
	private:
		friend class SingletonBase<SaveDataClass>;
	private:
		std::vector<SaveParam> m_data;
	private:
		SaveDataClass() {
			Load();
		}
		~SaveDataClass() {}
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
			return (int64_t)-1;
		}
	public:
		void Save() noexcept {
			std::ofstream outputfile("Save/new.svf");
			for (auto& d : m_data) {
				outputfile << d.first + "=" + std::to_string(d.second) + "\n";
			}
			outputfile.close();
		}
		bool Load() noexcept {

			m_data.clear();

			std::ifstream inputputfile("Save/new.svf");
			if (!inputputfile) { return false; }
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
		void Reset() noexcept {
			m_data.clear();
		}
	};

	//--------------------------------------------------------------------------------------------------
	// サイドログ
	//--------------------------------------------------------------------------------------------------

	class SideLog : public SingletonBase<SideLog> {
	private:
		friend class SingletonBase<SideLog>;
	private:
		class SideLogData {
			unsigned int m_Color{0};
			char m_Message[64]{};
			float m_TimeMax{-1.f};
			float m_Time{-1.f};
			float m_Flip{0.f};
			float m_Flip_Y{0.f};
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
			void UpdateActive() noexcept;
		public:
			const float GetFlip() { return m_Flip_Y; }
			const float ActivePer() { return (m_Time > 1.f) ? std::clamp((m_TimeMax - m_Time)*5.f + 0.1f, 0.f, 1.f) : std::clamp(m_Time, 0.f, 1.f); }
			const char* GetMsg() { return m_Message; }
			const unsigned int GetMsgColor() { return m_Color; }
		};
	private:
		std::array<SideLogData, 16> data;
		int LastSel{0};
	public:
		template <typename... Args>
		void Add(float second, unsigned int Color, const char* Mes, Args&&... args) noexcept {
			for (auto& d : data) {
				d.AddFlip(1.f);
			}
			data.at(LastSel).SetData(second, Color, Mes, args...);
			++LastSel %= ((int)data.size());
		}
		void Update() noexcept {
			for (auto& d : data) {
				if (d.ActivePer() > 0.f) {
					d.UpdateActive();
				}
			}
		}
		void Draw() noexcept;
	};

	//--------------------------------------------------------------------------------------------------
	// ポップアップ
	//--------------------------------------------------------------------------------------------------
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

			std::function<void(int xmin, int ymin, int xmax, int ymax, bool EndSwitch)> m_Doing;
			std::function<void()> m_ExitDoing;
			std::function<void()> m_GuideDoing;
		public:
			PopUpDrawClass() {}
			~PopUpDrawClass() {}
		public:
			void			Set(const char* WindowName, int sizex, int sizey,
				std::function<void(int xmin, int ymin, int xmax, int ymax, bool EndSwitch)> doing,
				std::function<void()> ExitDoing,
				std::function<void()> GuideDoing
			) noexcept {
				sprintf_s(m_WindwoName, 64, WindowName);
				WinSizeX = sizex;
				WinSizeY = sizey;
				m_Doing = doing;
				m_ExitDoing = ExitDoing;
				m_GuideDoing = GuideDoing;
			}
			void			Start() noexcept;
			void			End() noexcept;
			void			Update() noexcept;
			void			Draw(int xcenter,int ycenter) noexcept;
		public:
			const auto IsEnd() const noexcept { return !m_Active && !(m_ActivePer > 1.f / 255.f); }
		};
	private:
		std::array<PopUpDrawClass, 24> que;
		int NowSel{ 0 };
		int LastSel{ 0 };
		bool PrevPause{false};
	public:
		const auto IsActivePop() const noexcept { return (NowSel != LastSel); }
	public:
		void Add(const char* WindowName, int sizex, int sizey,
					std::function<void(int xmin, int ymin, int xmax, int ymax, bool EndSwitch)> doing,
					std::function<void()> ExitDoing,
					std::function<void()> GuideDoing,
					bool IsInsert = false) noexcept;
		void EndAll() noexcept;
		void Update() noexcept;
		void Draw(int xcenter, int ycenter) noexcept {
			if (!IsActivePop()) { return; }
			que.at(NowSel).Draw(xcenter, ycenter);
		}
	};

	//--------------------------------------------------------------------------------------------------
	// PCの情報を走査
	//--------------------------------------------------------------------------------------------------
	class CheckPCSpec {
		struct MatchScore {
			std::string		m_Name;
			int				m_Score{0};
			int				m_HitCount{0};
		};
	private:
		bool IsEnd = false;
		std::vector<MatchScore> CPUResult;
		std::vector<MatchScore> GPUResult;

		TCHAR CPUString[256]{""};
		double FreeMemorySize{0};
		double TotalMemorySize{0};
		TCHAR GPUString[256]{""};

		std::array<std::string, 12> CPUStr;
		std::array<std::string, 12> GPUStr;
		std::thread m_thread;
	public:
		static void GetOnlyNumber(const char* Target, std::array<std::string, 12>* String) {
			int NumCount = 0;
			for (auto& s : *String) {
				s = "";
			}
			for (int i = 0;i < strlenDx(Target);i++) {
				if (!
					(
						Target[i] == ' ' ||
						Target[i] == '-'
						)
					) {
					String->at(NumCount) += Target[i];
				}
				else {
					if (String->at(NumCount) != "") {
						NumCount++;
						String->at(NumCount) = "";
					}
				}
			}
		};
	public:
		void FindCPU() {
			IsEnd = false;
			std::vector<MatchScore> Tmp;Tmp.reserve(64);
			{
				CPUResult.clear();
				std::ifstream inputputfile("data/PassMarkCPU.txt");
				if (inputputfile) {
					Tmp.clear();
					std::string ALL;
					std::array<std::string, 12> TmpString;
					int HitCount = 0;
					int border = 0;
					while (std::getline(inputputfile, ALL)) {
						if (ALL == "") { continue; }
						auto LEFT = getparams::getleft(ALL);
						auto RIGHT = getparams::getright(ALL);
						GetOnlyNumber(LEFT.c_str(), &TmpString);
						HitCount = 0;
						for (auto& s : CPUStr) {
							if (s == "") { continue; }
							for (auto& t : TmpString) {
								if (t == "") { continue; }
								if ((s.find(t) != std::string::npos) || (t.find(s) != std::string::npos)) {
									HitCount++;
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
				std::ifstream inputputfile("data/PassMarkGPU.txt");
				if (inputputfile) {
					Tmp.clear();
					std::string ALL;
					std::array<std::string, 12> TmpString;
					int HitCount = 0;
					int border = 0;
					while (std::getline(inputputfile, ALL)) {
						if (ALL == "") { continue; }
						auto LEFT = getparams::getleft(ALL);
						auto RIGHT = getparams::getright(ALL);
						GetOnlyNumber(LEFT.c_str(), &TmpString);
						HitCount = 0;
						for (auto& s : GPUStr) {
							if (s == "") { continue; }
							for (auto& t : TmpString) {
								if (t == "") { continue; }
								if ((s.find(t) != std::string::npos) || (t.find(s) != std::string::npos)) {
									HitCount++;
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
		CheckPCSpec() {

		}
		~CheckPCSpec() {
			if (m_thread.joinable()) {
				m_thread.detach();
			}
		}
	public:
		const auto* GetCPUDatas() const noexcept { return IsEnd ? &CPUResult : nullptr; }
		const auto* GetGPUDatas() const noexcept { return IsEnd ? &GPUResult : nullptr; }
		const auto& GetFreeMemorySize() const noexcept { return FreeMemorySize; }
		const auto& GetTotalMemorySize() const noexcept { return TotalMemorySize; }
	public:
		void Set() {
			GetPcInfo(NULL, NULL, CPUString, NULL, &FreeMemorySize, &TotalMemorySize, NULL, GPUString, NULL, NULL);
			GetOnlyNumber(CPUString, &CPUStr);
			GetOnlyNumber(GPUString, &GPUStr);
		}
		void StartSearch() {
			if (m_thread.joinable()) {
				m_thread.detach();
			}
			std::thread newThead(&CheckPCSpec::FindCPU, this);
			m_thread.swap(newThead);
		}
	};

	//--------------------------------------------------------------------------------------------------
	// 統一UI
	//--------------------------------------------------------------------------------------------------
	enum class EnumUIPartsType :int {
		Zero = 0,
		Box = Zero,
		Msg,
		Max,
	};
	static const char* g_UIPartsString[(int)EnumUIPartsType::Max] = {
		"Box",
		"Msg",
	};
	enum class LerpType {
		linear,
		pow2,
		Max
	};
	static const char* g_LerpTypeStr[(int)LerpType::Max] = {
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
			int			m_framepoint{ 0 };	//前回〜現在のフレーム数
			float		m_Alpha{ 1.f };		//上記のフレーム数の段階でのXXX
			float		m_XScale{ 1.f };
			float		m_YScale{ 1.f };
			float		m_XOfs{ 0.f };
			float		m_YOfs{ 0.f };
			float		m_ZRotOfs{ 0.f };
			LerpType	m_LerpType{ LerpType::linear };//前フレームとのつなぎ方
		};
		class UI_CommonParts {
		private://固定のパラメーター類
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
		private://変動する値
			bool			m_MouseOver{ false };
			bool			m_MousePress{ false };
			int				m_DrawXCenter{ 0 };
			int				m_DrawYCenter{ 0 };
		public://外からいじられるもの
			FrameInfo					m_FrameInfo{};
			std::array<std::string,3>	m_TextEX0{};
		public:
			const auto& GetUniqueID() const noexcept { return m_UniqueID; }
			const auto& GetName() const noexcept { return m_Name; }
			const auto& GetLayer() const noexcept { return m_Layer; }

			const auto& GetMousePress() const noexcept { return m_MousePress; }
		public:
			void SetUniqueID(int value) noexcept { m_UniqueID = value; }
			void SetFrameInfo(const FrameInfo& value) noexcept { m_FrameInfo = value; }
		public:
			void SetParts(const nlohmann::json& pJson) noexcept;
			void Update() noexcept;
			void Draw() noexcept;
		};
		class UI_CommonAnimes {
		private://固定のパラメーター類
			std::vector<int> m_TargetID{};//対照のパーツ
			std::vector<FrameInfo> m_AnimeFrame{};
		private://変動する値
			int m_NowAnim{ 0 };
		public://外からいじられるもの
			int m_Frame{ 0 };
		public:
		public:
			void SetParts(const nlohmann::json& pJson, const std::vector<std::unique_ptr<UI_CommonParts>>& Parts) noexcept;
			void Update(std::vector<std::unique_ptr<UI_CommonParts>>* Parts) noexcept;
		};

		class UI_OneLayer {
		private://パーツ
			std::vector<std::unique_ptr<UI_CommonParts>> m_CommonParts;
			int UniqueIDNum{ -1 };
		private://アニメーション
			std::vector<std::unique_ptr<UI_CommonAnimes>> m_CommonAnimes;
		private:
			bool m_IsEnd{ false };//アニメーション側からの終了命令
		public:
			const auto IsActive() const noexcept { return UniqueIDNum != -1; }
		public:
			void Load(const char* path) noexcept;
			void Update() noexcept;
			void Draw() noexcept;
			void Dispose() noexcept;
		};
	private:
		std::array<UI_OneLayer, 5> m_Layer;
	public:
		UISystem() noexcept {
		}
		~UISystem() noexcept {
		}
	public:
		int AddUI(const char* path) noexcept;
		void DelUI(int layer) noexcept;

		void Update() noexcept;
		void Draw() noexcept;
		void DisposeAll() noexcept;
	};
};
