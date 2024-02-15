#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"

#include "SpriteCommon.h"
#include "Sprite.h"

#include "ImGuiManager.h" 
// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

    //ポインタ置き場
    Input* input_ = nullptr;
    WinApp* winApp_ = nullptr;
    DirectXCommon* dxCommon_ = nullptr;

    //WindowsAPIの初期化
    winApp_ = new WinApp();
    winApp_->Initialize();

    // DirectX初期化処理　ここから
    dxCommon_ = new DirectXCommon();
    dxCommon_->Initialize(winApp_);
    // DirectX初期化処理　ここまで

    //Input生成、初期化
    input_ = new Input();
    input_->Initialize(winApp_);

    //imguiの初期化処理
    ImGuiManager* imgui = ImGuiManager::Create();
    ImGuiManager::Initialize(winApp_->GetHwnd(), dxCommon_);

    //スプライトコモン
    SpriteCommon* spriteCommon = new SpriteCommon();
    spriteCommon->Initialize(dxCommon_);

    //スプライト
    Sprite* sprite = new Sprite();
    sprite->Initialize(dxCommon_,spriteCommon);

    // ゲームループ
    while (true) {
        if (winApp_->Update() == true) {
            break;
        }
        ImGuiManager::NewFrame();
        imgui->ShowDemo();


        input_->Update();


       //更新前処理
        ImGuiManager::CreateCommand();
        dxCommon_->PreDraw();

        sprite->Draw();

        //更新後処理
        ImGuiManager::CommandsExcute(dxCommon_->GetCommandList());
        dxCommon_->PostDraw();
    }

    delete sprite;
    delete spriteCommon;

    delete imgui;

    //解放処理
    delete input_;
    // DirectX解放
    delete dxCommon_;
    //WindowsAPI解放
    winApp_->Finalize();
    delete winApp_;

    return 0;
}
