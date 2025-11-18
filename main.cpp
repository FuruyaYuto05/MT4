#include <Novice.h>
#include <cmath> // std::sqrt, std::abs, std::sin, std::cos 用
#include <algorithm> 

const char kWindowTitle[] = "学籍番号";



// Vector3構造体の定義
struct Vector3 {
	float x;
	float y;
	float z;
};

// Vector3の長さ
float Length(const Vector3& v) {
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

// Vector3の正規化
Vector3 Normalize(const Vector3& v) {
	float len = Length(v);
	if (len < 1e-6f) {
		return { 0.0f, 0.0f, 0.0f };
	}
	float invLen = 1.0f / len;
	return {
		v.x * invLen,
		v.y * invLen,
		v.z * invLen
	};
}

// Vector3のCross積
Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	return {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	};
}

// Vector3とスカラーの乗算
Vector3 Multiply(const Vector3& v, float scalar) {
	return { v.x * scalar, v.y * scalar, v.z * scalar };
}

// Vector3の加算
Vector3 Add(const Vector3& v1, const Vector3& v2) {
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

// Vector3の減算
Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

struct Matrix4x4 {
	float m[4][4];
};

// 4x4行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] =
				m1.m[i][0] * m2.m[0][j] +
				m1.m[i][1] * m2.m[1][j] +
				m1.m[i][2] * m2.m[2][j] +
				m1.m[i][3] * m2.m[3][j];
		}
	}
	return result;
}


Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	float w = 1.0f;
	Vector3 result;
	
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + w * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + w * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + w * matrix.m[3][2];
	float transformedW = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + w * matrix.m[3][3];

	if (std::abs(transformedW) > 1e-6f) {
		result.x /= transformedW;
		result.y /= transformedW;
		result.z /= transformedW;
	}
	return result;
}




// Quaternion構造体の定義
struct Quaternion {
	float x; 
	float y; 
	float z; 
	float w; 
};

// Quaternionの積
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs) {
	Quaternion result;
	result.w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
	result.x = lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y;
	result.y = lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x;
	result.z = lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w;
	return result;
}

// 単位Quaternionを返す
Quaternion IdentityQuaternion() {
	return { 0.0f, 0.0f, 0.0f, 1.0f };
}

// 共役Quaternionを返す
Quaternion Conjugate(const Quaternion& quaternion) {
	return { -quaternion.x, -quaternion.y, -quaternion.z, quaternion.w };
}

// Quaternionのnormを返す
float Norm(const Quaternion& quaternion) {
	return std::sqrt(
		quaternion.x * quaternion.x +
		quaternion.y * quaternion.y +
		quaternion.z * quaternion.z +
		quaternion.w * quaternion.w
	);
}

// 正規化したQuaternionを返す
Quaternion Normalize(const Quaternion& quaternion) {
	float norm = Norm(quaternion);
	if (norm < 1e-6f) { 
		return { 0.0f, 0.0f, 0.0f, 0.0f };
	}
	float invNorm = 1.0f / norm;
	return {
		quaternion.x * invNorm,
		quaternion.y * invNorm,
		quaternion.z * invNorm,
		quaternion.w * invNorm
	};
}

// 逆Quaternionを返す
Quaternion Inverse(const Quaternion& quaternion) {
	float normSquared = quaternion.x * quaternion.x + quaternion.y * quaternion.y + quaternion.z * quaternion.z + quaternion.w * quaternion.w;
	if (normSquared < 1e-6f) { 
		return { 0.0f, 0.0f, 0.0f, 0.0f };
	}
	float invNormSquared = 1.0f / normSquared;
	Quaternion conjugate = Conjugate(quaternion);

	return {
		conjugate.x * invNormSquared,
		conjugate.y * invNormSquared,
		conjugate.z * invNormSquared,
		conjugate.w * invNormSquared
	};
}


// 軸角度から回転Quaternionを生成する
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
	Vector3 normalizedAxis = Normalize(axis);
	float halfAngle = angle / 2.0f;
	float sinHalf = std::sin(halfAngle);
	float cosHalf = std::cos(halfAngle);

	Quaternion result;
	result.x = normalizedAxis.x * sinHalf;
	result.y = normalizedAxis.y * sinHalf;
	result.z = normalizedAxis.z * sinHalf;
	result.w = cosHalf;

	return result;
}

// ベクトルをQuaternionで回転させる
Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion) {
	
	Quaternion p = { vector.x, vector.y, vector.z, 0.0f };

	Quaternion qInv = Inverse(quaternion);
	Quaternion temp = Multiply(quaternion, p);
	Quaternion resultQ = Multiply(temp, qInv);

	
	return { resultQ.x, resultQ.y, resultQ.z };
}

