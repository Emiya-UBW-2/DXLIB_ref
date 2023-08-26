#pragma once
#include "DXLib_ref.h"

//リサイズ
#define x_r(p1) (int(p1) * DXDraw::Instance()->m_DispXSize / 1920)
#define y_r(p1) (int(p1) * DXDraw::Instance()->m_DispYSize / 1080)

namespace DXLib_ref {
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*関数																																		*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	//Matrix版のイージング
	static void Easing_Matrix(MATRIX_ref* A, const MATRIX_ref& B, float ratio, EasingType EasingType) noexcept {
		VECTOR_ref basex = A->xvec();
		VECTOR_ref basey = A->yvec();
		VECTOR_ref basez = A->zvec();
		Easing(&basex, B.xvec(), ratio, EasingType);
		Easing(&basey, B.yvec(), ratio, EasingType);
		Easing(&basez, B.zvec(), ratio, EasingType);

		*A = MATRIX_ref::Axis1(
			basex.Norm(),
			basey.Norm(),
			basez.Norm()).GetRot();
	}
	//Matrix版の線形補完
	static MATRIX_ref Lerp_Matrix(const MATRIX_ref& A, const MATRIX_ref& B, float Per) noexcept {
		return MATRIX_ref::Axis1(
			Lerp(A.xvec(), B.xvec(), Per).Norm(),
			Lerp(A.yvec(), B.yvec(), Per).Norm(),
			Lerp(A.zvec(), B.zvec(), Per).Norm());
	}
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

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*DXLIBラッパー																																*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	//--------------------------------------------------------------------------------------------------
	// 演算補助
	//--------------------------------------------------------------------------------------------------
	//三角とカプセルとの判定
	static bool GetHitCapsuleToTriangle(const VECTOR_ref& startpos, const VECTOR_ref& endpos, float size, const VECTOR_ref& tri_p1, const VECTOR_ref& tri_p2, const VECTOR_ref& tri_p3) {
		return HitCheck_Capsule_Triangle(startpos.get(), endpos.get(), size, tri_p1.get(), tri_p2.get(), tri_p3.get()) == TRUE;
	}
	//直線と点の一番近い点
	static float GetMinLenSegmentToPoint(const VECTOR_ref& startpos, const VECTOR_ref& endpos, const VECTOR_ref& tgt) {
		return Segment_Point_MinLength(startpos.get(), endpos.get(), tgt.get());
	}
	//平面と点の一番近い点
	static VECTOR_ref GetMinPosSegmentToPoint(const VECTOR_ref& startpos, const VECTOR_ref& endpos, const VECTOR_ref& tgt) {
		return Plane_Point_MinLength_Position(startpos.get(), endpos.get(), tgt.get());
	}
	//ランダム
	static float GetRandf(float m_arg) noexcept { return -m_arg + (float)(GetRand((int)(m_arg * 2.f * 10000.f))) / 10000.f; }
	//線分同士の交差判定
	static bool GetSegmenttoSegment(const VECTOR_ref &SegmentAPos1, const VECTOR_ref &SegmentAPos2, const VECTOR_ref &SegmentBPos1, const VECTOR_ref &SegmentBPos2, SEGMENT_SEGMENT_RESULT *Result) noexcept {
		VECTOR Pos1t = SegmentAPos1.get();
		VECTOR Pos2t = SegmentAPos2.get();
		VECTOR PosAt = SegmentBPos1.get();
		VECTOR PosBt = SegmentBPos2.get();

		Segment_Segment_Analyse(&Pos1t, &Pos2t, &PosAt, &PosBt, Result);
		float len = 0.001f;
		return (Result->SegA_SegB_MinDist_Square <= (len*len));
	}
	//線分と点の最接近点情報を解析する
	static bool GetSegmenttoPoint(const VECTOR_ref &SegmentAPos1, const VECTOR_ref &SegmentAPos2, const VECTOR_ref &PointPos, SEGMENT_POINT_RESULT *Result) noexcept {
		VECTOR Pos1t = SegmentAPos1.get();
		VECTOR Pos2t = SegmentAPos2.get();
		VECTOR PosAt = PointPos.get();

		Segment_Point_Analyse(&Pos1t, &Pos2t, &PosAt, Result);
		float len = 0.001f;
		return (Result->Seg_Point_MinDist_Square <= (len*len));
	}
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
	static void DrawEdgeBox_2D(int p1x, int p1y, int p2x, int p2y, const unsigned int& color, const unsigned int& EdgeColor) noexcept {
		int p = -1;
		DrawBox(p1x + p, p1y + p, p2x - p, p2y - p, EdgeColor, FALSE);
		p = 0;
		DrawBox(p1x + p, p1y + p, p2x - p, p2y - p, color, FALSE);
		p = 1;
		DrawBox(p1x + p, p1y + p, p2x - p, p2y - p, EdgeColor, FALSE);
	}
	//縁ぬき四角
	static void DrawBoxLine_2D(int p1x, int p1y, int p2x, int p2y, const unsigned int& color, int thickness = 1) noexcept {
		DxLib::DrawLine(p1x, p1y, p1x, p2y, color, thickness);
		DxLib::DrawLine(p1x, p1y, p2x, p1y, color, thickness);
		DxLib::DrawLine(p1x, p2y, p2x, p2y, color, thickness);
		DxLib::DrawLine(p2x, p1y, p2x, p2y, color, thickness);
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
	//カプセル描画
	static bool DrawCapsule_3D(const VECTOR_ref& p1, const VECTOR_ref& p2, float range, const unsigned int& color, const unsigned int& speccolor) noexcept {
		return DxLib::DrawCapsule3D(p1.get(), p2.get(), range, 8, color, speccolor, TRUE) == TRUE;
	}
	//球描画
	static bool DrawSphere_3D(const VECTOR_ref& p1, float range, const unsigned int& color, const unsigned int& speccolor) noexcept {
		return DxLib::DrawSphere3D(p1.get(), range, 8, color, speccolor, TRUE) == TRUE;
	}

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
		VECTOR_ref	m_WorldPos;
		VECTOR_ref	m_LocalPos;
	public:
		void			operator=(const frames& tgt) noexcept {
			this->m_FrameID = tgt.m_FrameID;
			this->m_WorldPos = tgt.m_WorldPos;
			this->m_LocalPos = tgt.m_LocalPos;
		}
		void			Set(int i, const MV1& obj) noexcept {
			m_FrameID = i;
			m_WorldPos = obj.frame(i);
			if (obj.frame_parent(i) >= 0) {
				m_LocalPos = obj.frame(i) - obj.frame((int)obj.frame_parent(i));
			}
			else {
				m_LocalPos = obj.frame(i);//
			}
		}
		const auto&		GetFrameID() const noexcept { return m_FrameID; }
		const auto&		GetFrameWorldPosition() const noexcept { return m_WorldPos; }
		const auto&		GetFrameLocalPosition() const noexcept { return m_LocalPos; }
	};
	//位置情報
	class moves {
	public:
		VECTOR_ref pos;		//座標
		VECTOR_ref repos;	//前フレームの座標
		VECTOR_ref vec;		//加速
		MATRIX_ref mat;		//回転

