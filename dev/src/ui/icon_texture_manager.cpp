#include "icon_texture_manager.h"

#include "imgui_impl_vulkan.h"

#include <stdio.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include "../third_party/stb_image.h"

namespace {

struct IconTexture {
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView image_view = VK_NULL_HANDLE;
    VkSampler sampler = VK_NULL_HANDLE;
    VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
    uint32_t mip_levels = 1;
};

struct IconEntry {
    std::string key;
    IconTexture texture;
};

IconTextureManagerInitInfo g_init_info{};
std::vector<IconEntry> g_entries;
std::unordered_map<std::string, size_t> g_key_to_index;

bool CheckVkResult(VkResult err, const char* op)
{
    if (err == VK_SUCCESS)
        return true;
    fprintf(stderr, "[vulkan] %s failed: %d\n", op, (int)err);
    return false;
}

uint32_t FindMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(g_init_info.physical_device, &mem_properties);
    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; ++i)
        if ((type_filter & (1u << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;
    return UINT32_MAX;
}

bool CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& out_buffer, VkDeviceMemory& out_memory)
{
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (!CheckVkResult(vkCreateBuffer(g_init_info.device, &buffer_info, g_init_info.allocator, &out_buffer), "vkCreateBuffer"))
        return false;

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(g_init_info.device, out_buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = FindMemoryType(mem_requirements.memoryTypeBits, properties);
    if (alloc_info.memoryTypeIndex == UINT32_MAX)
        return false;

    if (!CheckVkResult(vkAllocateMemory(g_init_info.device, &alloc_info, g_init_info.allocator, &out_memory), "vkAllocateMemory(buffer)"))
        return false;
    if (!CheckVkResult(vkBindBufferMemory(g_init_info.device, out_buffer, out_memory, 0), "vkBindBufferMemory"))
        return false;
    return true;
}

bool BeginSingleTimeCommands(VkCommandPool& out_command_pool, VkCommandBuffer& out_command_buffer)
{
    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = g_init_info.queue_family;
    pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    if (!CheckVkResult(vkCreateCommandPool(g_init_info.device, &pool_info, g_init_info.allocator, &out_command_pool), "vkCreateCommandPool"))
        return false;

    VkCommandBufferAllocateInfo alloc_info = {};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = out_command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;
    if (!CheckVkResult(vkAllocateCommandBuffers(g_init_info.device, &alloc_info, &out_command_buffer), "vkAllocateCommandBuffers"))
        return false;

    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if (!CheckVkResult(vkBeginCommandBuffer(out_command_buffer, &begin_info), "vkBeginCommandBuffer"))
        return false;
    return true;
}

bool EndSingleTimeCommands(VkCommandPool command_pool, VkCommandBuffer command_buffer)
{
    if (!CheckVkResult(vkEndCommandBuffer(command_buffer), "vkEndCommandBuffer"))
        return false;

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;
    if (!CheckVkResult(vkQueueSubmit(g_init_info.queue, 1, &submit_info, VK_NULL_HANDLE), "vkQueueSubmit"))
        return false;
    if (!CheckVkResult(vkQueueWaitIdle(g_init_info.queue), "vkQueueWaitIdle"))
        return false;

    vkFreeCommandBuffers(g_init_info.device, command_pool, 1, &command_buffer);
    vkDestroyCommandPool(g_init_info.device, command_pool, g_init_info.allocator);
    return true;
}

void TransitionImageLayout(VkCommandBuffer cmd, VkImage image, VkImageLayout old_layout, VkImageLayout new_layout, uint32_t base_mip_level, uint32_t level_count)
{
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = base_mip_level;
    barrier.subresourceRange.levelCount = level_count;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    } else {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }

    vkCmdPipelineBarrier(cmd, src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void GenerateMipmaps(VkCommandBuffer cmd, VkImage image, int32_t width, int32_t height, uint32_t mip_levels)
{
    int32_t mip_width = width;
    int32_t mip_height = height;

    for (uint32_t i = 1; i < mip_levels; ++i)
    {
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        vkCmdPipelineBarrier(cmd,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &barrier);

        VkImageBlit blit = {};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mip_width, mip_height, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;

        const int32_t next_width = std::max(1, mip_width / 2);
        const int32_t next_height = std::max(1, mip_height / 2);
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { next_width, next_height, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(cmd,
            image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit, VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        vkCmdPipelineBarrier(cmd,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0, 0, nullptr, 0, nullptr, 1, &barrier);

        mip_width = next_width;
        mip_height = next_height;
    }

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = mip_levels - 1;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(cmd,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void CopyBufferToImage(VkCommandBuffer cmd, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkBufferImageCopy region = {};
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageExtent = { width, height, 1 };
    vkCmdCopyBufferToImage(cmd, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
}

void DestroyTexture(IconTexture& texture)
{
    if (texture.descriptor_set != VK_NULL_HANDLE) {
        ImGui_ImplVulkan_RemoveTexture(texture.descriptor_set);
        texture.descriptor_set = VK_NULL_HANDLE;
    }
    if (texture.sampler != VK_NULL_HANDLE) {
        vkDestroySampler(g_init_info.device, texture.sampler, g_init_info.allocator);
        texture.sampler = VK_NULL_HANDLE;
    }
    if (texture.image_view != VK_NULL_HANDLE) {
        vkDestroyImageView(g_init_info.device, texture.image_view, g_init_info.allocator);
        texture.image_view = VK_NULL_HANDLE;
    }
    if (texture.image != VK_NULL_HANDLE) {
        vkDestroyImage(g_init_info.device, texture.image, g_init_info.allocator);
        texture.image = VK_NULL_HANDLE;
    }
    if (texture.memory != VK_NULL_HANDLE) {
        vkFreeMemory(g_init_info.device, texture.memory, g_init_info.allocator);
        texture.memory = VK_NULL_HANDLE;
    }
}

bool LoadTextureFromFile(const char* filename, IconTexture& out_texture)
{
    int image_width = 0;
    int image_height = 0;
    int image_channels = 0;
    stbi_uc* pixels = stbi_load(filename, &image_width, &image_height, &image_channels, STBI_rgb_alpha);
    if (pixels == nullptr)
        return false;

    const int pixel_count = image_width * image_height;
    for (int i = 0; i < pixel_count; ++i)
    {
        stbi_uc* p = &pixels[i * 4];
        p[0] = 255;
        p[1] = 255;
        p[2] = 255;
    }

    const VkDeviceSize upload_size = (VkDeviceSize)image_width * (VkDeviceSize)image_height * 4;

    VkBuffer staging_buffer = VK_NULL_HANDLE;
    VkDeviceMemory staging_buffer_memory = VK_NULL_HANDLE;
    if (!CreateBuffer(upload_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        staging_buffer, staging_buffer_memory)) {
        stbi_image_free(pixels);
        return false;
    }

    void* mapped = nullptr;
    if (!CheckVkResult(vkMapMemory(g_init_info.device, staging_buffer_memory, 0, upload_size, 0, &mapped), "vkMapMemory")) {
        stbi_image_free(pixels);
        return false;
    }
    std::memcpy(mapped, pixels, (size_t)upload_size);
    vkUnmapMemory(g_init_info.device, staging_buffer_memory);
    stbi_image_free(pixels);

    const uint32_t mip_levels = 1;

    VkImageCreateInfo image_info = {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    image_info.extent.width = (uint32_t)image_width;
    image_info.extent.height = (uint32_t)image_height;
    image_info.extent.depth = 1;
    image_info.mipLevels = mip_levels;
    image_info.arrayLayers = 1;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    if (!CheckVkResult(vkCreateImage(g_init_info.device, &image_info, g_init_info.allocator, &out_texture.image), "vkCreateImage"))
        return false;

    VkMemoryRequirements image_mem_requirements;
    vkGetImageMemoryRequirements(g_init_info.device, out_texture.image, &image_mem_requirements);
    VkMemoryAllocateInfo image_alloc_info = {};
    image_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    image_alloc_info.allocationSize = image_mem_requirements.size;
    image_alloc_info.memoryTypeIndex = FindMemoryType(image_mem_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    if (image_alloc_info.memoryTypeIndex == UINT32_MAX)
        return false;

    if (!CheckVkResult(vkAllocateMemory(g_init_info.device, &image_alloc_info, g_init_info.allocator, &out_texture.memory), "vkAllocateMemory(image)"))
        return false;
    if (!CheckVkResult(vkBindImageMemory(g_init_info.device, out_texture.image, out_texture.memory, 0), "vkBindImageMemory"))
        return false;

    VkCommandPool command_pool = VK_NULL_HANDLE;
    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    if (!BeginSingleTimeCommands(command_pool, command_buffer))
        return false;
    TransitionImageLayout(command_buffer, out_texture.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, mip_levels);
    CopyBufferToImage(command_buffer, staging_buffer, out_texture.image, (uint32_t)image_width, (uint32_t)image_height);
    TransitionImageLayout(command_buffer, out_texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 1);
    if (!EndSingleTimeCommands(command_pool, command_buffer))
        return false;

    vkDestroyBuffer(g_init_info.device, staging_buffer, g_init_info.allocator);
    vkFreeMemory(g_init_info.device, staging_buffer_memory, g_init_info.allocator);

    VkImageViewCreateInfo view_info = {};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = out_texture.image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = VK_FORMAT_R8G8B8A8_UNORM;
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    view_info.subresourceRange.levelCount = mip_levels;
    view_info.subresourceRange.layerCount = 1;
    if (!CheckVkResult(vkCreateImageView(g_init_info.device, &view_info, g_init_info.allocator, &out_texture.image_view), "vkCreateImageView"))
        return false;

    VkSamplerCreateInfo sampler_info = {};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_NEAREST;
    sampler_info.minFilter = VK_FILTER_LINEAR;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 0.0f;
    VkPhysicalDeviceFeatures device_features = {};
    vkGetPhysicalDeviceFeatures(g_init_info.physical_device, &device_features);
    sampler_info.anisotropyEnable = device_features.samplerAnisotropy ? VK_TRUE : VK_FALSE;
    sampler_info.maxAnisotropy = device_features.samplerAnisotropy ? 8.0f : 1.0f;
    if (!CheckVkResult(vkCreateSampler(g_init_info.device, &sampler_info, g_init_info.allocator, &out_texture.sampler), "vkCreateSampler"))
        return false;

    out_texture.mip_levels = mip_levels;
    out_texture.descriptor_set = ImGui_ImplVulkan_AddTexture(out_texture.sampler, out_texture.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    return out_texture.descriptor_set != VK_NULL_HANDLE;
}

std::string BuildFullIconPath(const char* relative_icon_path)
{
    std::string path = g_init_info.base_path ? std::string(g_init_info.base_path) : std::string();
    path += "../..";
    path += "/assets/icons/";
    path += relative_icon_path;
    return path;
}

void RemoveEntryByIndex(size_t index)
{
    if (index >= g_entries.size())
        return;

    DestroyTexture(g_entries[index].texture);
    const std::string key = g_entries[index].key;

    const size_t last = g_entries.size() - 1;
    if (index != last) {
        g_entries[index] = g_entries[last];
        g_key_to_index[g_entries[index].key] = index;
    }
    g_entries.pop_back();
    g_key_to_index.erase(key);
}

}

bool IconTextureManagerInitialize(const IconTextureManagerInitInfo& init_info)
{
    if (init_info.physical_device == VK_NULL_HANDLE ||
        init_info.device == VK_NULL_HANDLE ||
        init_info.queue == VK_NULL_HANDLE ||
        init_info.queue_family == (uint32_t)-1 ||
        init_info.base_path == nullptr)
        return false;

    g_init_info = init_info;
    return true;
}

void IconTextureManagerShutdown()
{
    for (IconEntry& entry : g_entries)
        DestroyTexture(entry.texture);
    g_entries.clear();
    g_key_to_index.clear();
    g_init_info = {};
}

ImTextureID IconTextureManagerLoadPng(const char* key, const char* relative_icon_path)
{
    if (key == nullptr || relative_icon_path == nullptr)
        return (ImTextureID)0;

    auto found = g_key_to_index.find(key);
    if (found != g_key_to_index.end())
        return (ImTextureID)g_entries[found->second].texture.descriptor_set;

    IconTexture texture;
    std::string full_path = BuildFullIconPath(relative_icon_path);
    if (!LoadTextureFromFile(full_path.c_str(), texture)) {
        fprintf(stderr, "Failed to load icon: %s\n", full_path.c_str());
        return (ImTextureID)0;
    }

    IconEntry entry;
    entry.key = key;
    entry.texture = texture;

    g_entries.push_back(entry);
    const size_t index = g_entries.size() - 1;
    g_key_to_index[entry.key] = index;
    return (ImTextureID)g_entries[index].texture.descriptor_set;
}

ImTextureID IconTextureManagerGet(const char* key)
{
    if (key == nullptr)
        return (ImTextureID)0;

    auto found = g_key_to_index.find(key);
    if (found == g_key_to_index.end())
        return (ImTextureID)0;
    return (ImTextureID)g_entries[found->second].texture.descriptor_set;
}

void IconTextureManagerUnload(const char* key)
{
    if (key == nullptr)
        return;

    auto found = g_key_to_index.find(key);
    if (found == g_key_to_index.end())
        return;
    RemoveEntryByIndex(found->second);
}
