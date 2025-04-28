#pragma once

#include "Scene.h"

//�V�[���}�l�[�W���[
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
    //�B��̃C���X�^���X
    static SceneManager& Instance()
    {
        static SceneManager instance;
        return instance;
    }

    //�X�V����
    void Update(float elapsedTime);

    //�`�揈��
    void Render(float elapsedTime);

    void DrawGUI();

    //�V�[���N���A
    void Clear();

    //�V�[���؂�ւ�
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