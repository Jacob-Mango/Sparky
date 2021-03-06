#pragma once

//
// Sparky Engine header file
//

#define _NO_NUMERIC_LIMITS

#include <sp/app/Application.h>
#include <sp/app/Window.h>

#include "sp/maths/maths.h"

#include <sp/audio/Sound.h>
#include <sp/audio/SoundManager.h>

#include <sp/debug/DebugLayer.h>
#include <sp/debug/DebugMenu.h>
#include <sp/debug/DebugMenuAction.h>
#include <sp/debug/DebugMenuItem.h>

#include <sp/scene/Node.h>
#include <sp/scene/Object.h>
#include <sp/scene/Scene.h>
#include <sp/scene/Scene2D.h>

#include <sp/scene/component/Component.h>
#include <sp/scene/component/Components.h>

#include <sp/events/Event.h>
#include <sp/events/KeyEvent.h>
#include <sp/events/MouseEvent.h>

#include <sp/graphics/Renderer2D.h>
#include <sp/graphics/Renderer3D.h>
#include <sp/graphics/ForwardRenderer.h>
#include <sp/graphics/DeferredRenderer.h>

#include <sp/graphics/Font.h>
#include <sp/graphics/FontManager.h>
#include <sp/graphics/IRenderable.h>
#include <sp/graphics/Mask.h>
#include <sp/graphics/Mesh.h>
#include <sp/graphics/MeshFactory.h>
#include <sp/graphics/Model.h>
#include <sp/graphics/Renderable2D.h>
#include <sp/graphics/RenderCommand.h>
#include <sp/graphics/Sprite.h>
#include <sp/graphics/TextureManager.h>

#include <sp/graphics/material/Material.h>
#include <sp/graphics/material/MaterialManager.h>
#include <sp/graphics/material/PBRMaterial.h>

#include <sp/graphics/camera/Camera.h>
#include <sp/graphics/camera/FPSCamera.h>

#include <sp/graphics/layers/Group.h>
#include <sp/graphics/layers/Layer.h>
#include <sp/graphics/layers/Layer2D.h>
#include <sp/graphics/layers/Layer3D.h>

#include <sp/graphics/postfx/PostEffects.h>
#include <sp/graphics/postfx/PostEffectsPass.h>

#include <sp/graphics/postfx/effects/HDRPostEffect.h>
#include <sp/graphics/postfx/effects/FXAAPostEffect.h>
#include <sp/graphics/postfx/effects/SSAOPostEffect.h>

#include <sp/graphics/shaders/Shader.h>
#include <sp/graphics/shaders/ShaderFactory.h>
#include <sp/graphics/shaders/ShaderManager.h>
#include <sp/graphics/shaders/ShaderUniform.h>

#include <sp/graphics/API/Renderer.h>
#include <sp/graphics/API/Texture.h>
#include <sp/graphics/API/Texture2D.h>
#include <sp/graphics/API/TextureCube.h>
#include <sp/graphics/API/TextureDepth.h>
#include <sp/graphics/API/Framebuffer.h>
#include <sp/graphics/API/Framebuffer2D.h>
#include <sp/graphics/API/FramebufferDepth.h>

#include <sp/graphics/ui/Button.h>
#include <sp/graphics/ui/Panel.h>
#include <sp/graphics/ui/Slider.h>
#include <sp/graphics/ui/Widget.h>
#include <sp/graphics/ui/Label.h>

#include <sp/utils/ImageLoad.h>
#include <sp/utils/Log.h>
#include <sp/utils/Timer.h>
#include <sp/utils/Timestep.h>

#include <sp/system/FileSystem.h>
#include <sp/system/Memory.h>
#include <sp/system/MemoryManager.h>
#include <sp/system/VFS.h>

#include <sp/String.h>
#include <sp/Types.h>