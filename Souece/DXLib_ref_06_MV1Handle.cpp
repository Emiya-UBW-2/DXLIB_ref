#include "DXLib_ref.h"
// #include "DXLib_ref_06_MV1Handle.hpp"

namespace DXLibRef {
	// --------------------------------------------------------------------------------------------------
	// 
	// --------------------------------------------------------------------------------------------------
	void MV1::AnimControler::Update(bool loop, float speed) noexcept {
		auto* DXLib_refParts = DXLib_ref::Instance();
		this->m_time += 30.f * DXLib_refParts->GetDeltaTime() * speed;
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
