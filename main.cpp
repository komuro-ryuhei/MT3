#include <Novice.h>

#include "MyMath.h"
#include "MT3.h"

#include <algorithm>
#include <ImGui.h>

const char kWindowTitle[] = "LC1B_13_コムロ_リュウヘイ";


bool IsCollision(const AABB& aabb, const Sphere& sphere) {

	// 最近接点を求める
	Vector3 closestpoint{
		std::clamp(sphere.center.x,aabb.min.x,aabb.max.x),
		std::clamp(sphere.center.y,aabb.min.y,aabb.max.y),
		std::clamp(sphere.center.z,aabb.min.z,aabb.max.z),
	};

	float distance = Length(closestpoint - sphere.center);
	// 距離が半径よりも小さいと
	if (distance <= sphere.radius) {

		return true;
	} else {

		return false;
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };


	int kWindowWidth = 1280;
	int kWindowHeight = 720;


	Vector3 rotate{ 0.0f,0.0f,0.0f };
	Vector3 translate{ 0.0f,0.0f,0.0f };

	Vector3 cameraScale{ 1.0f, 1.0f, 1.0f };
	Vector3 cameraRotate{ 0.26f, 0.0f, 0.0f };
	Vector3 cameraTranslate{ 0.0f, 1.9f, -6.49f };


	Sphere sphere;
	sphere.center = { -1.0f,1.0f,-1.0f };
	sphere.radius = 1.0f;

	AABB aabb{
		.min{0.5f,-0.5f,-0.5f},
		.max{2.0f,1.0f,1.0f},
	};

	unsigned color = 0xFFFFFFFF;

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

		aabb.min.x = (std::min)(aabb.min.x, aabb.max.x);
		aabb.max.x = (std::max)(aabb.min.x, aabb.max.x);

		aabb.min.y = (std::min)(aabb.min.y, aabb.max.y);
		aabb.max.y = (std::max)(aabb.min.y, aabb.max.y);

		aabb.min.z = (std::min)(aabb.min.z, aabb.max.z);
		aabb.max.z = (std::max)(aabb.min.z, aabb.max.z);


		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0,1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraScale, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse4x4(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);


		ImGui::Begin("camera");

		ImGui::DragFloat3("CameraPosition", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);

		ImGui::End();

		ImGui::DragFloat3("Sphere.center", &sphere.center.x, 0.01f);
		ImGui::DragFloat("Sphere.radius", &sphere.radius, 0.01f);

		ImGui::DragFloat3("aabb.min", &aabb.min.x, 0.01f);
		ImGui::DragFloat3("aabb.max", &aabb.max.x, 0.01f);


		if (IsCollision(aabb, sphere)) {

			color = 0xFF0000FF;
		} else {

			color = 0xFFFFFFFF;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawSphere(sphere, worldViewProjectionMatrix, viewportMatrix, 0xFFFFFFFF);

		DrawAABB(aabb, worldViewProjectionMatrix, viewportMatrix, color);

		DrawGrid(worldViewProjectionMatrix, viewportMatrix);

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
