#pragma once
#include "DXLib_ref.h"

#if defined(_USE_BOX2D_)

// Box2D拡張
namespace std {
	template <>
	struct default_delete<b2Body> {
		void		operator()(b2Body* body) const noexcept {
			body->GetWorld()->DestroyBody(body);
		}
	};
}; // namespace std
// 
namespace DXLibRef {
	// BOX2D
	static auto* CreateB2Body(std::shared_ptr<b2World>& world, b2BodyType type, float32 x_, float32 y_, float angle = 0) noexcept {
		b2BodyDef f_bodyDef;
		f_bodyDef.type = type;
		f_bodyDef.position.Set(x_, y_);
		f_bodyDef.angle = angle;
		return world->CreateBody(&f_bodyDef);
	}
	class b2Pats {
	private:
		b2FixtureDef				fixtureDef;				// 動的ボディフィクスチャを定義します
		std::unique_ptr<b2Body>		body;					// 
		b2Fixture* playerfix{ nullptr };	// 
	public:
		Vector3DX					pos;// 仮
	public:
		void		Set(b2Body* body_ptr, b2Shape* dynamicBox) noexcept {
			fixtureDef.shape = dynamicBox;								// 
			fixtureDef.density = 1.0f;									// ボックス密度をゼロ以外に設定すると、動的になる
			fixtureDef.friction = 0.3f;									// デフォルトの摩擦をオーバーライド
			this->body.reset(body_ptr);									// 
			this->playerfix = this->body->CreateFixture(&fixtureDef);	// シェイプをボディに追加
		}

		void		SetLinearVelocity(const b2Vec2& position) noexcept {
			this->body->SetLinearVelocity(position);
		}

		void		Update(const Vector3DX& add, float yradadd) noexcept {
			this->body->SetLinearVelocity(b2Vec2(add.x, add.z));
			this->body->SetAngularVelocity(yradadd);
		}


		void		Dispose(void) noexcept {
			if (this->playerfix != nullptr) {
				delete this->playerfix->GetUserData();
				this->playerfix->SetUserData(nullptr);
			}
		}

		const auto	Get(void) noexcept { return body.get(); }
		const auto	Pos(void) noexcept { return body->GetPosition(); }
		const auto	Rad(void) noexcept { return body->GetAngle(); }
		const auto	Speed(void) noexcept { return std::hypot(this->body->GetLinearVelocity().x, this->body->GetLinearVelocity().y); }
		void		SetTransform(const b2Vec2& position, float32 angle) noexcept {
			body->SetTransform(position, angle);
		}
	};
};

#endif // _USE_BOX2D_
