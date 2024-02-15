#pragma once
#include "Matrix4x4.h"
#include "Vector3.h"
#include <assert.h>
#include <cmath>
#include <math.h>

// ���@
Vector3 Add(Vector3 v1, Vector3 v2);

// ���@
Vector3 Subtract(Vector3 v1, Vector3 v2);

// �X�J���[�{ (��)
Vector3 Multiply(float k, Vector3 v1);

// ����
float Dot(const Vector3& v1, const Vector3& v2);

// ����(�m����)
float Length(const Vector3& v);

// ���K��
Vector3 Normalize(const Vector3& v);

// �s��̉��@
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

// �s��̌��@
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);

// �s��̐�
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);

// �t�s��
Matrix4x4 Inverse(const Matrix4x4& m1);

// �]�u�s��
Matrix4x4 Transpose(const Matrix4x4& m1);

// �P�ʍs��̍쐬
Matrix4x4 MakeIdentity4x4();

// ���s�ړ�
Matrix4x4 MakeTranslateMatrix(Vector3 translate);

// ���W�ϊ�
Vector3 cTransform(Vector3 vector, Matrix4x4 matrix);

// X����]�s��
Matrix4x4 MakeRotateXmatrix(float radian);

// Y����]�s��
Matrix4x4 MakeRotateYmatrix(float radian);

// Z����]�s��
Matrix4x4 MakeRotateZmatrix(float radian);

// �A�t�B���ϊ�
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);

// �������e�s��
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);

// ���ˉe�s��
Matrix4x4 MakeOrthographicMatrix(
    float left, float top, float right, float bottom, float nearClip, float farClip);

// �r���[�|�[�g�ϊ��s��
Matrix4x4 MakeViewportMatrix(
    float left, float top, float width, float height, float minDepth, float maxDepth);


