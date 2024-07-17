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

	struct Pendulum {

		Vector3 anchor; // アンカーポイント
		float length;   // 紐の長さ
		float angle;    // 現在の角度
		float angularVelocity;     // 角速度ω 
		float angularAcceleration; // 角加速度
	};

	Pendulum pendulum;
	pendulum.anchor = { 0.0f,1.0f,0.0f };
	pendulum.length = 0.8f;
	pendulum.angle = 0.7f;
	pendulum.angularVelocity = 0.0f;
	pendulum.angularAcceleration = 0.0f;


	Sphere sphere;
	sphere.center.x = pendulum.anchor.x + std::sin(pendulum.angle) * pendulum.length;
	sphere.center.y = pendulum.anchor.y - std::cos(pendulum.angle) * pendulum.length;
	sphere.center.z = pendulum.anchor.z;
	sphere.radius = 0.05f;


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

		Matrix4x4 anchorWorldMatirx = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
		Matrix4x4 sphereWorldMatirx = MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });

		Matrix4x4 anchorWvpMatrix = Multiply(anchorWorldMatirx, Multiply(worldViewMatrix, projectionMatrix));
		Matrix4x4 sphereWvpMatrix = Multiply(sphereWorldMatirx, Multiply(worldViewMatrix, projectionMatrix));


		Matrix4x4 anchorLineWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, Vector3(anchorWorldMatirx.m[3][0], anchorWorldMatirx.m[3][1], anchorWorldMatirx.m[3][2]));
		Matrix4x4 sphereLineWorldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, Vector3(sphereWorldMatirx.m[3][0], sphereWorldMatirx.m[3][1], sphereWorldMatirx.m[3][2]));

		Matrix4x4 anchorLineWvpMatrix = Multiply(anchorLineWorldMatrix, Multiply(worldViewMatrix, projectionMatrix));
		Matrix4x4 sphereLineWvpMatrix = Multiply(sphereLineWorldMatrix, Multiply(worldViewMatrix, projectionMatrix));


		if (isMove) {

			pendulum.angularAcceleration = -(9.8f / pendulum.length) * std::sin(pendulum.angle);
			pendulum.angularVelocity += pendulum.angularAcceleration * deltaTime;
			pendulum.angle += pendulum.angularVelocity * deltaTime;

			sphere.center.x = pendulum.anchor.x + std::sin(pendulum.angle) * pendulum.length;
			sphere.center.y = pendulum.anchor.y - std::cos(pendulum.angle) * pendulum.length;
			sphere.center.z = pendulum.anchor.z;
		}

		ImGui::Checkbox("IsMove", &isMove);
		

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawSphere(sphere, worldViewProjectionMatrix, worldViewportMatrix, 0xFFFFFFFF);
		DrawGrid(worldViewProjectionMatrix, worldViewportMatrix);

		DrawLine(sphere.center, pendulum.anchor, anchorLineWvpMatrix, sphereLineWvpMatrix, worldViewportMatrix, 0xFFFFFFFF);

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
