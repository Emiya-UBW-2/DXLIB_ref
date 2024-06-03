#include "DXLib_ref.h"
//#include "DXLib_ref_06_MV1Handle.hpp"

namespace DXLibRef {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	void MV1::ani::Update(const bool& loop, float speed) {
		auto* DrawParts = DXDraw::Instance();
		this->time += 30.f / DrawParts->GetFps()*speed;
		if (loop) {
			if (speed >= 0.f) {
				if (this->time >= this->alltime) {
					this->time = 0.f;
				}
			}
			else {
				if (this->time <= 0.f) {
					this->time = this->alltime;
				}
			}
		}
	}
};
