// Dear ImGui: standalone example application for SDL3 + Vulkan

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

// 在自己的引擎或应用中集成 imgui_impl_vulkan.cpp/.h 时请注意：
// - ImGui_ImplVulkan_XXX 是 Vulkan 后端的正式接口和数据结构。
//   应用需要通过这些接口初始化后端、提交绘制数据以及释放相关资源。
// - ImGui_ImplVulkanH_XXX 是供本示例和 Vulkan 后端内部使用的辅助接口。
//   它们主要用于简化交换链等资源的管理，不建议在实际引擎或应用中直接依赖。
// 更多信息请参阅 imgui_impl_vulkan.h 中的注释。

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"
#include "ui/test_component.h"
#include <VkBootstrap.h>
#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

// This example doesn't compile with Emscripten yet! Awaiting SDL3 support.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

// 启用volk
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
#define VOLK_IMPLEMENTATION
#include <volk.h>
#endif

//#define APP_USE_UNLIMITED_FRAME_RATE

// Vulkan 全局状态。示例程序只有一个窗口，因此集中保存以简化各阶段之间的传递。
static VkAllocationCallbacks*   g_Allocator = nullptr;
static vkb::Instance            g_VkbInstance;
static vkb::Device              g_VkbDevice;
static VkInstance               g_Instance = VK_NULL_HANDLE;
static VkPhysicalDevice         g_PhysicalDevice = VK_NULL_HANDLE;
static VkDevice                 g_Device = VK_NULL_HANDLE;
static uint32_t                 g_QueueFamily = (uint32_t)-1;
static VkQueue                  g_Queue = VK_NULL_HANDLE;
static VkPipelineCache          g_PipelineCache = VK_NULL_HANDLE;
static VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;

static ImGui_ImplVulkanH_Window g_MainWindowData;
static uint32_t                 g_MinImageCount = 2;
static bool                     g_SwapChainRebuild = false;

static void check_vk_result(VkResult err)
{
    if (err == VK_SUCCESS)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}

static bool SetupVulkanInstance(const ImVector<const char*>& instance_extensions)
{
    // Volk 负责在运行时加载 Vulkan 函数；未启用 Volk 时由链接器直接提供。
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
    const VkResult volk_result = volkInitialize();
    if (volk_result != VK_SUCCESS)
    {
        fprintf(stderr, "Failed to initialize volk: VkResult = %d\n", volk_result);
        return false;
    }
#endif

    vkb::InstanceBuilder builder;
    // SDL 会根据当前平台提供创建窗口表面所需的实例扩展。
    builder.set_app_name("Imgui_dev")
        .enable_extensions(static_cast<size_t>(instance_extensions.Size), instance_extensions.Data);
#ifdef _DEBUG
    // 调试构建中启用 Vulkan 校验层，尽早发现 API 使用错误。
    builder.request_validation_layers().use_default_debug_messenger();
#endif

    auto instance_result = builder.build();
    if (!instance_result)
    {
        fprintf(stderr, "Failed to create Vulkan instance: %s\n", instance_result.error().message().c_str());
        return false;
    }

    g_VkbInstance = instance_result.value();
    g_Instance = g_VkbInstance.instance;
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
    volkLoadInstance(g_Instance);
#endif
    return true;
}

static bool SetupVulkanDevice(VkSurfaceKHR surface)
{
    // 选择物理设备
    auto physical_device_result = vkb::PhysicalDeviceSelector{ g_VkbInstance }
        .set_surface(surface)
        .select();
    if (!physical_device_result)
    {
        fprintf(stderr, "Failed to select Vulkan physical device: %s\n", physical_device_result.error().message().c_str());
        return false;
    }

    // 创建设备
    auto device_result = vkb::DeviceBuilder{ physical_device_result.value() }.build();
    if (!device_result)
    {
        fprintf(stderr, "Failed to create Vulkan device: %s\n", device_result.error().message().c_str());
        return false;
    }

    g_VkbDevice = device_result.value();
    // 获取图形队列
    auto graphics_queue_result = g_VkbDevice.get_queue(vkb::QueueType::graphics);
    auto graphics_queue_index_result = g_VkbDevice.get_queue_index(vkb::QueueType::graphics);
    auto present_queue_index_result = g_VkbDevice.get_queue_index(vkb::QueueType::present);
    if (!graphics_queue_result || !graphics_queue_index_result || !present_queue_index_result)
    {
        fprintf(stderr, "Failed to find Vulkan graphics/present queues.\n");
        return false;
    }
    if (graphics_queue_index_result.value() != present_queue_index_result.value())
    {
        fprintf(stderr, "Separate Vulkan graphics and present queue families are not supported.\n");
        return false;
    }

    // 获取物理设备和设备
    g_PhysicalDevice = g_VkbDevice.physical_device.physical_device;
    g_Device = g_VkbDevice.device;
    g_Queue = graphics_queue_result.value();
    g_QueueFamily = graphics_queue_index_result.value();
#ifdef IMGUI_IMPL_VULKAN_USE_VOLK
    volkLoadDevice(g_Device);
#endif

    // ImGui Vulkan 后端通过描述符池管理字体和用户纹理资源。
    VkDescriptorPoolSize pool_sizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE },
        { VK_DESCRIPTOR_TYPE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE },
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 0;
    for (VkDescriptorPoolSize& pool_size : pool_sizes)
        pool_info.maxSets += pool_size.descriptorCount;
    pool_info.poolSizeCount = (uint32_t)IM_COUNTOF(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    const VkResult err = vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator, &g_DescriptorPool);
    check_vk_result(err);
    return true;
}

