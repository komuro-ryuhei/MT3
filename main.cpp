#include <Novice.h>
#include "MyMath.h"

const char kWindowTitle[] = "LC1B_13_コムロ_リュウヘイ";

Vector3 Cross(const Vector3& v1, const Vector3& v2) {

	return Vector3(
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	);
}

bool IsBackface(const Vector3& v0, const Vector3& v1, const Vector3& v2, const Vector3& cameraPos) {
	Vector3 edge1 = v1 - v0;
	Vector3 edge2 = v2 - v0;
	Vector3 normal = Cross(edge1, edge2);
	Vector3 viewDirection = cameraPos - v0;

	// 法線ベクトルとカメラの方向ベクトルとの内積を計算
	float dotProduct = Dot(normal, viewDirection);

	// 内積が負であれば背面
	return dotProduct < 0;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	Vector3 rotate{ 0.0f,0.0f,0.0f };
	Vector3 translate{ 0.0f,0.0f,0.0f };

	Vector3 cameraPosition{ 0.0f,0.0f,-5.0f };

	Vector3 v1{ 1.2f,-3.9f,2.5f };
	Vector3 v2{ 2.8f,0.4f,-1.3f };
	Vector3 cross = Cross(v1, v2);

	int kWindowWidth = 1280;
	int kWindowHeight = 720;

	Vector3 kLocalVertices[3];
	kLocalVertices[0] = { 0.0f,0.5f,0.0f };
	kLocalVertices[1] = { -0.5f,-0.5f,0.0f };
	kLocalVertices[2] = { 0.5f,-0.5f,0.0f };


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

		if (keys[DIK_W] != 0) {
			translate.z += 0.02f; // 前進
		}
		if (keys[DIK_S] != 0) {
			translate.z -= 0.02f; // 後退
		}
		if (keys[DIK_A] != 0) {
			translate.x -= 0.02f; // 左移動
		}
		if (keys[DIK_D] != 0) {
			translate.x += 0.02f; // 右移動
		}

		// Y軸方向に回転する
		rotate.y += 0.05f;

		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0,1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, cameraPosition);
		Matrix4x4 viewMatrix = Inverse4x4(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		Vector3 screenVertices[3];
		for (uint32_t i = 0; i < 3; ++i) {
			Vector3 ndcVertex = Transform(kLocalVertices[i], worldViewProjectionMatrix);
			screenVertices[i] = Transform(ndcVertex, viewportMatrix);
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		VectorScreenPrintf(0, 0, cross, "Cross");

		// 背面カリング
		if (!IsBackface(screenVertices[0], screenVertices[1], screenVertices[2], cameraPosition)) {
			// 背面でなければ三角形を描画
			Novice::DrawTriangle(
				int(screenVertices[0].x), int(screenVertices[0].y),
				int(screenVertices[1].x), int(screenVertices[1].y),
				int(screenVertices[2].x), int(screenVertices[2].y),
				RED, kFillModeSolid);
		}

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
