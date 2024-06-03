#pragma once

#include "MyMath.h"

// 球
struct Sphere {
	Vector3 center;
	float radius;
};

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewPortMatrix) {
	const float kGridHalfWidth = 2.0f; // Grid半分の幅
	const uint32_t kSubdivision = 10; // 分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision); // 1つ分の長さ

	// 奥から手前への線を順々に引いていく
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		// ワールド座標系上の始点と終点を求める
		Vector3 start(-kGridHalfWidth + xIndex * kGridEvery, 0.0f, -kGridHalfWidth);
		Vector3 end(-kGridHalfWidth + xIndex * kGridEvery, 0.0f, kGridHalfWidth);

		// ビュープロジェクション行列とビューポート行列を使用してスクリーン座標系に変換
		Vector3 screenStart = Transform(start, viewProjectionMatrix);
		Vector3 screenEnd = Transform(end, viewProjectionMatrix);

		// スクリーン座標系からビューポート座標系に変換
		screenStart = Transform(screenStart, viewPortMatrix);
		screenEnd = Transform(screenEnd, viewPortMatrix);

		// 変換した座標を使って表示
		if (xIndex == 5) {
			Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), 0x000000FF);
		} else {
			Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), 0xAAAAAAFF);
		}
	}

	// 左から右も同じように順々に引いていく
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		// ワールド座標系上の始点と終点を求める
		Vector3 start(-kGridHalfWidth, 0.0f, -kGridHalfWidth + zIndex * kGridEvery);
		Vector3 end(kGridHalfWidth, 0.0f, -kGridHalfWidth + zIndex * kGridEvery);

		// ビュープロジェクション行列とビューポート行列を使用してスクリーン座標系に変換
		Vector3 screenStart = Transform(start, viewProjectionMatrix);
		Vector3 screenEnd = Transform(end, viewProjectionMatrix);

		// スクリーン座標系からビューポート座標系に変換
		screenStart = Transform(screenStart, viewPortMatrix);
		screenEnd = Transform(screenEnd, viewPortMatrix);

		// 変換した座標を使って表示
		if (zIndex == 5) {
			Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), 0x000000FF);
		} else {
			Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), 0xAAAAAAFF);
		}
	}
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewPortMatrix, uint32_t color) {
	const uint32_t kSubdivision = 16;
	const float kLatEvery = static_cast<float>(M_PI / kSubdivision); // 緯度分割1つ分の角度
	const float kLonEvery = static_cast<float>((M_PI * 2) / kSubdivision); // 経度分割1つ分の角度

	// 緯度の方向に分割 -π/2 ~ π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = static_cast<float>(-M_PI / 2.0f) + kLatEvery * latIndex; // 現在の緯度

		// 経度方向に分割 0 ~ 2π
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery; // 現在の経度

			// world座標系でのa,b,cを求める
			Vector3 a, b, c;
			a = { sphere.radius * std::cos(lat) * std::cos(lon) + sphere.center.x,
				  sphere.radius * std::sin(lat) + sphere.center.y,
				  sphere.radius * std::cos(lat) * std::sin(lon) + sphere.center.z };

			b = { sphere.radius * std::cos(lat + kLatEvery) * std::cos(lon) + sphere.center.x,
				  sphere.radius * std::sin(lat + kLatEvery) + sphere.center.y,
				  sphere.radius * std::cos(lat + kLatEvery) * std::sin(lon) + sphere.center.z };

			c = { sphere.radius * std::cos(lat) * std::cos(lon + kLonEvery) + sphere.center.x,
				  sphere.radius * std::sin(lat) + sphere.center.y,
				  sphere.radius * std::cos(lat) * std::sin(lon + kLonEvery) + sphere.center.z };

			// a,b,cをスクリーン座標まで変換
			// ビュープロジェクション行列とビューポート行列を使用してスクリーン座標系に変換
			Vector3 screenA = Transform(a, viewProjectionMatrix);
			Vector3 screenB = Transform(b, viewProjectionMatrix);
			Vector3 screenC = Transform(c, viewProjectionMatrix);

			// スクリーン座標系からビューポート座標系に変換
			screenA = Transform(screenA, viewPortMatrix);
			screenB = Transform(screenB, viewPortMatrix);
			screenC = Transform(screenC, viewPortMatrix);

			// ab,bcで線を引く
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenB.x), int(screenB.y), color);
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenC.x), int(screenC.y), color);
		}
	}
}

// 線
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

// 平面
struct Plane {
	Vector3 normal; // 法線
	float distance; // 距離
};

Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return { -vector.y,vector.x,0.0f };
	}
	return{ 0.0f,-vector.z,vector.y };
}

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {

	Vector3 center = Multiply(plane.distance, plane.normal); // 1
	Vector3 perpendiculars[4];
	perpendiculars[0] = Normalize(Perpendicular(plane.normal)); // 2
	perpendiculars[1] = { -perpendiculars[0].x, -perpendiculars[0].y, -perpendiculars[0].z }; // 3
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]); // 4
	perpendiculars[3] = { -perpendiculars[2].x,-perpendiculars[2].y,-perpendiculars[2].z }; // 5
	// 6
	Vector3 points[4];
	for (uint32_t index = 0; index < 4; ++index) {
		Vector3 extend = Multiply(2.0f, perpendiculars[index]);
		Vector3 point = Add(center, extend);
		points[index] = Transform(Transform(point, viewProjectionMatrix), viewportMatrix);
	}
	// pointsをそれぞれ結んでDrawLine矩形を描画
	 // 正方形を描画
	Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[2].x), int(points[2].y), int(points[1].x), int(points[1].y), RED);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[3].x), int(points[3].y), GREEN);
	Novice::DrawLine(int(points[3].x), int(points[3].y), int(points[0].x), int(points[0].y), BLUE);
}

// 三角形
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