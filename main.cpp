#include <Novice.h>
#include <Vector3.h>
#include "MyMath.h"
#include <imgui.h>

const char kWindowTitle[] = "LC1B_13_コムロ_リュウヘイ";

struct Line {
	Vector3 origin; //!< 始点
	Vector3 diff; //!< 終点への差分ベクトル
};

struct Ray {
	Vector3 origin; //!< 始点
	Vector3 diff; //!< 終点への差分ベクトル
};

struct Segment {
	Vector3 origin; //!< 始点
	Vector3 diff; //!< 終点への差分ベクトル
};

Vector3 Project(const Vector3& v1, const Vector3& v2) {

	float dotProduct = Dot(v1, v2);
	float lengthSquared = Dot(v2, v2);
	float scaleFactor = dotProduct / lengthSquared;
	return Vector3(v2.x * scaleFactor, v2.y * scaleFactor, v2.z * scaleFactor);
}

// ClosestPoint関数
Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 startToPoint = { point.x - segment.origin.x, point.y - segment.origin.y, point.z - segment.origin.z };
	float t = Dot(startToPoint, segment.diff) / Dot(segment.diff, segment.diff);

	// 線分の端点が最も近い場合
	if (t <= 0) {
		return segment.origin;
	}
	// 線分の端点が最も近い場合
	else if (t >= 1) {
		return { segment.origin.x + segment.diff.x, segment.origin.y + segment.diff.y, segment.origin.z + segment.diff.z };
	}
	// 線分の内部にある場合
	else {
		return { segment.origin.x + t * segment.diff.x,
				 segment.origin.y + t * segment.diff.y,
				 segment.origin.z + t * segment.diff.z };
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

	Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0,1.0f }, rotate, translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraScale, cameraRotate, cameraTranslate);
	Matrix4x4 viewMatrix = Inverse4x4(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);


	Segment segment{ {-2.0f,-1.0f,0.0f},{3.0f,2.0f,2.0f} };
	Vector3 point{ -1.5f,0.6f,0.6f };
	
	Vector3 project = Project(Subtract(point, segment.origin), segment.diff);
	Vector3 closestPoint = ClosestPoint(point, segment);

	Sphere pointSphere{ point,0.01f }; //1cmの球を描画
	Sphere closestPointSphere{ closestPoint,0.01f };


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
		ImGui::InputFloat3("Project", &project.x, "%.3f",ImGuiInputTextFlags_ReadOnly);

		ImGui::End();

		cameraMatrix = MakeAffineMatrix(cameraScale, cameraRotate, cameraTranslate);
		viewMatrix = Inverse4x4(cameraMatrix);

		projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		Vector3 start = Transform(Transform(segment.origin, worldViewProjectionMatrix), viewportMatrix);
		Vector3 end = Transform(Transform(Add(segment.origin, segment.diff), worldViewProjectionMatrix), viewportMatrix);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawGrid(worldViewProjectionMatrix, viewportMatrix);

		DrawSphere(pointSphere, worldViewProjectionMatrix, viewportMatrix, RED);
		DrawSphere(closestPointSphere, worldViewProjectionMatrix, viewportMatrix, BLACK);

		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), WHITE);

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
