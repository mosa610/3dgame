#pragma once

#include <sstream>
#include <d3d11.h>
#include <wrl.h>
#include "Scene.h"
#include "Graphics/Graphics.h"
#include "Graphics/sprite_bach.h"
#include "Graphics/model.h"
#include "Graphics/shader.h"
#include "Graphics/skinned_mesh.h"
#include "Graphics/Font.h"

class SceneTest : public Scene
{
public:
    SceneTest() {}
    ~SceneTest() override {}

	// ‰Šú‰»
	void Initialize() override;

	// I—¹‰»
	void Finalize() override;

	// XVˆ—
	void Update(float elapsedTime) override;

	// •`‰æˆ—
	void Render(float elapsedTime) override;
private:
	std::unique_ptr<sprite_batch> sprite_batches[8];
	std::unique_ptr<skinned_mesh> skinned_meshes;
	std::unique_ptr<Font> font;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];

};