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

	Vector3 translates[3] = {
		{0.2f,1.0f,0.0f},
		{0.4f,0.0f,0.0f},
		{0.3f,0.0f,0.0f},
	};

	Vector3 rotates[3] = {
		{0.0f,0.0f,-6.8f},
		{0.0f,0.0f,-1.4f},
		{0.0f,0.0f,0.0f},
	};

	Vector3 scales[3] = {
		{1.0f,1.0f,1.0f},
		{1.0f,1.0f,1.0f},
		{1.0f,1.0f,1.0f},
	};

	Sphere sphere[3];

	for (int i = 0; i < 3; i++) {

		sphere[i].radius = 0.1f;
	}

	sphere[0].color = 0xFF0000FF;
	sphere[1].color = 0x00FF00FF;
	sphere[2].color = 0x0000FFFF;

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

		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });

		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraScale, cameraRotate, cameraTranslate);
		Matrix4x4 worldViewMatrix = Inverse4x4(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);

		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(worldViewMatrix, projectionMatrix));


		Matrix4x4 sholderWorldMatirx = MakeAffineMatrix(scales[0], rotates[0], translates[0]);
		Matrix4x4 elbowWorldMatirx = MakeAffineMatrix(scales[1], rotates[1], translates[1]) * sholderWorldMatirx;
		Matrix4x4 handWorldMatirx = MakeAffineMatrix(scales[2], rotates[2], translates[2]) * elbowWorldMatirx;

		Matrix4x4 sholderWvpMatrix = Multiply(sholderWorldMatirx, Multiply(worldViewMatrix, projectionMatrix));
		Matrix4x4 elbowWvpMatrix = Multiply(elbowWorldMatirx, Multiply(worldViewMatrix, projectionMatrix));
		Matrix4x4 handWvpMatrix = Multiply(handWorldMatirx, Multiply(worldViewMatrix, projectionMatrix));


		Matrix4x4 sholderLineWorldMatrix = MakeAffineMatrix({ 0.1f,0.1f,0.1f }, { 0.0f,0.0f,0.0f }, translates[0]);
		Matrix4x4 elbowLineWorldMatrix = MakeAffineMatrix({ 0.1f,0.1f,0.1f }, { 0.0f,0.0f,0.0f }, translates[1]);
		Matrix4x4 handLineWorldMatrix = MakeAffineMatrix({ 0.1f,0.1f,0.1f }, { 0.0f,0.0f,0.0f }, translates[2]);

		Matrix4x4 sholderLineWvpMatrix = Multiply(sholderLineWorldMatrix, Multiply(worldViewMatrix, projectionMatrix));
		Matrix4x4 elbowLineWvpMatrix = Multiply(elbowLineWorldMatrix, Multiply(worldViewMatrix, projectionMatrix));
		Matrix4x4 handLineWvpMatrix = Multiply(handLineWorldMatrix, Multiply(worldViewMatrix, projectionMatrix));

		Matrix4x4 worldViewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		for (int i = 0; i < 3; i++) {

			sphere[i].center = translates[i];
		}


		ImGui::DragFloat3("camera.translate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("camera.rotate", &cameraRotate.x, 0.01f);

		ImGui::Text("\n");

		ImGui::DragFloat3("sholder.translates", &translates[0].x, 0.01f);
		ImGui::DragFloat3("sholder.rotates", &rotates[0].x, 0.01f);
		ImGui::DragFloat3("elbow.translates", &translates[1].x, 0.01f);
		ImGui::DragFloat3("elbow.rotates", &rotates[1].x, 0.01f);
		ImGui::DragFloat3("hand.translates", &translates[2].x, 0.01f);
		ImGui::DragFloat3("hand.rotates", &rotates[2].x, 0.01f);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(worldViewProjectionMatrix, worldViewportMatrix);

		DrawSphere(sphere[0], sholderWvpMatrix, worldViewportMatrix, sphere[0].color);
		DrawSphere(sphere[1], elbowWvpMatrix, worldViewportMatrix, sphere[1].color);
		DrawSphere(sphere[2], handWvpMatrix, worldViewportMatrix, sphere[2].color);

		DrawLine(sphere[0].center, sphere[1].center, sholderWvpMatrix, worldViewportMatrix, 0xFFFFFFFF);
		DrawLine(sphere[1].center, sphere[2].center, elbowWvpMatrix, worldViewportMatrix, 0xFFFFFFFF);

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