// All the ImGui_ImplVulkanH_XXX structures/functions are optional helpers used by the demo.
// Your real engine/app may not use them.
static void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height)
{
    // 检查物理设备是否支持表面交换
    VkBool32 res;
    vkGetPhysicalDeviceSurfaceSupportKHR(g_PhysicalDevice, g_QueueFamily, surface, &res);
    if (res != VK_TRUE)
    {
        fprintf(stderr, "Error no WSI support on physical device 0\n");
        exit(-1);
    }

    // 选择表面格式
    const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
    const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    wd->Surface = surface;
    wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(g_PhysicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_COUNTOF(requestSurfaceImageFormat), requestSurfaceColorSpace);

    // Select Present Mode
#ifdef APP_USE_UNLIMITED_FRAME_RATE
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };
#else
    VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_FIFO_KHR };
#endif
    wd->PresentMode = ImGui_ImplVulkanH_SelectPresentMode(g_PhysicalDevice, wd->Surface, &present_modes[0], IM_COUNTOF(present_modes));
    //printf("[vulkan] Selected PresentMode = %d\n", wd->PresentMode);

    // 创建交换链、渲染通道、帧缓冲区等。
    IM_ASSERT(g_MinImageCount >= 2);
    ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, wd, g_QueueFamily, g_Allocator, width, height, g_MinImageCount, 0);
}

static void CleanupVulkan()
{
    // 销毁描述符池
    vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);
    // 销毁设备和实例
    vkb::destroy_device(g_VkbDevice);
    vkb::destroy_instance(g_VkbInstance);
}

static void CleanupVulkanWindow(ImGui_ImplVulkanH_Window* wd)
{
    // 销毁窗口
    ImGui_ImplVulkanH_DestroyWindow(g_Instance, g_Device, wd, g_Allocator);
    // 销毁表面
    vkDestroySurfaceKHR(g_Instance, wd->Surface, g_Allocator);
}

