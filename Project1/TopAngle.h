#pragma once
#include "Vector.h" // Vector3D を使用するため
#include <vector>   // std::vector を使用するため

/*
 * TopAngle.h
 * 役割: 画面左上にワールドを上から見た図（トップダウンビュー）を描画するクラスの定義です。
 *       監視対象のカメラオブジェクトへのポインタを持ち、そのカメラの位置や向き、
 *       および指定されたワールドオブジェクトを2Dで表示します。
 *
 * 最初のコードからの変更点・コメント:
 * - このファイルは最初のコードには存在せず、トップダウンビュー機能のために新規に追加されました。
 * - Cameraクラスへの依存があるため、前方宣言 (`class Camera;`) を使用して循環参照を防いでいます。
 * - 描画領域の設定値やヘルパー関数を定義しています。
 */

 // 前方宣言: Cameraクラスの定義を直接インクルードする代わりにクラス名だけを宣言
class Camera;

class TopAngle
{
public:
    // コンストラクタ: 監視対象のCameraオブジェクトへのポインタを受け取る
    TopAngle(Camera* cam);
    // デストラクタ
    ~TopAngle();

    // 描画関数: ワールドの線分データを受け取り、トップダウンビューを描画する
    void Draw(const std::vector<std::vector<Vector3D>>& worldLines);

private:
    Camera* camera; // 監視対象のカメラオブジェクトへのポインタ

    // --- トップダウンビューの表示設定 (定数) ---
    // これらはTopAngle.cppで定義されます。
    static const int VIEW_POS_X;     // ビュー領域の左上X座標 (スクリーン座標)
    static const int VIEW_POS_Y;     // ビュー領域の左上Y座標 (スクリーン座標)
    static const int VIEW_WIDTH;     // ビュー領域の幅 (ピクセル)
    static const int VIEW_HEIGHT;    // ビュー領域の高さ (ピクセル)
    static const float VIEW_SCALE;   // ワールド座標からビュー座標への拡大率
    static const float CAMERA_FOV_H; // カメラの水平視野角 (ラジアン) - 描画用
    static const float VIEW_RANGE;   // 視野角を示す線の長さ (ピクセル) - 描画用

    // ヘルパー関数: ワールド座標(X, Z)をトップダウンビューのスクリーン座標に変換
    Vector3D ConvertWorldToView(float worldX, float worldZ) const;
};