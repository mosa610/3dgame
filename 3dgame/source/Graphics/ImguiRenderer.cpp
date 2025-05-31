#include "ImguiRenderer.h"
#include "..//imgui/IconsFontAwesome6.h"

ImGuiRenderer::ImGuiRenderer(HWND hwnd, ID3D11Device* device,
	ID3D11DeviceContext* immediate_context)
{

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	ImGui::StyleColorsDark();

	// When viewports enabled, tweak style for platform windows
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;

		style.WindowPadding = ImVec2(0.0f, 0.0f);      // ウィンドウ内の余白をゼロに
		style.FramePadding = ImVec2(0.0f, 0.0f);       // ボタンなどの内部パディング
		style.ItemSpacing = ImVec2(0.0f, 0.0f);        // UI要素間の間隔
		style.WindowBorderSize = 0.0f;                 // ウィンドウの枠線なし
		style.FrameBorderSize = 0.0f;                  // フレームの枠線なし
		style.TabBorderSize = 0.0f;                    // タブの枠線なし
		style.CellPadding = ImVec2(0.0f, 0.0f);        // テーブル内の余白
		style.IndentSpacing = 0.0f;                    // インデントの間隔
		style.DisplaySafeAreaPadding = ImVec2(0.0f, 0.0f); // 安全領域のパディング
	}

	ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());

	ImFontConfig config;
	config.MergeMode = true;
	config.PixelSnapH = true;
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };  // ← ここ重要

	io.Fonts->AddFontFromFileTTF("Data\\Font\\fa-solid-900.ttf", 16.0f, &config, icons_ranges);

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, immediate_context);
	ImGui::StyleColorsDark();
}

ImGuiRenderer::~ImGuiRenderer()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiRenderer::NewFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiRenderer::Render(ID3D11DeviceContext* context)
{
	ImGui::Render();

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		//SetForegroundWindow(hwnd);
	}
}

LRESULT ImGuiRenderer::HandleMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
		return true;

    return false;
}

void ImGuiRenderer::DockSpace()
{
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	window_flags |= ImGuiWindowFlags_NoBackground;  // 👈 背景を透明にする

	ImGui::Begin("DockSpace", &p_open, window_flags);

	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f),
		ImGuiDockNodeFlags_PassthruCentralNode |  // 中央部分を透過させる！
		ImGuiDockNodeFlags_NoDockingInCentralNode // （必要に応じて）
	);
}

bool ImGuiRenderer::UpdateMouseCursor()
{
	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
		return false;

	ImGuiMouseCursor imguiCursor = ImGui::GetMouseCursor();
	if (imguiCursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
	{
		// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
		::SetCursor(NULL);
	}
	else
	{
		// Show OS mouse cursor
		LPTSTR win32Cursor = IDC_ARROW;
		switch (imguiCursor)
		{
		case ImGuiMouseCursor_Arrow:        win32Cursor = IDC_ARROW; break;
		case ImGuiMouseCursor_TextInput:    win32Cursor = IDC_IBEAM; break;
		case ImGuiMouseCursor_ResizeAll:    win32Cursor = IDC_SIZEALL; break;
		case ImGuiMouseCursor_ResizeEW:     win32Cursor = IDC_SIZEWE; break;
		case ImGuiMouseCursor_ResizeNS:     win32Cursor = IDC_SIZENS; break;
		case ImGuiMouseCursor_ResizeNESW:   win32Cursor = IDC_SIZENESW; break;
		case ImGuiMouseCursor_ResizeNWSE:   win32Cursor = IDC_SIZENWSE; break;
		case ImGuiMouseCursor_Hand:         win32Cursor = IDC_HAND; break;
		case ImGuiMouseCursor_NotAllowed:   win32Cursor = IDC_NO; break;
		}
		::SetCursor(::LoadCursor(NULL, win32Cursor));
	}
	return true;
}

void ImGuiRenderer::UpdateMousePos()
{
	ImGuiIO& io = ImGui::GetIO();

	// Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
	if (io.WantSetMousePos)
	{
		POINT pos = { (int)io.MousePos.x, (int)io.MousePos.y };
		::ClientToScreen(hWnd, &pos);
		::SetCursorPos(pos.x, pos.y);
	}

	// Set mouse position
	io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	POINT pos;
	if (HWND active_window = ::GetForegroundWindow())
		if (active_window == hWnd || ::IsChild(active_window, hWnd))
			if (::GetCursorPos(&pos) && ::ScreenToClient(hWnd, &pos))
				io.MousePos = ImVec2((float)pos.x, (float)pos.y);
}
