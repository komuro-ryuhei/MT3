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
		float halfApexAngle; // 円錐の頂点半分
		float angle;    // 現在の角度
		float angularVelocity;     // 角速度ω 
	};

	Pendulum conicalPendulum;
	conicalPendulum.anchor = { 0.0f,1.0f,0.0f };
	conicalPendulum.length = 0.8f;
	conicalPendulum.halfApexAngle = 0.7f;
	conicalPendulum.angle = 0.0f;
	conicalPendulum.angularVelocity = 0.0f;


	float radius = std::sin(conicalPendulum.halfApexAngle) * conicalPendulum.length;
	float height = std::cos(conicalPendulum.halfApexAngle) * conicalPendulum.length;

	Sphere sphere;
	sphere.center.x = conicalPendulum.anchor.x + std::cos(conicalPendulum.angle) * radius;
	sphere.center.y = conicalPendulum.anchor.y - height;
	sphere.center.z = conicalPendulum.anchor.z - std::sin(conicalPendulum.angle) * radius;
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

			conicalPendulum.angularVelocity = std::sqrt(9.8f / (conicalPendulum.length * std::cos(conicalPendulum.halfApexAngle)));
			conicalPendulum.angle += conicalPendulum.angularVelocity * deltaTime;

			radius = std::sin(conicalPendulum.halfApexAngle) * conicalPendulum.length;
			height = std::cos(conicalPendulum.halfApexAngle) * conicalPendulum.length;

			sphere.center.x = conicalPendulum.anchor.x + std::cos(conicalPendulum.angle) * radius;
			sphere.center.y = conicalPendulum.anchor.y - height;
			sphere.center.z = conicalPendulum.anchor.z - std::sin(conicalPendulum.angle) * radius;
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

		DrawLine(sphere.center, conicalPendulum.anchor, anchorLineWvpMatrix, sphereLineWvpMatrix, worldViewportMatrix, 0xFFFFFFFF);

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
