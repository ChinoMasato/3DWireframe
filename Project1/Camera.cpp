/* Camera.cpp */
#define NOMINMAX
#include "Camera.h"
#include "DxLib.h"
#include "CameraMath.h"
#include "Common.h"
#include <math.h>
#include <algorithm>
#include <stdexcept>
#include <string>
#include <sstream>
#include <iomanip>
#include "Logger.h"

// --- 匿名名前空間 ---
namespace { /* ... 定義・ヘルパー関数省略 ... */
    inline Vector4D VectorLerp4D(const Vector4D& start, const Vector4D& end, float t) { return { start.x + (end.x - start.x) * t, start.y + (end.y - start.y) * t, start.z + (end.z - start.z) * t, start.w + (end.w - start.w) * t }; }
    const int INSIDE = 0; const int LEFT = 1; const int RIGHT = 2; const int BOTTOM = 4; const int TOP = 8; const int OUTCODE_NEAR = 16; const int OUTCODE_FAR = 32;
    int ComputeOutCode(const Vector4D& p) { int code = INSIDE; if (p.x < -p.w) code |= LEFT; else if (p.x > p.w) code |= RIGHT; if (p.y < -p.w) code |= BOTTOM; else if (p.y > p.w) code |= TOP; if (p.z < 0.0f) code |= OUTCODE_NEAR; else if (p.z > p.w) code |= OUTCODE_FAR; return code; }
    bool ClipLineCohenSutherland(Vector4D& p1_clip, Vector4D& p2_clip) { /* ... 省略 ... */ int outcode1 = ComputeOutCode(p1_clip); int outcode2 = ComputeOutCode(p2_clip); const int MAX_ITERATIONS = 10; int iterations = 0; while (iterations < MAX_ITERATIONS) { iterations++; if ((outcode1 | outcode2) == 0) return true; else if ((outcode1 & outcode2) != 0) return false; else { int outcode_outside = (outcode1 != 0) ? outcode1 : outcode2; float t = 0.0f; Vector4D intersection_point; float dx = p2_clip.x - p1_clip.x, dy = p2_clip.y - p1_clip.y; float dz = p2_clip.z - p1_clip.z, dw = p2_clip.w - p1_clip.w; float denominator; if (outcode_outside & LEFT) { denominator = dx + dw; if (fabsf(denominator) < 1e-6f) return false; t = (-p1_clip.x - p1_clip.w) / denominator; } else if (outcode_outside & RIGHT) { denominator = dx - dw; if (fabsf(denominator) < 1e-6f) return false; t = (p1_clip.w - p1_clip.x) / denominator; } else if (outcode_outside & BOTTOM) { denominator = dy + dw; if (fabsf(denominator) < 1e-6f) return false; t = (-p1_clip.y - p1_clip.w) / denominator; } else if (outcode_outside & TOP) { denominator = dy - dw; if (fabsf(denominator) < 1e-6f) return false; t = (p1_clip.w - p1_clip.y) / denominator; } else if (outcode_outside & OUTCODE_NEAR) { if (fabsf(dz) < 1e-6f) return false; t = -p1_clip.z / dz; } else if (outcode_outside & OUTCODE_FAR) { denominator = dz - dw; if (fabsf(denominator) < 1e-6f) return false; t = (p1_clip.w - p1_clip.z) / denominator; } else return false; if (t < 0.0f || t > 1.0f) return false; intersection_point = VectorLerp4D(p1_clip, p2_clip, t); if (outcode_outside == outcode1) { p1_clip = intersection_point; outcode1 = ComputeOutCode(p1_clip); } else { p2_clip = intersection_point; outcode2 = ComputeOutCode(p2_clip); } } } return false; }
    static const float MOVE_SPEED = 2.5f;
    static const float ROTATION_SENSITIVITY = 5.0f;
    static const float MOUSE_ANGLE_RATE = ONE_DEGREE * 0.1f;
    static const float ROLL_SPEED = 2.0f * ONE_DEGREE;
}

// --- Camera クラス メンバー関数の実装 ---
Camera::Camera() : position{ 0.0f, 0.0f, -50.0f }, orientation() {
    SetMousePoint(static_cast<int>(WINDOW_WIDTH / 2), static_cast<int>(WINDOW_HEIGHT / 2));
}
Camera::~Camera() {}

