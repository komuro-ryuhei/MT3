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

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		Vector3 rotate1{ 0.0f,0.0f,0.0f };
		Vector3 rotate2{ -0.05f,-2.49f,0.15f };

		OBB obb1{
			.center{0.0f,0.0f,0.0f},
			.orientations{{1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},
			{0.0f,0.0f,1.0f}},
			.size{0.83f,0.26f,0.24f}
		};
		OBB obb2{
			.center{0.9f,0.66f,0.78f},
			.orientations{{1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},
			{0.0f,0.0f,1.0f}},
			.size{0.5f,0.37f,0.5f}
		};


		///
		/// ↓更新処理ここから
		///

		ImGui::Begin("OBB");

		ImGui::DragFloat3("obb1.center", &obb1.center.x, 0.01f);
		ImGui::SliderAngle("obb1.rotateX", &rotate1.x);
		ImGui::SliderAngle("obb1.rotateY", &rotate1.y);
		ImGui::SliderAngle("obb1.rotateZ", &rotate1.z);

		ImGui::DragFloat3("obb2.center", &obb2.center.x, 0.01f);
		ImGui::SliderAngle("obb2.rotateX", &rotate2.x);
		ImGui::SliderAngle("obb2.rotateY", &rotate2.y);
		ImGui::SliderAngle("obb2.rotateZ", &rotate2.z);

		ImGui::End();

		CaluculationMatrix();


		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawOBB(obb1, worldViewProjectionMatrix, worldViewportMatrix, WHITE);
		DrawOBB(obb2, worldViewProjectionMatrix, worldViewportMatrix, WHITE);

		DrawGrid(worldViewProjectionMatrix,worldViewportMatrix);

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
