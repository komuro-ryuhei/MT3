#include <Novice.h>

#include "MyMath.h"
#include "MT3.h"

#include <ImGui.h>
#include <algorithm>

const char kWindowTitle[] = "LC1B_13_コムロ_リュウヘイ";

/// <summary>
/// OBBと線の当たり判定
/// </summary>
/// <param name="rotate"></param>
/// <param name="obb"></param>
/// <param name="line"></param>
/// <returns></returns>
bool IsCollision(const Vector3& rotation, const OBB& obb, const Segment& segment) {

	// 回転行列の計算（オイラー角から回転行列を計算）
	Matrix4x4 rotateX = MakeRotateXMatrix(rotation.x);
	Matrix4x4 rotateY = MakeRotateYMatrix(rotation.y);
	Matrix4x4 rotateZ = MakeRotateZMatrix(rotation.z);
	Matrix4x4 rotateMatrix = Multiply(rotateX, Multiply(rotateY, rotateZ));

	// OBBの軸を回転させる
	Vector3 orientations[3];
	orientations[0] = Transform(obb.orientations[0], rotateMatrix);
	orientations[1] = Transform(obb.orientations[1], rotateMatrix);
	orientations[2] = Transform(obb.orientations[2], rotateMatrix);

	// 線分の始点と終点をOBBのローカル座標系に変換
	Vector3 lineStartLocal = segment.origin - obb.center;
	Vector3 lineEndLocal = lineStartLocal + segment.diff;

	// 各軸ごとにローカル座標に変換
	Vector3 lineStartTransformed = { Dot(lineStartLocal, orientations[0]), Dot(lineStartLocal, orientations[1]), Dot(lineStartLocal, orientations[2]) };
	Vector3 lineEndTransformed = { Dot(lineEndLocal, orientations[0]), Dot(lineEndLocal, orientations[1]), Dot(lineEndLocal, orientations[2]) };

	// 線分とAABBの当たり判定
	Vector3 boxMin = { -obb.size.x, -obb.size.y, -obb.size.z };
	Vector3 boxMax = { obb.size.x, obb.size.y, obb.size.z };

	// 線分とAABBの当たり判定を行う（Slab method）
	float tMin = 0.0f;
	float tMax = 1.0f;

	for (int i = 0; i < 3; ++i) {
		float start = (&lineStartTransformed.x)[i];
		float end = (&lineEndTransformed.x)[i];
		float min = (&boxMin.x)[i];
		float max = (&boxMax.x)[i];

		float t0 = (min - start) / (end - start);
		float t1 = (max - start) / (end - start);

		if (t0 > t1) std::swap(t0, t1);

		tMin = t0 > tMin ? t0 : tMin;
		tMax = t1 < tMax ? t1 : tMax;

		if (tMin > tMax) return false;
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
		
		CaluculationMatrix();

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


		Vector3 start = Transform(Transform(segment.origin, worldViewProjectionMatrix), worldViewportMatrix);
		Vector3 end = Transform(Transform(Add(segment.origin, segment.diff), worldViewProjectionMatrix), worldViewportMatrix);


		// 当たり判定チェック
		if (IsCollision(rotate,obb, segment)) {
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

		DrawOBB(obb, worldViewProjectionMatrix, worldViewportMatrix, color);

		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), 0xFFFFFFFF);

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