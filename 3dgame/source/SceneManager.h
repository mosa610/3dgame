#pragma once

#include "Scene.h"

//シーンマネージャー
class SceneManager
{
private:
    SceneManager() {}
    ~SceneManager() 
    {
        currentScene = nullptr;
        nextScene = nullptr;
    }
public:
    //唯一のインスタンス
    static SceneManager& Instance()
    {
        static SceneManager instance;
        return instance;
    }

    //更新処理
    void Update(float elapsedTime);

    //描画処理
    void Render(float elapsedTime);

    void DrawGUI();

    //シーンクリア
    void Clear();

    //シーン切り替え
    void ChangeScene(Scene* scene);

    void ResetShaderResource()
    {
        if (currentScene != nullptr)
        {
            currentScene->ResetShaderResource();
        }
    }

    void RemakeShaderResource(float width, float height)
    {
        if (currentScene != nullptr)
        {
            currentScene->RemakeShaderResource(width, height);
        }
    }

private:
    Scene* currentScene = nullptr;
    Scene* nextScene = nullptr;
};