#include <Novice.h>

#include "MyMath.h"
#include "MT3.h"

#include <ImGui.h>

const char kWindowTitle[] = "LC1B_13_コムロ_リュウヘイ";

Vector3 Reflect(const Vector3& input, const Vector3& normal) {
	
	Vector3 reflect = input - normal * Dot(input, normal) * 2.0f;
	return reflect;
}


// 球と平面の当たり判定
bool IsCollision(const Sphere& sphere, const Plane& plane) {

	float distance = Dot(plane.normal, sphere.center) - plane.distance;

	return fabs(distance) <= sphere.radius;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };


	Ball ball{};
	ball.position = { 0.8f,1.2f,0.3f };
	ball.mass = 2.0f;
	ball.acceleration = { 0.0f,-9.8f,0.0f };
	ball.radius = 0.05f;
	ball.color = 0xFFFFFFFF;

	Plane plane{};
	plane.normal = Normalize({ -0.2f,0.9f,-0.3f });
	plane.distance = 0.0f;


	float deltaTime = 1.0f / 60.0f; // デルタタイム

	bool isMove = false;

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


		if (isMove) {

			ball.velocity += ball.acceleration * deltaTime;
			ball.position += ball.velocity * deltaTime;

			if (IsCollision(Sphere{ ball.position,ball.radius }, plane)) {

				Vector3 reflected = Reflect(ball.velocity, plane.normal);
				Vector3 projectToNormal = Project(reflected, plane.normal);
				Vector3 movingDirection = reflected - projectToNormal;
				ball.velocity = projectToNormal + movingDirection;

				ball.color = 0xFF0000FF;
			} else {
				ball.color = 0xFFFFFFFF;
			}
		}

		ImGui::Checkbox("IsMove", &isMove);

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

		DrawSphere(Sphere{ ball.position, ball.radius }, worldViewProjectionMatrix, worldViewportMatrix, ball.color);
		DrawPlane(plane, worldViewProjectionMatrix, worldViewportMatrix, 0xFFFFFFFF);
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
