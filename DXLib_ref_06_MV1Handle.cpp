#include "DXLib_ref.h"
//#include "DXLib_ref_06_MV1Handle.hpp"

namespace DXLibRef {
	//--------------------------------------------------------------------------------------------------
	//
	//--------------------------------------------------------------------------------------------------
	void MV1::AnimControler::Update(const bool& loop, float speed) noexcept {
		auto* DrawParts = DXDraw::Instance();
		this->m_time += 30.f / DrawParts->GetFps() * speed;
		if (loop) {
			if (speed >= 0.f) {
				if (this->m_time >= this->m_AllTime) {
					this->m_time = 0.f;
				}
			}
			else {
				if (this->m_time <= 0.f) {
					this->m_time = this->m_AllTime;
				}
			}
		}
		else {
			if (speed >= 0.f) {
				if (this->m_time >= this->m_AllTime) {
					this->m_time = this->m_AllTime;
				}
			}
			else {
				if (this->m_time <= 0.f) {
					this->m_time = 0.f;
				}
			}
		}
	}
};
