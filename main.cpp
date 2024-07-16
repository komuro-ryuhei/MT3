#include <Novice.h>
#include "MyMath.h"
#include "MT3.h"
#include <ImGui.h>

const char kWindowTitle[] = "LC1B_13_コムロ_リュウヘイ";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, 1280, 720);

    // キー入力結果を受け取る箱
    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    struct Spring {
        // アンカー
        Vector3 ancor;
        float naturalLength; // 自然長
        float stiffness;     // 剛性、ばね定数k
        float dampingCoefficient; // 減衰係数
    };

    Spring spring{};
    spring.ancor = { 0.0f,0.0f,0.0f };
    spring.naturalLength = 1.0f;
    spring.stiffness = 100.0f;
    spring.dampingCoefficient = 2.0f;

    struct Ball {
        Vector3 position;    // ボールの位置
        Vector3 velocity;    // ボールの速度
        Vector3 aceleration; // ボールの加速度
        float mass;          // ボールの質量
        float radius;        // ボールの半径
        unsigned int color;  // ボールの色
    };

    Ball ball{};
    ball.position = { 1.2f,0.0f,0.0f };
    ball.mass = 2.0f;
    ball.radius = 0.05f;
    ball.color = 0x0000FFFF;

    Sphere sphere;
    sphere.center = ball.position;
    sphere.radius = 0.1f;

    bool isSpring = false;

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

        CaluculationMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });

        if (isSpring) {
            float deltaTime = 1.0f / 60.0f;

            Vector3 diff = ball.position - spring.ancor;
            float length = Length(diff);
            if (length != 0.0f) {
                Vector3 direction = Normalize(diff);
                Vector3 restPosition = spring.ancor + direction * spring.naturalLength;
                Vector3 displacement = ball.position - restPosition;
                Vector3 restoringForce = displacement * -spring.stiffness;

                // 減衰抵抗を計算する
                Vector3 dampingForce = ball.velocity * -spring.dampingCoefficient;

                // 減衰抵抗も加味して、物体にかかる力を決定する
                Vector3 force = restoringForce + dampingForce;

                // 加速度の計算
                ball.aceleration = force / ball.mass;
            }

            // 加速度も速度もどちらも秒を基準とした値
            // それが、 1 / 60 秒間(deltaTime)適応したと考える
            ball.velocity += ball.aceleration * deltaTime;
            ball.position += ball.velocity * deltaTime;

            // ボールの位置を更新
            sphere.center = ball.position;
        }

        ImGui::Checkbox("IsSpring", &isSpring);

        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///

        DrawGrid(worldViewProjectionMatrix, worldViewportMatrix);
        DrawSphere(sphere, worldViewProjectionMatrix, worldViewportMatrix, ball.color);
        DrawLine(spring.ancor, sphere.center, worldViewProjectionMatrix, worldViewportMatrix, 0xFFFFFFFF);

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
