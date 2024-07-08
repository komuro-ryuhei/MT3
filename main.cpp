#include <Novice.h>

#include "MyMath.h"
#include "MT3.h"

#include <ImGui.h>

const char kWindowTitle[] = "LC1B_13_コムロ_リュウヘイ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	struct Spring {
		// アンカー
		Vector3 ancor;
		float naturalLength; // 自然長
		float stiffness;     // 剛性、ばね定数k
	};

	Spring spring{};
	spring.ancor = { 0.0f,0.0f,0.0f };
	spring.naturalLength = 1.0f;
	spring.stiffness = 100.0f;

	struct Ball {
		Vector3 position;    // ボールの位置
		Vector3 velocity;    // ボールの速度
		Vector3 cceleration; // ボールの加速度
		float mass;          // ボールの質量
		float radius;        // ボールの半径
		unsigned int color;  // ボールの色
	};

	Ball ball{};
	ball.position = { 1.2f,0.0f,0.0f };
	ball.mass = 2.0f;
	ball.radius = 0.05f;
	ball.color = 0x0000FFFF;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		Vector3 diff = ball.position - spring.ancor;
		float length = Length(diff);
		if (length != 0.0f) {

			Vector3 direction = Normalize(diff);
			Vector3 restPosition = spring.ancor + direction * spring.naturalLength;
			Vector3 displacement = length * (ball.position - restPosition);
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