Vector3D Camera::GetPosition() const { return position; }
Matrix Camera::GetViewMatrix() const { /* ... 省略 ... */ Quaternion invOrientation = orientation.Conjugate(); Matrix invRotationMatrix = invOrientation.ToRotationMatrix(); Matrix invTranslate = Matrix::Identity(); invTranslate.m[3][0] = -position.x; invTranslate.m[3][1] = -position.y; invTranslate.m[3][2] = -position.z; Matrix viewMatrix = MatrixMultiply(invTranslate, invRotationMatrix); return viewMatrix; }
Matrix Camera::GetProjectionMatrix() const { return PerspectiveFovLH(fovY, aspectRatio, nearZ, farZ); }
Vector3D Camera::GetForwardVector() const { Matrix rotMat = orientation.ToRotationMatrix(); return VEC3Transform({ 0.0f, 0.0f, 1.0f }, rotMat); }
Vector3D Camera::GetRightVector() const { Matrix rotMat = orientation.ToRotationMatrix(); return VEC3Transform({ 1.0f, 0.0f, 0.0f }, rotMat); }
Vector3D Camera::GetUpVector() const { Matrix rotMat = orientation.ToRotationMatrix(); return VEC3Transform({ 0.0f, 1.0f, 0.0f }, rotMat); }
Vector3D WorldToScreen(const Vector3D& worldPos, const Matrix& viewProjMatrix) { /* ... 省略 ... */ Vector4D clipPos = VEC4Transform({ worldPos.x, worldPos.y, worldPos.z, 1.0f }, viewProjMatrix); if (fabsf(clipPos.w) < 1e-6f || clipPos.z < 0.0f || clipPos.z > clipPos.w) { return { -1.0f, -1.0f, 0.0f }; } Vector3D ndc = { clipPos.x / clipPos.w, clipPos.y / clipPos.w, clipPos.z / clipPos.w }; float hW = WINDOW_WIDTH / 2.0f; float hH = WINDOW_HEIGHT / 2.0f; int sx = static_cast<int>(ndc.x * hW + hW); int sy = static_cast<int>(-ndc.y * hH + hH); return { (float)sx, (float)sy, ndc.z }; }

void Camera::Draw(const std::vector<std::vector<Vector3D>>& worldLines) { /* ... 省略 ... */
    Matrix viewMatrix = GetViewMatrix(); Matrix projMatrix = GetProjectionMatrix(); Matrix viewProjMatrix = MatrixMultiply(viewMatrix, projMatrix);
    for (const auto& line : worldLines) { if (line.size() < 2) continue; Vector4D p1_clip = VEC4Transform({ line[0].x, line[0].y, line[0].z, 1.0f }, viewProjMatrix); Vector4D p2_clip = VEC4Transform({ line[1].x, line[1].y, line[1].z, 1.0f }, viewProjMatrix); if (ClipLineCohenSutherland(p1_clip, p2_clip)) { Vector3D p1_ndc = { 0,0,0 }, p2_ndc = { 0,0,0 }; if (fabsf(p1_clip.w) > 1e-6f && fabsf(p2_clip.w) > 1e-6f) { p1_ndc = { p1_clip.x / p1_clip.w, p1_clip.y / p1_clip.w, p1_clip.z / p1_clip.w }; p2_ndc = { p2_clip.x / p2_clip.w, p2_clip.y / p2_clip.w, p2_clip.z / p2_clip.w }; float hW = WINDOW_WIDTH / 2.f, hH = WINDOW_HEIGHT / 2.f; int sx1 = (int)(p1_ndc.x * hW + hW), sy1 = (int)(-p1_ndc.y * hH + hH); int sx2 = (int)(p2_ndc.x * hW + hW), sy2 = (int)(-p2_ndc.y * hH + hH); DrawLine(sx1, sy1, sx2, sy2, GetColor(255, 255, 255)); } } }
    if (lastWorldMoveOffset.LengthSq() > 1e-12f) { Vector3D moveDir = lastWorldMoveOffset.Normalized(); Vector3D moveEndPos = position + moveDir * 10.0f; Vector3D startScreen = WorldToScreen(position, viewProjMatrix); Vector3D endScreen = WorldToScreen(moveEndPos, viewProjMatrix); if (startScreen.x >= 0.0f && endScreen.x >= 0.0f) { DrawLine((int)startScreen.x, (int)startScreen.y, (int)endScreen.x, (int)endScreen.y, GetColor(0, 255, 255)); } }
}


