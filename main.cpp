#include <Novice.h>

#include "MyMath.h"
#include "MT3.h"

#include <ImGui.h>

const char kWindowTitle[] = "LC1B_13_コムロ_リュウヘイ";

bool IsCollision(const OBB& obb1, const OBB& obb2) {
	// SATによる分離軸の検出
	const float EPSILON = 1e-6f;

	// 15個の分離軸
	Vector3 axes[15];
	for (int i = 0; i < 3; ++i) {
		axes[i] = obb1.orientations[i];
		axes[i + 3] = obb2.orientations[i];
	}

	int axisIndex = 6;
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			axes[axisIndex] = Cross(obb1.orientations[i], obb2.orientations[j]);
			if (Length(axes[axisIndex]) > EPSILON) {
				axes[axisIndex] = Normalize(axes[axisIndex]);
			}
			++axisIndex;
		}
	}

	// 各軸に対する分離軸テスト
	for (int i = 0; i < 15; ++i) {
		Vector3 axis = axes[i];
		if (Length(axis) < EPSILON) {
			continue;
		}

		// OBB1を投影
		float obb1Min = INFINITY, obb1Max = -INFINITY;
		for (int j = 0; j < 8; ++j) {
			Vector3 vertex = obb1.center +
				obb1.orientations[0] * (j & 1 ? obb1.size.x : -obb1.size.x) +
				obb1.orientations[1] * (j & 2 ? obb1.size.y : -obb1.size.y) +
				obb1.orientations[2] * (j & 4 ? obb1.size.z : -obb1.size.z);
			float projection = Dot(vertex, axis);
			obb1Min = (std::min)(obb1Min, projection);
			obb1Max = (std::max)(obb1Max, projection);
		}

		// OBB2を投影
		float obb2Min = INFINITY, obb2Max = -INFINITY;
		for (int j = 0; j < 8; ++j) {
			Vector3 vertex = obb2.center +
				obb2.orientations[0] * (j & 1 ? obb2.size.x : -obb2.size.x) +
				obb2.orientations[1] * (j & 2 ? obb2.size.y : -obb2.size.y) +
				obb2.orientations[2] * (j & 4 ? obb2.size.z : -obb2.size.z);
			float projection = Dot(vertex, axis);
			obb2Min = (std::min)(obb2Min, projection);
			obb2Max = (std::max)(obb2Max, projection);
		}

		// 分離軸テスト
		if (obb1Max < obb2Min || obb2Max < obb1Min) {
			return false;
		}
	}

	// 全ての分離軸で交差しているので衝突している
	return true;
}


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

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

		ImGui::Begin("Camera");

		ImGui::DragFloat3("CameraPosition", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);

		ImGui::End();


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


		// 回転行列を作成
		Matrix4x4 rotateMatrix1 = Multiply(MakeRotateXMatrix(rotate1.x), Multiply(MakeRotateYMatrix(rotate1.y), MakeRotateZMatrix(rotate1.z)));

		// 回転行列から軸を抽出
		obb1.orientations[0].x = rotateMatrix1.m[0][0];
		obb1.orientations[0].y = rotateMatrix1.m[0][1];
		obb1.orientations[0].z = rotateMatrix1.m[0][2];

		obb1.orientations[1].x = rotateMatrix1.m[1][0];
		obb1.orientations[1].y = rotateMatrix1.m[1][1];
		obb1.orientations[1].z = rotateMatrix1.m[1][2];

		obb1.orientations[2].x = rotateMatrix1.m[2][0];
		obb1.orientations[2].y = rotateMatrix1.m[2][1];
		obb1.orientations[2].z = rotateMatrix1.m[2][2];


		// 回転行列を作成
		Matrix4x4 rotateMatrix2 = Multiply(MakeRotateXMatrix(rotate2.x), Multiply(MakeRotateYMatrix(rotate2.y), MakeRotateZMatrix(rotate2.z)));

		// 回転行列から軸を抽出
		obb2.orientations[0].x = rotateMatrix2.m[0][0];
		obb2.orientations[0].y = rotateMatrix2.m[0][1];
		obb2.orientations[0].z = rotateMatrix2.m[0][2];

		obb2.orientations[1].x = rotateMatrix2.m[1][0];
		obb2.orientations[1].y = rotateMatrix2.m[1][1];
		obb2.orientations[1].z = rotateMatrix2.m[1][2];

		obb2.orientations[2].x = rotateMatrix2.m[2][0];
		obb2.orientations[2].y = rotateMatrix2.m[2][1];
		obb2.orientations[2].z = rotateMatrix2.m[2][2];


		if (IsCollision(obb1, obb2)) {
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

		DrawOBB(obb1, worldViewProjectionMatrix, worldViewportMatrix, color);
		DrawOBB(obb2, worldViewProjectionMatrix, worldViewportMatrix, 0xFFFFFFFF);

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
