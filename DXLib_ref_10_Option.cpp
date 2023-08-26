#include "DXLib_ref_10_Option.hpp"

namespace DXLib_ref {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	const OPTION* SingletonBase<OPTION>::m_Singleton = nullptr;

	void			OPTION::Load(void) noexcept {
		SetOutApplicationLogValidFlag(FALSE);
		int mdata = FileRead_open("data/Setting.txt", FALSE);
		grass_level = std::clamp<int>(getparams::_int(mdata), 0, 4);
		DoF = getparams::_bool(mdata);
		Bloom = getparams::_bool(mdata);
		Shadow = getparams::_bool(mdata);
		useVR = getparams::_bool(mdata);
		SSAO = getparams::_bool(mdata);
		Fov = getparams::_float(mdata);
		Vsync = getparams::_bool(mdata);
		SE = getparams::_float(mdata);
		VOICE = getparams::_float(mdata);
		BGM = getparams::_float(mdata);
		AllWaysFront = getparams::_bool(mdata);
		aberration = getparams::_bool(mdata);
		{
			std::string DctXVer = getparams::_str(mdata);
			if (DctXVer == "9") {
				DirectXVer = DX_DIRECT3D_9;
			}
			else if (DctXVer == "9c") {
				DirectXVer = DX_DIRECT3D_9EX;
			}
			else if (DctXVer == "11") {
				DirectXVer = DX_DIRECT3D_11;
			}
		}

		FileRead_close(mdata);
		SetOutApplicationLogValidFlag(TRUE);
	}
	void			OPTION::Save(void) noexcept {
		std::ofstream outputfile("data/setting.txt");
		outputfile << "grass_level=" + std::to_string(grass_level) + "\n";
		outputfile << "DoF=" + std::string((DoF) ? "true" : "false") + "\n";
		outputfile << "bloom=" + std::string((Bloom) ? "true" : "false") + "\n";
		outputfile << "shadow=" + std::string((Shadow) ? "true" : "false") + "\n";
		outputfile << "usevr=" + std::string((useVR) ? "true" : "false") + "\n";
		outputfile << "SSAO=" + std::string((SSAO) ? "true" : "false") + "\n";
		outputfile << "fov=" + std::to_string(Fov) + "\n";
		outputfile << "vsync=" + std::string((Vsync) ? "true" : "false") + "\n";
		outputfile << "SE=" + std::to_string(SE) + "\n";
		outputfile << "VOICE=" + std::to_string(VOICE) + "\n";
		outputfile << "BGM=" + std::to_string(BGM) + "\n";
		outputfile << "AllWaysFront=" + std::string((AllWaysFront) ? "true" : "false") + "\n";
		outputfile << "aberration=" + std::string((aberration) ? "true" : "false") + "\n";
		switch (DirectXVer) {
		case  DX_DIRECT3D_9:
			outputfile << "DirectXVer=" + std::string("9") + "\n";
			break;
		case  DX_DIRECT3D_9EX:
			outputfile << "DirectXVer=" + std::string("9c") + "\n";
			break;
		case  DX_DIRECT3D_11:
			outputfile << "DirectXVer=" + std::string("11") + "\n";
			break;
		default:
			break;
		}
		outputfile.close();
	}
};