		const MATRIX_ref MatIn(void) const noexcept { return mat * MATRIX_ref::Mtrans(pos); }

		void			operator=(const moves&tgt) {
			this->pos = tgt.pos;
			this->repos = tgt.repos;
			this->vec = tgt.vec;
			this->mat = tgt.mat;
		}

		void			SetPos(const VECTOR_ref& tgt) {
			this->repos = this->pos;
			this->pos = tgt;
		}
		void			Update_Physics(float speed_randam = 0.f, float rate = 1.f) {
			this->pos += this->vec*((float)((1000 - int(1000.f*speed_randam)) + GetRand(int(1000.f*speed_randam) * 2)) / 1000.f);
			this->vec.yadd(M_GR / powf((GetFPS() / rate), 2.f));

			//this->gun_m.pos += this->gun_m.vec;
			//this->gun_m.vec.yadd(M_GR / std::powf(GetFPS(), 2.f));
		}

		void			HitGround(const MV1_COLL_RESULT_POLY& colres, float hight) {//0.005f
			this->pos = VECTOR_ref(colres.HitPosition) + VECTOR_ref(colres.Normal)*hight;
			this->mat *= MATRIX_ref::RotVec2(this->mat.xvec(), VECTOR_ref(colres.Normal)*-1.f);
		}
	};

	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	/*シェーダー																																*/
	/*------------------------------------------------------------------------------------------------------------------------------------------*/
	//シェーダー
	class shaders {
	public:
		class shader_Vertex {
			VERTEX3DSHADER Screen_vertex[6] = { 0.0f };
		public:
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
	private:
		int m_VertexShaderhandle{ -1 };
		int m_VertexShadercbhandle{ -1 };

