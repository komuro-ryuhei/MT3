#include <Novice.h>

#include"MyMath.h"
#include"MT3.h"

#include <imgui.h>

const char kWindowTitle[] = "LC1B_13_コムロ_リュウヘイ";

struct Triangle {
	Vector3 vertices[3]; // 頂点
};

void DrawTriangle(
	const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {

	// 三角形の頂点をスクリーンスペースに変換
	Vector3 transformedVertices[3];
	for (int i = 0; i < 3; ++i) {
		// viewProjection行列を適用
		transformedVertices[i] = Transform(triangle.vertices[i], viewProjectionMatrix);
		// viewport行列を適用
		transformedVertices[i] = Transform(transformedVertices[i], viewportMatrix);
	}

	// 頂点間に線を描画して三角形を形成
	Novice::DrawLine(int(transformedVertices[0].x), int(transformedVertices[0].y),
		int(transformedVertices[1].x), int(transformedVertices[1].y), color);
	Novice::DrawLine(int(transformedVertices[1].x), int(transformedVertices[1].y),
		int(transformedVertices[2].x), int(transformedVertices[2].y), color);
	Novice::DrawLine(int(transformedVertices[2].x), int(transformedVertices[2].y),
		int(transformedVertices[0].x), int(transformedVertices[0].y), color);
}

bool IsCollision(const Triangle& triangle, const Segment& segment) {

	// 三角形の法線を計算
	Vector3 v01 = Subtract(triangle.vertices[1], triangle.vertices[0]);
	Vector3 v12 = Subtract(triangle.vertices[2], triangle.vertices[1]);
	Vector3 normal = Cross(v01, v12);

	// 平面方程式を求める (ax + by + cz + d = 0)
	float d = -Dot(normal, triangle.vertices[0]);

	// 線分の方向ベクトル
	Vector3 segmentDir = Subtract(segment.diff, Vector3(0.0f, 0.0f, 0.0f));

	// 線分の始点から終点までのベクトル
	Vector3 segmentVec = Add(segment.origin, segment.diff);

	// 線分が平面と交差するかをチェック
	float t = -(Dot(normal, segment.origin) + d) / Dot(normal, segmentDir);
	if (t < 0.0f || t > 1.0f) {
		return false; // 線分は平面と交差しない
	}

	// 平面と線分の交点
	Vector3 intersection = Add(segment.origin, Vector3(segmentDir.x * t, segmentDir.y * t, segmentDir.z * t));

	// 交点が三角形の内部にあるかチェック
	auto PointInTriangle = [&](const Vector3& point) {
		Vector3 v0p = Subtract(point, triangle.vertices[0]);
		Vector3 v1p = Subtract(point, triangle.vertices[1]);
		Vector3 v2p = Subtract(point, triangle.vertices[2]);
		Vector3 cross01 = Cross(v01, v0p);
		Vector3 cross12 = Cross(v12, v1p);
		Vector3 v20 = Subtract(triangle.vertices[0], triangle.vertices[2]);
		Vector3 cross20 = Cross(v20, v2p);

		return (Dot(cross01, normal) >= 0.0f &&
			Dot(cross12, normal) >= 0.0f &&
			Dot(cross20, normal) >= 0.0f);
		};

	return PointInTriangle(intersection);
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


	Segment segment{ {-0.3f,-0.3f,0.0f},{0.5f,0.5f,0.5f} };
	Vector3 point{ -1.5f,0.6f,0.6f };

	Vector3 project = Project(Subtract(point, segment.origin), segment.diff);
	Vector3 closestPoint = ClosestPoint(point, segment);


	Triangle triangle{
		{
			{0.0f, 1.0f, 0.0f},   // 頂点0
			{-1.0f, -1.0f, 0.0f}, // 頂点1
			{1.0f, -1.0f, 0.0f}   // 頂点2
		}
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

		ImGui::Begin("Window");

		ImGui::DragFloat3("CameraPosition", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("CameraRotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("triangle.v1", &triangle.vertices[0].x, 0.01f);
		ImGui::DragFloat3("triangle.v2", &triangle.vertices[1].x, 0.01f);
		ImGui::DragFloat3("point", &point.x, 0.01f);
		ImGui::DragFloat3("Segment origin", &segment.origin.x, 0.01f);
		ImGui::DragFloat3("Segment diff", &segment.diff.x, 0.01f);
		ImGui::InputFloat3("Project", &project.x, "%.3f", ImGuiInputTextFlags_ReadOnly);

		ImGui::End();


		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0,1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraScale, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = Inverse4x4(cameraMatrix);
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);


		Vector3 start = Transform(Transform(segment.origin, worldViewProjectionMatrix), viewportMatrix);
		Vector3 end = Transform(Transform(Add(segment.origin, segment.diff), worldViewProjectionMatrix), viewportMatrix);


		if (IsCollision(triangle,segment)) {
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

		DrawTriangle(triangle, worldViewProjectionMatrix, viewportMatrix, WHITE);

		DrawGrid(worldViewProjectionMatrix, viewportMatrix);

		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), color);

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