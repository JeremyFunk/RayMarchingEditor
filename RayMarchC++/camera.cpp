#include "free_cam.h"
#include "window_data.h"
#include "helpers_glm.h"

enum CameraMode {
    Fixed, Free
};

struct CameraData {
    Camera camera;
    
    // True if the user entered the virtual render camera.
    bool enteredCam;

    // True if the user is repositioning the camera
    bool repositionCam;

    // True if the user moved on the last frame;
    bool moved;

    CameraMode mode;

    CameraData() {
        camera = Camera(glm::vec3(0.0, 0.0, 3.0));
        enteredCam = false;
        mode = Free;
    }

    void UpdateMouse(WindowData& w, float deltaTime, glm::vec3 camPos, glm::vec2 camPY) {
        moved = false;
        bool cMoved = false;

        if (glfwGetKey(w.window, GLFW_KEY_TAB) == GLFW_RELEASE) {
            if (mode == Fixed) {
                mode = Free;
            }
            else {
                mode = Fixed;
            }
        }
        if (glfwGetKey(w.window, GLFW_KEY_APOSTROPHE) == GLFW_RELEASE) {
            enteredCam = true;
            cMoved = true;
            update_camera(camPos, camPY);
        }

        if (mode == Free) {
            if (glfwGetKey(w.window, GLFW_KEY_W) == GLFW_PRESS) {
                camera.ProcessKeyboard(FORWARD, deltaTime);
                cMoved = true;
            }
            if (glfwGetKey(w.window, GLFW_KEY_S) == GLFW_PRESS) {
                camera.ProcessKeyboard(BACKWARD, deltaTime);
                cMoved = true;
            }
            if (glfwGetKey(w.window, GLFW_KEY_A) == GLFW_PRESS) {
                camera.ProcessKeyboard(LEFT, deltaTime);
                cMoved = true;
            }
            if (glfwGetKey(w.window, GLFW_KEY_D) == GLFW_PRESS) {
                camera.ProcessKeyboard(RIGHT, deltaTime);
                cMoved = true;
            }
            if (glfwGetKey(w.window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                camera.ProcessKeyboard(UP, deltaTime);
                cMoved = true;
            }
            if (glfwGetKey(w.window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                camera.ProcessKeyboard(DOWN, deltaTime);
                cMoved = true;
            }
            camera.ProcessMouseScroll(static_cast<float>(w.mouseData.scrollSpeed));
            if (w.mouseData.scrollSpeed != 0) {
                cMoved = true;
            }
        }

        camera.ProcessMouseMovement(w.mouseData.xOffset, w.mouseData.yOffset);
        if (w.mouseData.xOffset != 0 || w.mouseData.yOffset != 0 || cMoved) {
            move();
        }
    }

    // Used to generate rays in the shader.
    glm::vec3 ConvertToDirectionVector() {
        float pitchRadians = to_radians(camera.Pitch);
        float yawRadians = to_radians(camera.Yaw);

        float sinPitch = sin(pitchRadians);
        float cosPitch = cos(pitchRadians);
        float sinYaw = sin(yawRadians);
        float cosYaw = cos(yawRadians);
        return glm::normalize(glm::vec3(cosPitch * sinYaw, -sinPitch, cosPitch * cosYaw));
    }

private:
    void move() {
        if (!repositionCam) {
            enteredCam = false;
        }
        moved = true;
    }
    // Used when the camera jumps into the virtual render camera.
    bool update_camera(glm::vec3 camPos, glm::vec2 camPY) {
        camera.Position = glm::vec3(camPos);

        auto front = glm::vec3(camera.Front);
        auto up = glm::vec3(camera.Up);
        auto right = glm::vec3(camera.Right);

        camera.Yaw = camPY.y;
        camera.Pitch = camPY.x;
        camera.updateCameraVectors();

        return camera.Front != front || camera.Up != up || camera.Right != right;
    }
};