		int m_PixelShaderhandle{ -1 };
		int m_PixelShadercbhandle{ -1 };
		int m_PixelShadercbhandle2{ -1 };
	public:
		void			Init(std::string VertexShader, std::string PixelShader) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				return;
			}
			this->m_VertexShadercbhandle = CreateShaderConstantBuffer(sizeof(float) * 4);
			this->m_VertexShaderhandle = LoadVertexShader(("shader/" + VertexShader).c_str());		// 頂点シェーダーバイナリコードの読み込み
			this->m_PixelShadercbhandle = CreateShaderConstantBuffer(sizeof(float) * 4);
			this->m_PixelShadercbhandle2 = CreateShaderConstantBuffer(sizeof(float) * 4);
			this->m_PixelShaderhandle = LoadPixelShader(("shader/" + PixelShader).c_str());			// ピクセルシェーダーバイナリコードの読み込み
		}
		//
		void			SetVertexParam(float param1, float param2, float param3, float param4) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				return;
			}
			FLOAT4* f4 = (FLOAT4 *)GetBufferShaderConstantBuffer(this->m_VertexShadercbhandle);		// 頂点シェーダー用の定数バッファのアドレスを取得
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(this->m_VertexShadercbhandle);								// 頂点シェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(this->m_VertexShadercbhandle, DX_SHADERTYPE_VERTEX, 4);			// 頂点シェーダーの定数バッファを定数バッファレジスタ４にセット
		}
		//
		void			SetPixelDispSize(int dispx, int dispy) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				return;
			}
			FLOAT2* dispsize = (FLOAT2*)GetBufferShaderConstantBuffer(this->m_PixelShadercbhandle);	// ピクセルシェーダー用の定数バッファのアドレスを取得
			dispsize->u = (float)dispx;
			dispsize->v = (float)dispy;
			UpdateShaderConstantBuffer(this->m_PixelShadercbhandle);								// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(this->m_PixelShadercbhandle, DX_SHADERTYPE_PIXEL, 2);			// ピクセルシェーダー用の定数バッファを定数バッファレジスタ2にセット
		}
		void			SetPixelParam(float param1, float param2, float param3, float param4) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				return;
			}
			FLOAT4* f4 = (FLOAT4*)GetBufferShaderConstantBuffer(this->m_PixelShadercbhandle2);		// ピクセルシェーダー用の定数バッファのアドレスを取得
			f4->x = param1;
			f4->y = param2;
			f4->z = param3;
			f4->w = param4;
			UpdateShaderConstantBuffer(this->m_PixelShadercbhandle2);								// ピクセルシェーダー用の定数バッファを更新して書き込んだ内容を反映する
			SetShaderConstantBuffer(this->m_PixelShadercbhandle2, DX_SHADERTYPE_PIXEL, 3);			// ピクセルシェーダー用の定数バッファを定数バッファレジスタ3にセット
		}

		void			Draw_lamda(std::function<void()> doing) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				doing();
				return;
			}
			SetUseVertexShader(this->m_VertexShaderhandle);											// 使用する頂点シェーダーをセット
			SetUsePixelShader(this->m_PixelShaderhandle);											// 使用するピクセルシェーダーをセット
			MV1SetUseOrigShader(TRUE);
			doing();
			MV1SetUseOrigShader(FALSE);
			SetUseVertexShader(-1);
			SetUsePixelShader(-1);
		}
		void			Draw(shader_Vertex& Screenvertex) noexcept {
			if (GetUseDirect3DVersion() != DX_DIRECT3D_11) {
				return;
			}
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
		int dynamicCubeTex;		// 周囲を回る小さいモデルたちを映りこませるための描画対象にできるキューブマップテクスチャ
		VECTOR lookAt[6];	// 映りこむ周囲の環境を描画する際のカメラの注視点
		VECTOR up[6];		// 移りこむ周囲の環境を描画する際のカメラの上方向
		int MIPLEVEL = 2;
	public:
		void Init() {
			// 描画対象にできるキューブマップテクスチャを作成
			SetCreateDrawValidGraphMipLevels(MIPLEVEL);
			SetCubeMapTextureCreateFlag(TRUE);
			dynamicCubeTex = MakeScreen(512, 512, TRUE);
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

		void ReadyDraw(const VECTOR_ref& Pos, const std::function<void()>& Doing) {
			for (int i = 0; i < 6; i++) {		// 映りこむ環境を描画する面の数だけ繰り返し
				for (int j = 0; j < MIPLEVEL; j++) {			// ミップマップの数だけ繰り返し
					SetRenderTargetToShader(0, dynamicCubeTex, i, j);		// 描画先番号０番の描画対象を描画対象にできるキューブマップのi番目の面に設定
					ClearDrawScreen();										// クリア
					{
						SetupCamera_Perspective(90.0f / 180.0f * DX_PI_F);								// カメラの画角は90度に設定
						SetCameraNearFar(0.5f, 1000.0f);												// Nearクリップ面とFarクリップ面の距離を設定
						SetCameraPositionAndTargetAndUpVec(Pos.get(), (Pos + lookAt[i]).get(), up[i]);	// カメラの位置と注視点、カメラの上方向を設定
						Doing();
					}
				}
			}
		}

		auto GetCubeMapTex() { return dynamicCubeTex; }
	};
};
