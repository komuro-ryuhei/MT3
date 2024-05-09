#include <Novice.h>

#include <Vector3.h>
#include <ImGui.h>

#include "MyMath.h"

const char kWindowTitle[] = "LC1B_13_コムロ_リュウヘイ";

bool IsCollision(const Sphere& sphere1, const Sphere& sphere2) {

	// 2つの球の中心点間の距離を求める
	float distance = Length(sphere2.center - sphere1.center);
	// 半径の合計よりも短ければ衝突
	if (distance <= sphere1.radius + sphere2.radius) {
		return true;
	}
	return false;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Vector3 rotate{ 0.0f,0.0f,0.0f };
	Vector3 translate{ 0.0f,0.0f,0.0f };

	Vector3 cameraScale{ 1.0f, 1.0f, 1.0f };
	Vector3 cameraRotate{ 0.26f, 0.0f, 0.0f };
	Vector3 cameraTranslate{ 0.0f, 1.9f, -6.49f };

	int kWindowWidth = 1280;
	int kWindowHeight = 720;


	Sphere sphere[2];
	sphere[0].center = {-1.0f,0.0f,0.0f};
	sphere[0].radius = 1.0f;
	sphere[0].color = 0xFFFFFFFF;

	sphere[1].center = { 1.0f,0.0f,0.0f };
	sphere[1].radius = 1.0f;
	sphere[1].color = 0xFFFFFFFF;

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

		ImGui::Begin("Window");

		ImGui::DragFloat3("Sphere[0].center", &sphere[0].center.x, 0.01f);
		ImGui::DragFloat("Sphere[0].Radius", &sphere[0].radius, 0.01f);

		ImGui::DragFloat3("Sphere[1].center", &sphere[1].center.x, 0.01f);
		ImGui::DragFloat("Sphere[1].Radius", &sphere[1].radius, 0.01f);

		ImGui::End();

		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0,1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraScale, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse4x4(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		if (IsCollision(sphere[0],sphere[1])) {
			sphere[0].color = 0xFF0000FF;
		} else {
			sphere[0].color = 0xFFFFFFFF;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(worldViewProjectionMatrix, viewportMatrix);

		DrawSphere(sphere[0], worldViewProjectionMatrix, viewportMatrix, sphere[0].color);
		DrawSphere(sphere[1], worldViewProjectionMatrix, viewportMatrix, sphere[1].color);

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
