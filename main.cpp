#include <Novice.h>
#include "MyMath.h"
#include "MT3.h"
#include <ImGui.h>
#include <cmath>

const char kWindowTitle[] = "LC1B_13_コムロ_リュウヘイ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	float rotateRadius = 0.8f;      // 回転する円の半径
	float angularVelocity = 3.14f;  // 角速度
	float angle = 0.0f;             // 角度
	float deltaTime = 1.0f / 60.0f; // デルタタイム

	Vector3 centerPos = { 0.0f, 0.0f, 0.0f }; // 回転中心点

	Sphere sphere;
	sphere.center = { centerPos.x + rotateRadius * std::cos(angle), centerPos.y + rotateRadius * std::sin(angle), centerPos.z };
	sphere.radius = 0.05f;

	bool isRotate = false;

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

		CaluculationMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });

		if (isRotate) {
			angle += angularVelocity * deltaTime;

			sphere.center.x = centerPos.x + rotateRadius * std::cos(angle);
			sphere.center.y = centerPos.y + rotateRadius * std::sin(angle);
		}

		ImGui::Checkbox("IsRotate", &isRotate);

		ImGui::DragFloat3("pos", &sphere.center.x, 0.01f);

		ImGui::Begin("camera");
		ImGui::DragFloat3("CameraPosition", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::End();

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawSphere(sphere, worldViewProjectionMatrix, worldViewportMatrix, 0xFFFFFFFF);
		DrawGrid(worldViewProjectionMatrix, worldViewportMatrix);

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