void Camera::Update() {
    // --- 1. マウスによる視点回転 ---
    int currentMouseX = 0, currentMouseY = 0;
    GetMousePoint(&currentMouseX, &currentMouseY);
    const int centerX = static_cast<int>(WINDOW_WIDTH / 2);
    const int centerY = static_cast<int>(WINDOW_HEIGHT / 2);
    int mouseMoveX = currentMouseX - centerX;
    int mouseMoveY = currentMouseY - centerY;
    lastMouseMoveX = mouseMoveX; lastMouseMoveY = mouseMoveY;

    // ローカル軸周りの回転角度を計算 (符号は +=)
    lastYawAngle = mouseMoveX * MOUSE_ANGLE_RATE; // ローカルY(Up)周り
    lastPitchAngle = mouseMoveY * MOUSE_ANGLE_RATE; // ローカルX(Right)周り

    // --- 2. キーボードによるロール回転 ---
    lastRollAngle = 0.0f;
    if (CheckHitKey(KEY_INPUT_E)) lastRollAngle += ROLL_SPEED;
    if (CheckHitKey(KEY_INPUT_Q)) lastRollAngle -= ROLL_SPEED;

    // --- 3. 回転の適用 ---
    // 現在のローカル軸を取得
    currentUp = GetUpVector();
    currentRight = GetRightVector();
    currentForward = GetForwardVector();

    // 各ローカル軸周りの差分回転クォータニオンを生成
    Quaternion yawDelta = Quaternion::FromAxisAngle(currentUp, lastYawAngle);
    Quaternion pitchDelta = Quaternion::FromAxisAngle(currentRight, lastPitchAngle);
    Quaternion rollDelta = Quaternion::FromAxisAngle(currentForward, lastRollAngle);

    // !! 修正: 差分回転を先に合成し、現在の姿勢に前から適用 !!
    Quaternion deltaRotation = rollDelta * pitchDelta * yawDelta;
    orientation = deltaRotation * orientation; // ワールド空間での回転として適用
    orientation.Normalize(); // 正規化

    // ピッチ角制限は削除

    // --- 4. カメラの向きベクトル再計算 ---
    currentForward = GetForwardVector();
    currentRight = GetRightVector();
    currentUp = GetUpVector();

    // --- 5. キーボードによる移動入力 ---
    lastMoveForward = 0.0f; lastMoveRight = 0.0f; lastMoveUp = 0.0f;
    if (CheckHitKey(KEY_INPUT_W)) lastMoveForward += 1.0f;
    if (CheckHitKey(KEY_INPUT_S)) lastMoveForward -= 1.0f;
    if (CheckHitKey(KEY_INPUT_D)) lastMoveRight += 1.0f;
    if (CheckHitKey(KEY_INPUT_A)) lastMoveRight -= 1.0f;
    if (CheckHitKey(KEY_INPUT_SPACE)) lastMoveUp += 1.0f;
    if (CheckHitKey(KEY_INPUT_LCONTROL)) lastMoveUp -= 1.0f;

    // --- 6. 移動量の計算と位置更新 ---
    lastWorldMoveOffset = { 0.0f, 0.0f, 0.0f };
    lastWorldMoveOffset += currentForward * lastMoveForward * MOVE_SPEED;
    lastWorldMoveOffset += currentRight * lastMoveRight * MOVE_SPEED;
    lastWorldMoveOffset += currentUp * lastMoveUp * MOVE_SPEED;
    position += lastWorldMoveOffset;

    // --- 7. マウスカーソルを画面中央に戻す ---
    SetMousePoint(centerX, centerY);

    // --- デバッグログ出力 ---
    LogDebug(GetDetailedDebugInfo());
}

// デバッグ情報 (画面表示用)
std::string Camera::GetDebugInfo() const { /* ... 省略 ... */
    std::stringstream ss; ss << std::fixed << std::setprecision(2); ss << "Pos:(" << position.x << "," << position.y << "," << position.z << ") "; ss << "Mouse:(" << lastMouseMoveX << "," << lastMouseMoveY << ") "; float rollInput = 0.0f; if (CheckHitKey(KEY_INPUT_E)) rollInput += 1.0f; if (CheckHitKey(KEY_INPUT_Q)) rollInput -= 1.0f; ss << "Key: F" << lastMoveForward << " R" << lastMoveRight << " U" << lastMoveUp << " Roll" << rollInput; return ss.str();
}

// デバッグ情報 (ログ用)
std::string Camera::GetDetailedDebugInfo() const { /* ... 省略 ... */
    std::stringstream ss; ss << std::fixed << std::setprecision(4); ss << "Pos(x:" << position.x << ",y:" << position.y << ",z:" << position.z << ") "; ss << "Ori(x:" << orientation.x << ",y:" << orientation.y << ",z:" << orientation.z << ",w:" << orientation.w << ") "; ss << "MouseIn(x:" << lastMouseMoveX << ",y:" << lastMouseMoveY << ") "; ss << "RotAngle(Yaw:" << lastYawAngle * 180 / PI << ",Pitch:" << lastPitchAngle * 180 / PI << ",Roll:" << lastRollAngle * 180 / PI << ") "; float rollInput = 0.0f; if (CheckHitKey(KEY_INPUT_E)) rollInput += 1.0f; if (CheckHitKey(KEY_INPUT_Q)) rollInput -= 1.0f; ss << "KeyIn(F:" << lastMoveForward << ",R:" << lastMoveRight << ",U:" << lastMoveUp << ",Roll:" << rollInput << ") "; ss << "AxisF(x:" << currentForward.x << ",y:" << currentForward.y << ",z:" << currentForward.z << ") "; ss << "AxisR(x:" << currentRight.x << ",y:" << currentRight.y << ",z:" << currentRight.z << ") "; ss << "AxisU(x:" << currentUp.x << ",y:" << currentUp.y << ",z:" << currentUp.z << ") "; ss << "MoveOffset(x:" << lastWorldMoveOffset.x << ",y:" << lastWorldMoveOffset.y << ",z:" << lastWorldMoveOffset.z << ")"; return ss.str();
}