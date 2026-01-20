#define NOMINMAX

#include <Novice.h>
#include <cmath>
#include <algorithm> // for std::max

const char kWindowTitle[] = "学籍番号";

// 定数
const float kPI = 3.1415926535f;

// 構造体の定義
struct Vector3 {
	float x;
	float y;
	float z;
};

struct Matrix4x4 {
	float m[4][4];
};

// ユーティリティ関数の定義

/// @brief ベクトルの長さを計算
float Length(const Vector3& v) {
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

/// @brief ベクトルを正規化
Vector3 Normalize(const Vector3& v) {
	float len = Length(v);
	if (len != 0.0f) {
		return { v.x / len, v.y / len, v.z / len };
	}
	return { 0.0f, 0.0f, 0.0f };
}

/// @brief 内積
float Dot(const Vector3& v1, const Vector3& v2) {
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

/// @brief 外積
Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	return {
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	};
}

/// @brief ベクトルとスカラーの積
Vector3 operator*(const Vector3& v, float s) {
	return { v.x * s, v.y * s, v.z * s };
}
Vector3 operator*(float s, const Vector3& v) {
	return v * s;
}
/// @brief 単項マイナス演算子
Vector3 operator-(const Vector3& v) {
	return { -v.x, -v.y, -v.z };
}

/// @brief 単位行列を作成
Matrix4x4 Identity() {
	Matrix4x4 result{};
	result.m[0][0] = 1.0f;
	result.m[1][1] = 1.0f;
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;
	return result;
}

// 課題の関数：先生の指示通り修正
// 1. acosを使わず、Dotをcos、Crossのノルムをsinとして使用
// 2. MakeRotateAxisAngleを呼ばず、直接式に当てはめる

/// @brief ある方向からある方向へ向ける回転行列を作成
Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to) {
	Vector3 n; // 回転軸
	float cosTheta; // cos
	float sinTheta; // sin

	// 1. cosTheta を求める (内積)
	cosTheta = Dot(from, to);

	// 2. 回転軸と sinTheta を求める (外積)
	Vector3 cross = Cross(from, to);
	sinTheta = Length(cross);

	// 通常は外積の結果が回転軸になるが、正規化が必要
	// 回転軸 n を計算 (sinThetaが0に近い場合は計算できないため後述の分岐へ)
	if (sinTheta >= 1.0e-6f) {
		n = { cross.x / sinTheta, cross.y / sinTheta, cross.z / sinTheta };
	} else {
		// sinThetaが0に近い = 平行または逆平行
		n = { 0.0f, 0.0f, 0.0f }; // 仮
	}

	// 3. 場合分けして行列を作成

	// a. 同じ方向 (cosTheta が 1 に近い)
	if (cosTheta >= 1.0f - 1.0e-6f) {
		return Identity();
	}

	// b. 反対方向 (cosTheta が -1 に近い)
	// この場合、外積(sin)が0になり軸が定まらないため、垂直な任意の軸を探す
	if (cosTheta <= -1.0f + 1.0e-6f) {
		Vector3 tempAxis = { 0.0f, 1.0f, 0.0f };
		if (std::abs(from.y) > std::abs(from.x) && std::abs(from.y) > std::abs(from.z)) {
			// y成分が大きいならx軸を仮にするなどして垂直軸を作る
			tempAxis = { 1.0f, 0.0f, 0.0f };
		}
		n = Cross(from, tempAxis);
		n = Normalize(n);

		// 180度回転なので、cos=-1, sin=0 として計算
		cosTheta = -1.0f;
		sinTheta = 0.0f;
	}

	// c. 行列の計算 (ロドリゲスの回転公式の変形)
	// R = I + sin(n)x + (1-cos)(n)x^2
	// 成分展開して計算する

	float t = 1.0f - cosTheta;
	float nx = n.x;
	float ny = n.y;
	float nz = n.z;

	Matrix4x4 result{};

	result.m[0][0] = t * nx * nx + cosTheta;
	result.m[0][1] = t * nx * ny + sinTheta * nz;
	result.m[0][2] = t * nx * nz - sinTheta * ny;
	result.m[0][3] = 0.0f;

	result.m[1][0] = t * nx * ny - sinTheta * nz;
	result.m[1][1] = t * ny * ny + cosTheta;
	result.m[1][2] = t * ny * nz + sinTheta * nx;
	result.m[1][3] = 0.0f;

	result.m[2][0] = t * nx * nz + sinTheta * ny;
	result.m[2][1] = t * ny * nz - sinTheta * nx;
	result.m[2][2] = t * nz * nz + cosTheta;
	result.m[2][3] = 0.0f;

	result.m[3][0] = 0.0f;
	result.m[3][1] = 0.0f;
	result.m[3][2] = 0.0f;
	result.m[3][3] = 1.0f;

	return result;
}

// 確認用出力関数
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x, y, "%s", label);
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			Novice::ScreenPrintf(x + j * 60, y + (i + 1) * 20, "%6.3f", matrix.m[i][j]);
		}
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

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

		const int kRowHeight = 100;

		// 先生のスライドに合わせたベクトル設定
		Vector3 from0 = Normalize({ 1.0f, 0.7f, 0.5f });
		Vector3 to0 = -from0;
		Vector3 from1 = Normalize({ -0.6f, 0.9f, 0.2f });
		Vector3 to1 = Normalize({ 0.4f, 0.7f, -0.5f });

		// rotateMatrix0: (1,0,0) から (-1,0,0) への回転
		// ※ここを修正しないとスライドの結果と一致しません
		Matrix4x4 rotateMatrix0 = DirectionToDirection(
			Normalize({ 1.0f, 0.0f, 0.0f }),
			Normalize({ -1.0f, 0.0f, 0.0f })
		);

		// rotateMatrix1: from0 から to0 への回転
		Matrix4x4 rotateMatrix1 = DirectionToDirection(from0, to0);

		// rotateMatrix2: from1 から to1 への回転
		Matrix4x4 rotateMatrix2 = DirectionToDirection(from1, to1);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		MatrixScreenPrintf(0, 0, rotateMatrix0, "rotateMatrix0");
		MatrixScreenPrintf(0, kRowHeight, rotateMatrix1, "rotateMatrix1");
		MatrixScreenPrintf(0, kRowHeight * 2, rotateMatrix2, "rotateMatrix2");

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