// Quaternionから回転行列を生成する
Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion) {
	Matrix4x4 result;

	float x = quaternion.x;
	float y = quaternion.y;
	float z = quaternion.z;
	float w = quaternion.w;

	float x2 = x * x;
	float y2 = y * y;
	float z2 = z * z;
	float xy = x * y;
	float xz = x * z;
	float yz = y * z;
	float wx = w * x;
	float wy = w * y;
	float wz = w * z;

	// 行列の計算式 (行優先で定義)
	result.m[0][0] = 1.0f - 2.0f * (y2 + z2);
	result.m[0][1] = 2.0f * (xy + wz);
	result.m[0][2] = 2.0f * (xz - wy);
	result.m[0][3] = 0.0f;

	result.m[1][0] = 2.0f * (xy - wz);
	result.m[1][1] = 1.0f - 2.0f * (x2 + z2);
	result.m[1][2] = 2.0f * (yz + wx);
	result.m[1][3] = 0.0f;

	result.m[2][0] = 2.0f * (xz + wy);
	result.m[2][1] = 2.0f * (yz - wx);
	result.m[2][2] = 1.0f - 2.0f * (x2 + y2);
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	
	return result;
}




// Quaternionの成分をウィンドウに表示する
void DrawQuaternion(int x, int y, const Quaternion& q, const char* label) {
	Novice::ScreenPrintf(x, y, "%.2f", q.x);
	Novice::ScreenPrintf(x + 50, y, "%.2f", q.y);
	Novice::ScreenPrintf(x + 100, y, "%.2f", q.z);
	Novice::ScreenPrintf(x + 150, y, "%.2f", q.w);
	Novice::ScreenPrintf(x + 220, y, ": %s", label);
}

// float値をウィンドウに表示する
void DrawFloat(int x, int y, float value, const char* label) {
	Novice::ScreenPrintf(x, y, "%.2f", value);
	Novice::ScreenPrintf(x + 220, y, ": %s", label);
}

// Vector3の成分をウィンドウに表示する
void DrawVector3(int x, int y, const Vector3& v, const char* label) {
	Novice::ScreenPrintf(x, y, "%.2f", v.x);
	Novice::ScreenPrintf(x + 50, y, "%.2f", v.y);
	Novice::ScreenPrintf(x + 100, y, "%.2f", v.z);
	Novice::ScreenPrintf(x + 220, y, ": %s", label);
}

// Matrix4x4の成分をウィンドウに表示する
void DrawMatrix4x4(int x, int y, const Matrix4x4& m, const char* label) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			
			Novice::ScreenPrintf(x + j * 60, y + i * 20, "%.3f", m.m[i][j]);
		}
	}
	
	Novice::ScreenPrintf(x + 260, y + 15, ": %s", label);
}


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	
	Quaternion q1 = { 2.0f, 3.0f, 4.0f, 1.0f };
	Quaternion q2 = { 1.0f, 3.0f, 5.0f, 2.0f };

	Quaternion identity = IdentityQuaternion();
	Quaternion conj = Conjugate(q1);
	Quaternion inv = Inverse(q1);
	Quaternion normal = Normalize(q1);
	Quaternion mul1 = Multiply(q1, q2);
	Quaternion mul2 = Multiply(q2, q1);
	float norm = Norm(q1);
	

	Vector3 axis = { 1.0f, 0.4f, -0.2f }; 
	float angle = 0.45f;                 


	// 回転Quaternionの生成
	Quaternion rotation = MakeRotateAxisAngleQuaternion(axis, angle);
	
	// 回転対象の点
	Vector3 pointY = { 2.15f, -1.04f, 1.30f }; 
	
	// Quaternionから回転行列を生成
	Matrix4x4 rotateMatrix = MakeRotateMatrix(rotation);
	
	// Quaternionで回転させたベクトル
	Vector3 rotateByQuaternion = RotateVector(pointY, rotation);
	
	// 回転行列で変換したベクトル
	Vector3 rotateByMatrix = Transform(pointY, rotateMatrix);


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

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///


		int y = 50;
		int step = 25;
		int xStart = 50;

		DrawQuaternion(xStart, y, identity, "Identity");
		y += step;
		DrawQuaternion(xStart, y, conj, "Conjugate");
		y += step;
		DrawQuaternion(xStart, y, inv, "Inverse");
		y += step;
		DrawQuaternion(xStart, y, normal, "Normalize");
		y += step;
		DrawQuaternion(xStart, y, mul1, "Multiply(q1, q2)");
		y += step;
		DrawQuaternion(xStart, y, mul2, "Multiply(q2, q1)");
		y += step;
		DrawFloat(xStart, y, norm, "Norm");
		y += step;
		y += step; 

		
		DrawQuaternion(xStart, y, rotation, "rotation (MakeRotateAxisAngleQuaternion)");
		y += 2 * step;

		DrawMatrix4x4(xStart, y, rotateMatrix, "rotateMatrix (MakeRotateMatrix)");
		y += 5 * step;
		
		DrawVector3(xStart, y, pointY, "pointY");
		y += step;

		DrawVector3(xStart, y, rotateByQuaternion, "rotateByQuaternion (RotateVector)");
		y += step;
		
		DrawVector3(xStart, y, rotateByMatrix, "rotateByMatrix (Transform)");


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