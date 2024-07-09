#include "Framework.h"
#include "SceneManager.h"
#include "SceneTest.h"

Framework::Framework(HWND hwnd)
    :hwnd(hwnd)
	, graphics(hwnd)
{
	SceneManager::Instance().ChangeScene(new SceneTest);
}

Framework::~Framework()
{
	SceneManager::Instance().Clear();
}

void Framework::Update(float elapsedTime)
{
#ifdef USE_IMGUI
	graphics.Get_ImGui_renderer()->NewFrame();
#endif

#ifdef USE_IMGUI
	ImGui::Begin("ImGUI");
	ImGui::SetNextTreeNodeOpen(true, ImGuiSetCond_Once);
	if (ImGui::TreeNode("test01")) {
		ImGui::TreePop();
	}
	ImGui::End();
#endif



	SceneManager::Instance().Update(elapsedTime);
}

void Framework::Render(float elapsedTime)
{
	SceneManager::Instance().Render(elapsedTime);

	graphics.Get_ImGui_renderer()->Render(graphics.Get_device_context());


	UINT sync_interval{ 0 };
	graphics.Get_swap_chain()->Present(sync_interval, 0);
}

void Framework::calculate_frame_stats()
{
	static uint32_t frames = 0;
	static float elapsed_time = 0.0f;

	if (++frames, (tictoc.time_stamp() - elapsed_time) >= 1.0f)
	{
		float fps = static_cast<float>(frames);
		std::wostringstream outs;
		outs.precision(6);
		outs << APPLICATION_NAME << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)";
		SetWindowTextW(hwnd, outs.str().c_str());

		frames = 0;
		elapsed_time += 1.0f;
	}
}

int Framework::Run()
{
    MSG msg = {};




	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			tictoc.tick();
			calculate_frame_stats();
			Update(tictoc.time_interval());
			Render(tictoc.time_interval());
		}
	}

#ifdef USE_IMGUI

#endif


	return static_cast<int>(msg.wParam);
}

LRESULT Framework::Handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
#ifdef USE_IMGUI
	graphics.Get_ImGui_renderer()->HandleMessage(hwnd, msg, wparam, lparam);
#endif

	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps{};
		BeginPaint(hwnd, &ps);

		EndPaint(hwnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE)
		{
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
		break;
	case WM_ENTERSIZEMOVE:
		tictoc.stop();
		break;
	case WM_EXITSIZEMOVE:
		tictoc.start();
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}
