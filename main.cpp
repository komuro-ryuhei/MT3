#include <Novice.h>

#include "MyMath.h"
#include "MT3.h"

#include <ImGui.h>

const char kWindowTitle[] = "LC1B_13_コムロ_リュウヘイ";

// OBBとの当たり判定関数
bool IsCollision(const Segment& segment, const OBB& obb) {
	Vector3 d = segment.diff;
	Vector3 m = Subtract(segment.origin, obb.center);

	// サポート点
	Vector3 axes[15] = {
		{1, 0, 0}, {0, 1, 0}, {0, 0, 1},
		obb.orientations[0], obb.orientations[1], obb.orientations[2],
		Cross(obb.orientations[0], d), Cross(obb.orientations[1], d), Cross(obb.orientations[2], d),
		Cross(obb.orientations[0], {1, 0, 0}), Cross(obb.orientations[0], {0, 1, 0}), Cross(obb.orientations[0], {0, 0, 1}),
		Cross(obb.orientations[1], {1, 0, 0}), Cross(obb.orientations[1], {0, 1, 0}), Cross(obb.orientations[1], {0, 0, 1})
	};

	for (int i = 0; i < 15; ++i) {
		float l = fabsf(Dot(m, axes[i]));
		float r = obb.size.x * fabsf(Dot(obb.orientations[0], axes[i])) +
			obb.size.y * fabsf(Dot(obb.orientations[1], axes[i])) +
			obb.size.z * fabsf(Dot(obb.orientations[2], axes[i])) +
			0.5f * fabsf(Dot(d, axes[i]));

		if (l > r) {
			return false;
		}
	}
	return true;
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
		.orientations =
		{{1.0f,0.0f,0.0f},
		{ 0.0f,1.0f,0.0f},
		{ 0.0f,0.0f,1.0f}},
		.size{0.5f,0.5f,0.5f}
	};

	Segment segment{
		.origin{-0.8f,-0.3f,0.0f},
		.diff{0.5f,0.5f,0.5f}
	};

	Vector3 point{ -1.5f,0.6f,0.6f };

	Vector3 project = Project(Subtract(point, segment.origin), segment.diff);
	Vector3 closestPoint = ClosestPoint(point, segment);

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
		ImGui::Begin("Window");

		ImGui::DragFloat3("CameraPosition", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("point", &point.x, 0.01f);
		ImGui::DragFloat3("Segment origin", &segment.origin.x, 0.01f);
		ImGui::DragFloat3("Segment diff", &segment.diff.x, 0.01f);
		ImGui::InputFloat3("Project", &project.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

		ImGui::SliderAngle("rotateX", &rotate.x);
		ImGui::SliderAngle("rotateY", &rotate.y);
		ImGui::SliderAngle("rotateZ", &rotate.z);

		ImGui::DragFloat3("obb.orientations[0]", &obb.orientations[0].x, 0.01f);
		ImGui::DragFloat3("obb.orientations[1]", &obb.orientations[1].x, 0.01f);
		ImGui::DragFloat3("obb.orientations[2]", &obb.orientations[2].x, 0.01f);

		ImGui::DragFloat3("size", &obb.size.x, 0.01f);

		ImGui::End();


		// 回転行列を作成
		Matrix4x4 rotateMatrix = Multiply(MakeRotateXMatrix(rotate.x), Multiply(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z)));

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


		Vector3 start = Transform(Transform(segment.origin, worldViewProjectionMatrix), viewportMatrix);
		Vector3 end = Transform(Transform(Add(segment.origin, segment.diff), worldViewProjectionMatrix), viewportMatrix);


		// 当たり判定チェック
		if (IsCollision(segment, obb)) {
			color = 0xFF0000FF;  // 赤色
		} else {
			color = 0xFFFFFFFF;  // 白色
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawOBB(obb, worldViewProjectionMatrix, viewportMatrix, color);

		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), color);

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
