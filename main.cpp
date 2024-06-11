#include <Novice.h>

#include "MyMath.h"
#include "MT3.h"

#include <ImGui.h>
#include <algorithm>

const char kWindowTitle[] = "LC1B_13_コムロ_リュウヘイ";

struct OBB {
	Vector3 center;
	Vector3 orientations[3];
	Vector3 size;
};

bool IsCollision(const OBB& obb, const Sphere& sphere) {

	// 球の中心をOBBのローカル座標系に変換
	Vector3 d = sphere.center - obb.center;
	Vector3 localCenter;
	localCenter.x = Dot(d, obb.orientations[0]);
	localCenter.y = Dot(d, obb.orientations[1]);
	localCenter.z = Dot(d, obb.orientations[2]);

	// 最近傍点の計算
	Vector3 closestPoint = localCenter;

	closestPoint.x = (std::max)(-obb.size.x, (std::min)(obb.size.x, closestPoint.x));
	closestPoint.y = (std::max)(-obb.size.y, (std::min)(obb.size.y, closestPoint.y));
	closestPoint.z = (std::max)(-obb.size.z, (std::min)(obb.size.z, closestPoint.z));

	// OBBのローカル座標系からワールド座標系に戻す
	Vector3 closestPointWorld = obb.center +
		Multiply(closestPoint.x, obb.orientations[0]) +
		Multiply(closestPoint.y, obb.orientations[1]) +
		Multiply(closestPoint.z, obb.orientations[2]);

	// 球の中心と最近傍点の距離を計算
	float distance = Length(sphere.center - closestPointWorld);

	// 距離が球の半径以内なら衝突している
	//return distance <= sphere.radius;

	if (distance <= sphere.radius) {

		return true;
	} else {

		return false;
	}
}

void DrawOBB(const OBB& obb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewPortMatrix, uint32_t color) {
	Vector3 vertices[8];

	// OBBの8つの頂点を計算
	Vector3 halfSizeX = obb.orientations[0] * obb.size.x;
	Vector3 halfSizeY = obb.orientations[1] * obb.size.y;
	Vector3 halfSizeZ = obb.orientations[2] * obb.size.z;

	vertices[0] = obb.center - halfSizeX - halfSizeY - halfSizeZ;
	vertices[1] = obb.center + halfSizeX - halfSizeY - halfSizeZ;
	vertices[2] = obb.center + halfSizeX + halfSizeY - halfSizeZ;
	vertices[3] = obb.center - halfSizeX + halfSizeY - halfSizeZ;

	vertices[4] = obb.center - halfSizeX - halfSizeY + halfSizeZ;
	vertices[5] = obb.center + halfSizeX - halfSizeY + halfSizeZ;
	vertices[6] = obb.center + halfSizeX + halfSizeY + halfSizeZ;
	vertices[7] = obb.center - halfSizeX + halfSizeY + halfSizeZ;

	// 頂点を変換して描画
	for (int i = 0; i < 8; ++i) {
		vertices[i] = Transform(vertices[i], viewProjectionMatrix);
		vertices[i] = Transform(vertices[i], viewPortMatrix);
	}

	// OBBのエッジを描画
	int edges[12][2] = {
		{0, 1}, {1, 2}, {2, 3}, {3, 0},
		{4, 5}, {5, 6}, {6, 7}, {7, 4},
		{0, 4}, {1, 5}, {2, 6}, {3, 7}
	};

	for (int i = 0; i < 12; ++i) {
		Novice::DrawLine(int(vertices[edges[i][0]].x), int(vertices[edges[i][0]].y), int(vertices[edges[i][1]].x), int(vertices[edges[i][1]].y), color);
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	OBB obb{
		.center{-1.0f,0.0f,0.0f},
		.orientations = {
			{1.0f,0.0f,0.0f},
			{0.0f,1.0f,0.0f},
			{0.0f,0.0f,1.0f}},
		.size{0.5f,0.5f,0.5f}
	};

	Sphere sphere{
		.center{0.0f,0.0f,0.0f},
		.radius{0.5f}
	};

	unsigned int color = 0xFFFFFFFF;

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

		// 回転行列を作成
		Matrix4x4 rotateMatrix = Multiply(MakeRotateXMatrix(rotate.x), Multiply(MakeRotateYMatrix(rotate.y), MakeRotateYMatrix(rotate.z)));

		// 回転行列から軸を抽出
		obb.orientations[0].x = rotateMatrix.m[0][0];
		obb.orientations[0].y = rotateMatrix.m[0][1];
		obb.orientations[0].z = rotateMatrix.m[0][2];

		obb.orientations[1].x = rotateMatrix.m[1][0];
		obb.orientations[1].y = rotateMatrix.m[1][1];
		obb.orientations[1].z = rotateMatrix.m[1][2];

		obb.orientations[2].x = rotateMatrix.m[2][0];
		obb.orientations[2].y = rotateMatrix.m[2][1];
		obb.orientations[2].z = rotateMatrix.m[2][2];

		if (IsCollision(obb, sphere)) {
			color = 0xFF0000FF;
		} else {
			color = 0xFFFFFFFF;
		}

		ImGui::SliderAngle("rotateX", &rotate.x);
		ImGui::SliderAngle("rotateY", &rotate.y);
		ImGui::SliderAngle("rotateZ", &rotate.z);

		ImGui::DragFloat3("obb.orientations[0]", &obb.orientations[0].x, 0.01f);
		ImGui::DragFloat3("obb.orientations[1]", &obb.orientations[1].x, 0.01f);
		ImGui::DragFloat3("obb.orientations[2]", &obb.orientations[2].x, 0.01f);

		ImGui::DragFloat3("size", &obb.size.x, 0.01f);

		ImGui::DragFloat3("sphere.center", &sphere.center.x, 0.01f);

		ImGui::DragFloat("sphere.radius", &sphere.radius, 0.01f);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(worldViewProjectionMatrix, viewportMatrix);

		DrawOBB(obb, worldViewProjectionMatrix, viewportMatrix, color);

		DrawSphere(sphere, worldViewProjectionMatrix, viewportMatrix, 0xFFFFFFFF);

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
