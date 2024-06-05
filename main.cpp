#include <Novice.h>

#include "MyMath.h"
#include "MT3.h"

#include <ImGui.h>
#include <algorithm>

const char kWindowTitle[] = "LC1B_13_コムロ_リュウヘイ";

bool IsCollision(const AABB& aabb, const Segment& segment) {
	Vector3 dir = segment.diff;
	Vector3 invDir = { 1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z };

	float t1 = (aabb.min.x - segment.origin.x) * invDir.x;
	float t2 = (aabb.max.x - segment.origin.x) * invDir.x;
	float t3 = (aabb.min.y - segment.origin.y) * invDir.y;
	float t4 = (aabb.max.y - segment.origin.y) * invDir.y;
	float t5 = (aabb.min.z - segment.origin.z) * invDir.z;
	float t6 = (aabb.max.z - segment.origin.z) * invDir.z;

	float tmin = std::max({ std::min(t1, t2), std::min(t3, t4), std::min(t5, t6) });
	float tmax = std::min({ std::max(t1, t2), std::max(t3, t4), std::max(t5, t6) });

	if (tmin > tmax) {
		return false;
	}

	if (tmax < 0) {
		return false;
	}

	if (tmin <= 1 && tmax >= 0) {
		return true;
	}

	return false;
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	AABB aabb{
		.min{ -0.5f, -0.5f, -0.5f },
		.max{ 0.5f, 0.5f, 0.5f },
	};

	Segment segment{ {-1.0f, 0.3f, 0.0f}, {2.0f, -0.5f, 0.0f} };
	Vector3 point{ -1.5f, 0.6f, 0.6f };

	Vector3 project = Project(Subtract(point, segment.origin), segment.diff);
	Vector3 closestPoint = ClosestPoint(point, segment);

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

		CaluculationMatrix();

		ImGui::Begin("camera");

		ImGui::DragFloat3("CameraPosition", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);

		ImGui::End();

		ImGui::Begin("aabb");

		ImGui::DragFloat3("aabb.min", &aabb.min.x, 0.01f);
		ImGui::DragFloat3("aabb.max", &aabb.max.x, 0.01f);

		ImGui::End();

		ImGui::Begin("segment");

		ImGui::DragFloat3("point", &point.x, 0.01f);
		ImGui::DragFloat3("Segment origin", &segment.origin.x, 0.01f);
		ImGui::DragFloat3("Segment diff", &segment.diff.x, 0.01f);
		ImGui::InputFloat3("Project", &project.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

		ImGui::End();

		Vector3 start = Transform(Transform(segment.origin, worldViewProjectionMatrix), worldViewportMatrix);
		Vector3 end = Transform(Transform(Add(segment.origin, segment.diff), worldViewProjectionMatrix), worldViewportMatrix);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(worldViewProjectionMatrix, worldViewportMatrix);

		DrawAABB(aabb, worldViewProjectionMatrix, worldViewportMatrix, color);

		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), 0xFFFFFFFF);

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