#include <Novice.h>
#include <cmath>

const char kWindowTitle[] = "学籍番号";

//==========================
//  Quaternion 構造体
//==========================
struct Quaternion {
    float x, y, z, w;
};

struct Vector3 {
    float x, y, z;
};

//==========================
//  正規化
//==========================
Quaternion Normalize(const Quaternion& q) {
    float len = std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
    return { q.x / len, q.y / len, q.z / len, q.w / len };
}

//==========================
//  軸角クォータニオン生成
//==========================
Quaternion MakeRotateAxisAngleQuaternion(const Vector3& axis, float angle) {
    float half = angle * 0.5f;
    float s = sinf(half);
    float c = cosf(half);

    return Normalize({ axis.x * s, axis.y * s, axis.z * s, c });
}

//==========================
//      Slerp
//==========================
Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t) {

    float dot = q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
    Quaternion q1c = q1;

    // 近くなるように反転
    if (dot < 0.0f) {
        dot = -dot;
        q1c = { -q1.x, -q1.y, -q1.z, -q1.w };
    }

    float theta = acosf(dot);
    float sinTheta = sinf(theta);

    if (sinTheta < 0.0001f) {
        // ほぼ同一 → 線形補間
        return {
            q0.x + t * (q1c.x - q0.x),
            q0.y + t * (q1c.y - q0.y),
            q0.z + t * (q1c.z - q0.z),
            q0.w + t * (q1c.w - q0.w),
        };
    }

    float scale0 = sinf((1 - t) * theta) / sinTheta;
    float scale1 = sinf(t * theta) / sinTheta;

    return {
        scale0 * q0.x + scale1 * q1c.x,
        scale0 * q0.y + scale1 * q1c.y,
        scale0 * q0.z + scale1 * q1c.z,
        scale0 * q0.w + scale1 * q1c.w
    };
}


//==========================
//        main
//==========================
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    Novice::Initialize(kWindowTitle, 1280, 720);

    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    //-------------------------------------------------------
    //   課題内容：Slerp の実行（スライドと同じ）
    //-------------------------------------------------------
    Quaternion rotation0 = MakeRotateAxisAngleQuaternion({ 0.71f, 0.71f, 0.0f }, 0.3f);
    Quaternion rotation1 = MakeRotateAxisAngleQuaternion({ 0.71f, 0.0f, 0.71f }, 3.141592f);

    float tList[5] = { 0.0f, 0.3f, 0.5f, 0.7f, 1.0f };
    Quaternion interpolated[5];

    for (int i = 0; i < 5; i++) {
        interpolated[i] = Slerp(rotation0, rotation1, tList[i]);
    }
    //-------------------------------------------------------

    while (Novice::ProcessMessage() == 0) {

        Novice::BeginFrame();

        memcpy(preKeys, keys, 256);
        Novice::GetHitKeyStateAll(keys);

     
        int y = 20;
        for (int i = 0; i < 5; i++) {
            Novice::ScreenPrintf(
                20, y,
                "interpolate%d : %.2f %.2f %.2f %.2f   Slerp(q0,q1,%.1ff)",
                i,
                interpolated[i].x, interpolated[i].y, interpolated[i].z, interpolated[i].w,
                tList[i]
            );
            y += 30;
        }
        //-------------------------------------------------------

        Novice::EndFrame();

        if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE]) break;
    }

    Novice::Finalize();
    return 0;
}
