#include <Novice.h>
#include <cmath> // std::sqrt, std::abs 用

const char kWindowTitle[] = "学籍番号";

// Quaternion構造体の定義
struct Quaternion {
	float x; // 虚部 i の係数
	float y; // 虚部 j の係数
	float z; // 虚部 k の係数
	float w; // 実部
};

// --- Quaternionに関する関数群 ---

// Quaternionの積
Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs) {
	Quaternion result;
	result.w = lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z;
	result.x = lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y;
	result.y = lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x;
	result.z = lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w;
	return result;
}

// 単位Quaternionを返す (実部が 1, 虚部が 0)
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
	if (norm < 1e-6f) { // ゼロ割を避ける
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
	if (normSquared < 1e-6f) { // ゼロ割を避ける
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

// --- Noviceでの表示用関数 ---

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


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	// --- 課題の実行例の定義 ---
	Quaternion q1 = { 2.0f, 3.0f, 4.0f, 1.0f };
	Quaternion q2 = { 1.0f, 3.0f, 5.0f, 2.0f };

	Quaternion identity = IdentityQuaternion();
	Quaternion conj = Conjugate(q1);
	Quaternion inv = Inverse(q1);
	Quaternion normal = Normalize(q1);
	Quaternion mul1 = Multiply(q1, q2);
	Quaternion mul2 = Multiply(q2, q1);
	float norm = Norm(q1);

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

		// スライドの実行結果と同様の表示
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