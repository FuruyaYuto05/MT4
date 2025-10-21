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
	return { 0.0f, 0.0f, 0.0f }; // ゼロベクトルは正規化できない
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

/// @brief ベクトルとスカラーの積
Vector3 operator*(float s, const Vector3& v) {
	return v * s;
}

/// @brief ベクトルの足し算
Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

/// @brief ベクトルの引き算
Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
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

/// @brief 4x4行列の乗算
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result{};
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			result.m[i][j] = 0.0f;
			for (int k = 0; k < 4; ++k) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
}

/// @brief 任意軸回転行列を作成 (ロドリゲスの回転公式)
Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle) {
	Matrix4x4 result = Identity();
	float c = std::cos(angle);
	float s = std::sin(angle);
	float t = 1.0f - c;

	result.m[0][0] = t * axis.x * axis.x + c;
	result.m[0][1] = t * axis.x * axis.y + s * axis.z;
	result.m[0][2] = t * axis.x * axis.z - s * axis.y;
	result.m[1][0] = t * axis.x * axis.y - s * axis.z;
	result.m[1][1] = t * axis.y * axis.y + c;
	result.m[1][2] = t * axis.y * axis.z + s * axis.x;
	result.m[2][0] = t * axis.x * axis.z + s * axis.y;
	result.m[2][1] = t * axis.y * axis.z - s * axis.x;
	result.m[2][2] = t * axis.z * axis.z + c;

	return result;
}

// 課題の関数

/// @brief ある方向からある方向へ向ける回転行列を作成
/// @param from 回転前の方向ベクトル (正規化されていることが望ましい)
/// @param to 回転後の方向ベクトル (正規化されていることが望ましい)
/// @return 回転行列 (Matrix4x4)
Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to) {
	// fromとtoが正規化されていることを前提とする

	// 1. 回転軸 (Axis of Rotation) を求める
	// fromとtoの外積が回転軸となる
	Vector3 axis = Cross(from, to);
	
	// 2. 回転角度 (Angle of Rotation) を求める
	// fromとtoの内積から角度を求める
	float dot = Dot(from, to);

	// cos(angle) = dot(from, to) / (|from|*|to|)。fromとtoは正規化されているため、|from|=|to|=1
	// 浮動小数点誤差により、dotが1.0や-1.0をわずかに超える場合があるため、クランプする
	float safeDot = std::max(-1.0f, std::min(1.0f, dot));
	float angle = std::acos(safeDot);

	// 3. 回転行列を作成

	// a. fromとtoが同じ方向の場合 (angle = 0)
	if (std::abs(angle) < 1.0e-6f) {
		return Identity();
	}

	// b. fromとtoが反対方向の場合 (angle = PI)
	if (std::abs(angle - kPI) < 1.0e-6f) {
		// 回転軸が一意に定まらないため、任意の垂直な軸を選ぶ
		// 例: from = {0, 1, 0} の場合、axis = {1, 0, 0} を選ぶ
		// from = {1, 0, 0} の場合、axis = {0, 1, 0} を選ぶ
		// fromと垂直な適当な軸を探す
		Vector3 tempAxis = { 0.0f, 1.0f, 0.0f }; // 仮の軸
		if (std::abs(from.y) > std::abs(from.x) && std::abs(from.y) > std::abs(from.z)) {
			// y成分が最も大きい場合、x軸を仮の軸とする
			tempAxis = { 1.0f, 0.0f, 0.0f };
		}
		axis = Cross(from, tempAxis);
		axis = Normalize(axis); // 垂直な軸を正規化
		return MakeRotateAxisAngle(axis, kPI);
	}

	// c. 通常の回転
	// 回転軸を正規化
	axis = Normalize(axis);

	// 任意軸回転行列を生成
	return MakeRotateAxisAngle(axis, angle);
}

// 確認用出力関数 (Novice::ScreenPrintfを模倣)
void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
	Novice::ScreenPrintf(x, y, "%s", label);
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			// 小数点以下3桁で表示 (完成イメージに合わせるため)
			Novice::ScreenPrintf(x + j * 90, y + (i + 1) * 20, "%7.3f", matrix.m[i][j]);
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

		// 完成イメージのコードを再現
		const int kRowHeight = 100;

		Vector3 from0 = Normalize({ 1.0f, 0.7f, 0.5f });
		Vector3 to0 = from0 * (-1.0f); // -from0
		Vector3 from1 = Normalize({ -0.6f, 0.9f, 0.2f });
		Vector3 to1 = Normalize({ 0.4f, 0.7f, -0.5f });

		// rotateMatrix0: from0からto0 (-from0) への回転
		Matrix4x4 rotateMatrix0 = DirectionToDirection(from0, to0);

		// rotateMatrix1: from1から {0.0f, 0.0f, 1.0f} への回転
		Vector3 targetVector1 = Normalize({ -1.0f, 0.0f, 0.0f }); // Completion image uses {-1.0f, 0.0f, 0.0f}

		Matrix4x4 rotateMatrix1 = DirectionToDirection(from1, targetVector1);

		// rotateMatrix2: from1からto1への回転
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