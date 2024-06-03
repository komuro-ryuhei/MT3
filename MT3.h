#pragma once

#include "MyMath.h"

// ��
struct Sphere {
	Vector3 center;
	float radius;
};

void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewPortMatrix) {
	const float kGridHalfWidth = 2.0f; // Grid�����̕�
	const uint32_t kSubdivision = 10; // ������
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision); // 1���̒���

	// �������O�ւ̐������X�Ɉ����Ă���
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		// ���[���h���W�n��̎n�_�ƏI�_�����߂�
		Vector3 start(-kGridHalfWidth + xIndex * kGridEvery, 0.0f, -kGridHalfWidth);
		Vector3 end(-kGridHalfWidth + xIndex * kGridEvery, 0.0f, kGridHalfWidth);

		// �r���[�v���W�F�N�V�����s��ƃr���[�|�[�g�s����g�p���ăX�N���[�����W�n�ɕϊ�
		Vector3 screenStart = Transform(start, viewProjectionMatrix);
		Vector3 screenEnd = Transform(end, viewProjectionMatrix);

		// �X�N���[�����W�n����r���[�|�[�g���W�n�ɕϊ�
		screenStart = Transform(screenStart, viewPortMatrix);
		screenEnd = Transform(screenEnd, viewPortMatrix);

		// �ϊ��������W���g���ĕ\��
		if (xIndex == 5) {
			Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), 0x000000FF);
		} else {
			Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), 0xAAAAAAFF);
		}
	}

	// ������E�������悤�ɏ��X�Ɉ����Ă���
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		// ���[���h���W�n��̎n�_�ƏI�_�����߂�
		Vector3 start(-kGridHalfWidth, 0.0f, -kGridHalfWidth + zIndex * kGridEvery);
		Vector3 end(kGridHalfWidth, 0.0f, -kGridHalfWidth + zIndex * kGridEvery);

		// �r���[�v���W�F�N�V�����s��ƃr���[�|�[�g�s����g�p���ăX�N���[�����W�n�ɕϊ�
		Vector3 screenStart = Transform(start, viewProjectionMatrix);
		Vector3 screenEnd = Transform(end, viewProjectionMatrix);

		// �X�N���[�����W�n����r���[�|�[�g���W�n�ɕϊ�
		screenStart = Transform(screenStart, viewPortMatrix);
		screenEnd = Transform(screenEnd, viewPortMatrix);

		// �ϊ��������W���g���ĕ\��
		if (zIndex == 5) {
			Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), 0x000000FF);
		} else {
			Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), 0xAAAAAAFF);
		}
	}
}

void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewPortMatrix, uint32_t color) {
	const uint32_t kSubdivision = 16;
	const float kLatEvery = static_cast<float>(M_PI / kSubdivision); // �ܓx����1���̊p�x
	const float kLonEvery = static_cast<float>((M_PI * 2) / kSubdivision); // �o�x����1���̊p�x

	// �ܓx�̕����ɕ��� -��/2 ~ ��/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = static_cast<float>(-M_PI / 2.0f) + kLatEvery * latIndex; // ���݂̈ܓx

		// �o�x�����ɕ��� 0 ~ 2��
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery; // ���݂̌o�x

			// world���W�n�ł�a,b,c�����߂�
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

			// a,b,c���X�N���[�����W�܂ŕϊ�
			// �r���[�v���W�F�N�V�����s��ƃr���[�|�[�g�s����g�p���ăX�N���[�����W�n�ɕϊ�
			Vector3 screenA = Transform(a, viewProjectionMatrix);
			Vector3 screenB = Transform(b, viewProjectionMatrix);
			Vector3 screenC = Transform(c, viewProjectionMatrix);

			// �X�N���[�����W�n����r���[�|�[�g���W�n�ɕϊ�
			screenA = Transform(screenA, viewPortMatrix);
			screenB = Transform(screenB, viewPortMatrix);
			screenC = Transform(screenC, viewPortMatrix);

			// ab,bc�Ő�������
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenB.x), int(screenB.y), color);
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenC.x), int(screenC.y), color);
		}
	}
}

// ��
struct Line {
	Vector3 origin; //!< �n�_
	Vector3 diff; //!< �I�_�ւ̍����x�N�g��
};

struct Ray {
	Vector3 origin; //!< �n�_
	Vector3 diff; //!< �I�_�ւ̍����x�N�g��
};

struct Segment {
	Vector3 origin; //!< �n�_
	Vector3 diff; //!< �I�_�ւ̍����x�N�g��
};

Vector3 Project(const Vector3& v1, const Vector3& v2) {

	float dotProduct = Dot(v1, v2);
	float lengthSquared = Dot(v2, v2);
	float scaleFactor = dotProduct / lengthSquared;
	return Vector3(v2.x * scaleFactor, v2.y * scaleFactor, v2.z * scaleFactor);
}

// ClosestPoint�֐�
Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	Vector3 startToPoint = { point.x - segment.origin.x, point.y - segment.origin.y, point.z - segment.origin.z };
	float t = Dot(startToPoint, segment.diff) / Dot(segment.diff, segment.diff);

	// �����̒[�_���ł��߂��ꍇ
	if (t <= 0) {
		return segment.origin;
	}
	// �����̒[�_���ł��߂��ꍇ
	else if (t >= 1) {
		return { segment.origin.x + segment.diff.x, segment.origin.y + segment.diff.y, segment.origin.z + segment.diff.z };
	}
	// �����̓����ɂ���ꍇ
	else {
		return { segment.origin.x + t * segment.diff.x,
				 segment.origin.y + t * segment.diff.y,
				 segment.origin.z + t * segment.diff.z };
	}
}

// ����
struct Plane {
	Vector3 normal; // �@��
	float distance; // ����
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
	// points�����ꂼ�ꌋ���DrawLine��`��`��
	 // �����`��`��
	Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[2].x), int(points[2].y), int(points[1].x), int(points[1].y), RED);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[3].x), int(points[3].y), GREEN);
	Novice::DrawLine(int(points[3].x), int(points[3].y), int(points[0].x), int(points[0].y), BLUE);
}

// �O�p�`
struct Triangle {
	Vector3 vertices[3]; // ���_
};

void DrawTriangle(
	const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {

	// �O�p�`�̒��_���X�N���[���X�y�[�X�ɕϊ�
	Vector3 transformedVertices[3];
	for (int i = 0; i < 3; ++i) {
		// viewProjection�s���K�p
		transformedVertices[i] = Transform(triangle.vertices[i], viewProjectionMatrix);
		// viewport�s���K�p
		transformedVertices[i] = Transform(transformedVertices[i], viewportMatrix);
	}

	// ���_�Ԃɐ���`�悵�ĎO�p�`���`��
	Novice::DrawLine(int(transformedVertices[0].x), int(transformedVertices[0].y),
		int(transformedVertices[1].x), int(transformedVertices[1].y), color);
	Novice::DrawLine(int(transformedVertices[1].x), int(transformedVertices[1].y),
		int(transformedVertices[2].x), int(transformedVertices[2].y), color);
	Novice::DrawLine(int(transformedVertices[2].x), int(transformedVertices[2].y),
		int(transformedVertices[0].x), int(transformedVertices[0].y), color);
}