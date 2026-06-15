#include "test_component.h"

#include "imgui.h"

void TestComponent::Render()
{
    if (!open)
        return;

    if (!ImGui::Begin("Test Component", &open))
    {
        ImGui::End();
        return;
    }

    static float slider_value = 0.5f;
    static int click_count = 0;
    static char text_buffer[64] = "hello imgui";

    ImGui::Text("Dev test panel");
    ImGui::Separator();

    ImGui::SliderFloat("Slider", &slider_value, 0.0f, 1.0f);
    ImGui::InputText("Input", text_buffer, IM_ARRAYSIZE(text_buffer));

    if (ImGui::Button("Click me"))
        click_count++;

    ImGui::SameLine();
    ImGui::Text("clicks = %d", click_count);
    ImGui::Text("slider = %.2f", slider_value);

    ImGui::End();
}
