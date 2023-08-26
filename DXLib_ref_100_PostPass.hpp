#pragma once
#include "DXLib_ref.h"

namespace DXLib_ref {
	//
	static const int EXTEND = 4;
	//�x�[�X
	class PostPassBase {
	protected:
	public:
		PostPassBase(void) {}
		virtual ~PostPassBase(void) {}
	protected:
		virtual void SetEffect_Sub(GraphHandle*) noexcept {}
	public:
		void SetEffect(GraphHandle* TargetGraph) noexcept { SetEffect_Sub(TargetGraph); }
	};
	//
	class PostPassEffect : public SingletonBase<PostPassEffect> {
	private:
		friend class SingletonBase<PostPassEffect>;
	private:

		std::vector<std::unique_ptr<PostPassBase>> m_PostPass;

		GraphHandle SkyScreen;		//��`��
		GraphHandle FarScreen_;		//�`��X�N���[��
		GraphHandle NearScreen_;	//�`��X�N���[��

		GraphHandle BackScreen;		//�`��X�N���[��

		GraphHandle MAIN_Screen;	//�`��X�N���[��

		GraphHandle ColorScreen_;	//�`��X�N���[��
		GraphHandle NormalScreen_;	//�`��X�N���[��
		GraphHandle DummyScreen_;	//�`��X�N���[��
		GraphHandle	DepthScreen;								//�[�x���擾����X�N���[��
		shaders		DepthDraw;									//�V�F�[�_�[
		shaders		Depth;										//�V�F�[�_�[

		int r_brt = 255;
		int g_brt = 255;
		int b_brt = 255;
	public:
		auto& Get_BUF_Screen(void) noexcept { return BackScreen; }
		auto& Get_MAIN_Screen(void) noexcept { return MAIN_Screen; }
	public:
		void Set_Bright(int r, int g, int b) {
			r_brt = std::clamp(r, 0, 255);
			g_brt = std::clamp(g, 0, 255);
			b_brt = std::clamp(b, 0, 255);
		}
	private:
		//��ʑ̐[�x�`��
		void near_dof(std::function<void()> sky_doing, std::function<void()> doing, const Camera3DInfo& cams, bool update_effekseer = true);
	private:
		PostPassEffect(void);
		~PostPassEffect(void) noexcept;
	public:
		//
		void BUF_Draw(std::function<void()> sky_doing, std::function<void()> doing, const Camera3DInfo& cams, bool update_effekseer = true);
		//
		void FlipBuftoMain(void) noexcept;
		//
		void SetPostpassEffect(void) noexcept;
		//�[�x��������
		void SetDepth(const Camera3DInfo& camInfo, std::function<void()> doing) noexcept;
		//�������񂾐[�x�ɉ����đΉ�
		void DrawByDepth(std::function<void()> doing) noexcept;
	};
};




