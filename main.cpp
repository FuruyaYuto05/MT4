#include <Novice.h>
#include <cmath> 

const char kWindowTitle[] = "学籍番号";


struct Vector3 {
    float x;
    float y;
    float z;
};


struct Matrix4x4 {
    float m[4][4];
};

float Length(const Vector3& v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}


Vector3 Normalize(const Vector3& v) {
    float len = Length(v);
    if (len != 0.0f) {
        return { v.x / len, v.y / len, v.z / len };
    }
    return { 0.0f, 0.0f, 0.0f }; 
}


Matrix4x4 MakeIdentityMatrix() {
    return {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}


Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle) {

    Vector3 n = Normalize(axis);

    float c = std::cos(angle);
    float s = std::sin(angle);
    float oneMinusC = 1.0f - c;

    float nx = n.x;
    float ny = n.y;
    float nz = n.z;

    Matrix4x4 result;

    // 1行目
    result.m[0][0] = c + nx * nx * oneMinusC;
    result.m[0][1] = nx * ny * oneMinusC + nz * s;
    result.m[0][2] = nx * nz * oneMinusC - ny * s;
    result.m[0][3] = 0.0f;

    // 2行目
    result.m[1][0] = nx * ny * oneMinusC - nz * s;
    result.m[1][1] = c + ny * ny * oneMinusC;
    result.m[1][2] = ny * nz * oneMinusC + nx * s;
    result.m[1][3] = 0.0f;

    // 3行目
    result.m[2][0] = nx * nz * oneMinusC + ny * s;
    result.m[2][1] = ny * nz * oneMinusC - nx * s;
    result.m[2][2] = c + nz * nz * oneMinusC;
    result.m[2][3] = 0.0f;

    // 4行目
    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
}


void MatrixScreenPrintf(int x, int y, const Matrix4x4& matrix, const char* label) {
    Novice::ScreenPrintf(x, y, "%s", label);
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 4; ++col) {
            Novice::ScreenPrintf(x + col * 60, y + (row + 1) * 20, "%6.3f", matrix.m[row][col]);
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


    // 回転軸: (1, 1, 1)の方向
    Vector3 axis = Normalize({ 1.0f, 1.0f, 1.0f });
    // 回転角度: 0.44ラジアン
    float angle = 0.44f;
    // 結果を格納する行列
    Matrix4x4 rotateMatrix = MakeIdentityMatrix();
    // ----------------------------

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

       
        rotateMatrix = MakeRotateAxisAngle(axis, angle);

        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///

       
        MatrixScreenPrintf(0, 0, rotateMatrix, "rotateMatrix");




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