static void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data)
{
    // 每帧先获取交换链图像；窗口尺寸变化会使交换链过期，交给主循环重建。
    VkSemaphore image_acquired_semaphore  = wd->FrameSemaphores[wd->SemaphoreIndex].ImageAcquiredSemaphore;
    VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
    VkResult err = vkAcquireNextImageKHR(g_Device, wd->Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &wd->FrameIndex);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
        g_SwapChainRebuild = true;
    if (err == VK_ERROR_OUT_OF_DATE_KHR)
        return;
    if (err != VK_SUBOPTIMAL_KHR)
        check_vk_result(err);

    ImGui_ImplVulkanH_Frame* fd = &wd->Frames[wd->FrameIndex];
    {
        // 等待 GPU 完成上一次对此帧资源的使用，再安全地复用命令缓冲区。
        err = vkWaitForFences(g_Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
        check_vk_result(err);

        err = vkResetFences(g_Device, 1, &fd->Fence);
        check_vk_result(err);
    }
    {
        // 重置命令池
        err = vkResetCommandPool(g_Device, fd->CommandPool, 0);
        check_vk_result(err);
        // 开始命令缓冲区
        VkCommandBufferBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
        check_vk_result(err);
    }
    {
        // 开始渲染通道
        VkRenderPassBeginInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        info.renderPass = wd->RenderPass;
        info.framebuffer = fd->Framebuffer;
        info.renderArea.extent.width = wd->Width;
        info.renderArea.extent.height = wd->Height;
        info.clearValueCount = 1;
        info.pClearValues = &wd->ClearValue;
        vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
    }

    // 记录 ImGui 绘制数据到命令缓冲区
    ImGui_ImplVulkan_RenderDrawData(draw_data, fd->CommandBuffer);

    // 结束渲染通道
    vkCmdEndRenderPass(fd->CommandBuffer);
    {
        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        info.waitSemaphoreCount = 1;
        info.pWaitSemaphores = &image_acquired_semaphore;
        info.pWaitDstStageMask = &wait_stage;
        info.commandBufferCount = 1;
        info.pCommandBuffers = &fd->CommandBuffer;
        info.signalSemaphoreCount = 1;
        info.pSignalSemaphores = &render_complete_semaphore;

        err = vkEndCommandBuffer(fd->CommandBuffer);
        check_vk_result(err);
        err = vkQueueSubmit(g_Queue, 1, &info, fd->Fence);
        check_vk_result(err);
    }
}

static void FramePresent(ImGui_ImplVulkanH_Window* wd)
{
    // 交换链等待重建时不能继续呈现旧图像。
    if (g_SwapChainRebuild)
        return;
    VkSemaphore render_complete_semaphore = wd->FrameSemaphores[wd->SemaphoreIndex].RenderCompleteSemaphore;
    VkPresentInfoKHR info = {};
    info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    info.waitSemaphoreCount = 1;
    info.pWaitSemaphores = &render_complete_semaphore;
    info.swapchainCount = 1;
    info.pSwapchains = &wd->Swapchain;
    info.pImageIndices = &wd->FrameIndex;
    VkResult err = vkQueuePresentKHR(g_Queue, &info);
    if (err == VK_ERROR_OUT_OF_DATE_KHR || err == VK_SUBOPTIMAL_KHR)
        g_SwapChainRebuild = true;
    if (err == VK_ERROR_OUT_OF_DATE_KHR)
        return;
    if (err != VK_SUBOPTIMAL_KHR)
        check_vk_result(err);
    wd->SemaphoreIndex = (wd->SemaphoreIndex + 1) % wd->SemaphoreCount; // Now we can use the next set of semaphores
}

// Main code
int main(int, char**)
{
    // Setup SDL
    // [If using SDL_MAIN_USE_CALLBACKS: all code below until the main loop starts would likely be your SDL_AppInit() function]
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        printf("Error: SDL_Init(): %s\n", SDL_GetError());
        return 1;
    }

    // Create window with Vulkan graphics context
    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    SDL_WindowFlags window_flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL3+Vulkan example", (int)(1280 * main_scale), (int)(800 * main_scale), window_flags);
    if (window == nullptr)
    {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return 1;
    }

    ImVector<const char*> extensions;
    {
        // Vulkan 实例必须启用 SDL 创建当前平台窗口表面所需的扩展。
        uint32_t sdl_extensions_count = 0;
        const char* const* sdl_extensions = SDL_Vulkan_GetInstanceExtensions(&sdl_extensions_count);
        for (uint32_t n = 0; n < sdl_extensions_count; n++)
            extensions.push_back(sdl_extensions[n]);
    }
    if (!SetupVulkanInstance(extensions))
        return 1;

    // Create Window Surface
    VkSurfaceKHR surface;
    VkResult err;
    if (SDL_Vulkan_CreateSurface(window, g_Instance, g_Allocator, &surface) == 0)
    {
        printf("Failed to create Vulkan surface.\n");
        return 1;
    }
    if (!SetupVulkanDevice(surface))
        return 1;

    // 创建帧缓冲区
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    ImGui_ImplVulkanH_Window* wd = &g_MainWindowData;
    SetupVulkanWindow(wd, surface, w, h);
    // Vulkan 初始化完成后再显示窗口，避免用户看到尚未绘制的空白内容。
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_ShowWindow(window);

    // 初始化 ImGui 上下文
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // 设置 ImGui 样式
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // 设置缩放
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

    // 初始化平台/渲染后端
    ImGui_ImplSDL3_InitForVulkan(window);
    ImGui_ImplVulkan_InitInfo init_info = {};
    //init_info.ApiVersion = VK_API_VERSION_1_3;              // Pass in your value of VkApplicationInfo::apiVersion, otherwise will default to header version.
    init_info.Instance = g_Instance;
    init_info.PhysicalDevice = g_PhysicalDevice;
    init_info.Device = g_Device;
    init_info.QueueFamily = g_QueueFamily;
    init_info.Queue = g_Queue;
    init_info.PipelineCache = g_PipelineCache;
    init_info.DescriptorPool = g_DescriptorPool;
    init_info.MinImageCount = g_MinImageCount;
    init_info.ImageCount = wd->ImageCount;
    init_info.Allocator = g_Allocator;
    init_info.PipelineInfoMain.RenderPass = wd->RenderPass;
    init_info.PipelineInfoMain.Subpass = 0;
    init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.CheckVkResultFn = check_vk_result;
    ImGui_ImplVulkan_Init(&init_info);

    // 加载字体
    // - 如果没有显式加载字体，Dear ImGui 会选择内置字体：AddFontDefaultVector() 或 AddFontDefaultBitmap()。
    //   具体选择取决于 style.FontSizeBase * style.FontScaleMain * style.FontScaleDpi 的值是否达到指定阈值。
    // - 可以加载多个字体，并通过 ImGui::PushFont()/PopFont() 切换当前使用的字体。
    // - 如果字体文件加载失败，AddFont 系列函数会返回 nullptr。请处理这种错误，例如断言、显示错误并退出。
    // - 更多字体使用说明请阅读 docs/FONTS.md。
    // - 在 imconfig 配置文件中定义 IMGUI_ENABLE_FREETYPE，可以使用 FreeType 获得更高质量的字体渲染。
    // - C/C++ 字符串中的反斜杠 \ 需要写成两个反斜杠 \\。
    //style.FontSizeBase = 20.0f;
    //io.Fonts->AddFontDefaultVector();
    //io.Fonts->AddFontDefaultBitmap();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf");
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf");
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf");
    //IM_ASSERT(font != nullptr);

    // 我们的状态
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    TestComponent test_component;

    // 主循环
    bool done = false;
    while (!done)
    {
        // 轮询和处理事件 (输入、窗口大小变化等)

        // 可以读取 io.WantCaptureMouse 和 io.WantCaptureKeyboard，判断 Dear ImGui 是否需要处理输入。
        // - 当 io.WantCaptureMouse 为 true 时，不要把鼠标输入传递给主应用，也不要清除或覆盖应用保存的鼠标数据。
        // - 当 io.WantCaptureKeyboard 为 true 时，不要把键盘输入传递给主应用，也不要清除或覆盖应用保存的键盘数据。
        // 通常可以始终将所有输入传递给 Dear ImGui，再根据这两个标志决定是否向主应用屏蔽输入。
        // [如果使用 SDL_MAIN_USE_CALLBACKS：请在 SDL_AppEvent() 中调用 ImGui_ImplSDL3_ProcessEvent()。]
        
        // 轮询事件
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // 处理事件
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
                done = true;
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // [如果使用 SDL_MAIN_USE_CALLBACKS：请在 SDL_AppIterate() 中处理所有代码。]
        if (SDL_GetWindowFlags(window) & SDL_WINDOW_MINIMIZED)
        {
            // 最小化时暂停渲染并短暂休眠，避免无意义地占用 CPU。
            SDL_Delay(10);
            continue;
        }

        // 交换链大小变化时重建交换链
        int fb_width, fb_height;
        SDL_GetWindowSize(window, &fb_width, &fb_height);
        if (fb_width > 0 && fb_height > 0 && (g_SwapChainRebuild || g_MainWindowData.Width != fb_width || g_MainWindowData.Height != fb_height))
        {
            ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
            ImGui_ImplVulkanH_CreateOrResizeWindow(g_Instance, g_PhysicalDevice, g_Device, wd, g_QueueFamily, g_Allocator, fb_width, fb_height, g_MinImageCount, 0);
            g_MainWindowData.FrameIndex = 0;
            g_SwapChainRebuild = false;
        }

        // 开始 ImGui 帧
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // 从这里开始构建本帧 UI；ImGui::Render() 后才会生成实际绘制数据。
        // 1. 显示大 demo 窗口 (大部分示例代码都在 ImGui::ShowDemoWindow() 中！你可以浏览其代码来了解更多关于 Dear ImGui 的信息。)
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 自定义组件负责维护并绘制自己的界面。
        test_component.Render();

        // 应用级别的控制 kept in main for now.
        {
            ImGui::Begin("App");
            ImGui::Checkbox("Demo Window", &show_demo_window);
            ImGui::Checkbox("Another Window", &show_another_window);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. 显示另一个简单窗口。
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // 渲染
        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();
        const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
        if (!is_minimized)
        {
            // ImGui 使用预乘 Alpha 的清屏颜色，RGB 分量需要先乘以透明度。
            wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
            wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
            wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
            wd->ClearValue.color.float32[3] = clear_color.w;
            FrameRender(wd, draw_data);
            FramePresent(wd);
        }
    }

    // 清理
    // [如果使用 SDL_MAIN_USE_CALLBACKS：请在 SDL_AppQuit() 中处理所有代码。]
    // 销毁 Vulkan/ImGui 资源前等待 GPU 空闲，防止资源仍被命令缓冲区引用。
    err = vkDeviceWaitIdle(g_Device);
    check_vk_result(err);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    CleanupVulkanWindow(&g_MainWindowData);
    CleanupVulkan();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
