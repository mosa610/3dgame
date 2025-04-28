#include "SceneManager.h"

void SceneManager::Update(float elapsedTime)
{
    if (nextScene != nullptr)
    {
        //�Â��V�[�����I��
        Clear();

        //�V�����V�[����ݒ�
        currentScene = nextScene;
        nextScene = nullptr;

        //�V�[��������
        if (currentScene->IsReady() == false)
        {
            currentScene->Initialize();
        }
    }

    if (currentScene != nullptr)
    {
        currentScene->Update(elapsedTime);
    }
}

void SceneManager::Render(float elapsedTime)
{
    if (currentScene != nullptr)
    {
        currentScene->Render(elapsedTime);
    }
}

void SceneManager::DrawGUI()
{
    if (currentScene != nullptr)
    {
        currentScene->DrawGUI();
    }
}

void SceneManager::Clear()
{
    if (currentScene != nullptr)
    {
        currentScene->Finalize();
        delete currentScene;
        currentScene = nullptr;
    }
}

void SceneManager::ChangeScene(Scene* scene)
{
    //�V�����V�[����ݒ�
    nextScene = scene;
}
