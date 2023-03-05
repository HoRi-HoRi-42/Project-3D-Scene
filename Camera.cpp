#include "Camera.hpp"
#include <glm/gtx/euler_angles.hpp>

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;

        //TODO - Update the rest of camera parameters
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = -glm::normalize(glm::cross(cameraUp, cameraFrontDirection));
        this->cameraOrigUp = cameraUp;
        this->cameraUpDirection = glm::cross(cameraRightDirection, cameraFrontDirection);
        this->cameraOrigFront = cameraFrontDirection;
    }

    void Camera::setCameraTarget(const glm::vec3 &cameraTarget) {
        Camera::cameraTarget = cameraTarget;
    }

    void Camera::setCameraPosition(const glm::vec3 &cameraPosition) {
        Camera::cameraPosition = cameraPosition;
    }

    const glm::vec3 &Camera::getCameraUpDirection() const {
        return cameraUpDirection;
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        if (direction == MOVE_FORWARD) {
            cameraPosition = cameraPosition + speed * cameraFrontDirection;
        }

        if (direction == MOVE_BACKWARD) {
            cameraPosition = cameraPosition + speed * -cameraFrontDirection;
        }

        if (direction == MOVE_LEFT) {
            cameraPosition = cameraPosition + speed * -cameraRightDirection;
        }

        if (direction == MOVE_RIGHT) {
            cameraPosition = cameraPosition + speed * cameraRightDirection;
        }
        /*cameraTarget = normalize((cameraPosition + cameraOrigFront));*/
        cameraTarget = cameraPosition + cameraFrontDirection;
    }

    const glm::vec3 &Camera::getCameraFrontDirection() const {
        return cameraFrontDirection;
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        glm::mat4 eulerAngle = glm::yawPitchRoll(glm::radians(yaw), glm::radians(pitch), 0.0f);

        ////if (glm::radians(pitch) < 1.5708f && glm::radians(pitch) > -1.5708f) {
        cameraFrontDirection = glm::vec3(glm::normalize((eulerAngle * glm::vec4(cameraOrigFront, 0.0f))));
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraOrigUp));
        cameraUpDirection = glm::cross(cameraRightDirection, cameraFrontDirection);
        /*cameraTarget = cameraPosition + cameraFrontDirection;*/
        cameraTarget = cameraPosition + cameraFrontDirection;
        ////}

        //glm::vec3 direction;
        //direction.x = /*sin(glm::radians(yaw)) * cos(glm::radians(pitch))*/ sin(glm::radians(pitch));
        //direction.y = /*sin(glm::radians(pitch))*/ sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        //direction.z = cos(glm::radians(yaw)) * cos(glm::radians(pitch));

        //glm::vec3 front = glm::normalize(direction);

        //cameraFrontDirection = front;
        //cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, cameraUpDirection));
        //cameraTarget = cameraPosition + cameraFrontDirection;
    }

    const glm::vec3 &Camera::getCameraPosition() const {
        return cameraPosition;
    }

    const glm::vec3 &Camera::getCameraTarget() const {
        return cameraTarget;
    }

    void Camera::fire(){
        cameraTarget+=glm::vec3(0.0f,0.02f,0.0f);
    }

    void Camera::updateDirectionVectors() {
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
    }

    void Camera::setPosition(glm::vec3 position) {
        this->cameraPosition = position;
        updateDirectionVectors();
    }

    void Camera::rotateAroundY(float angle) {
        // rotate with specific angle around Y axis
        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        this->cameraPosition = rotation * glm::vec4(this->cameraPosition, 1.0f);
        updateDirectionVectors();
    }


    void Camera::scenePreview(float angle) {
        setPosition(glm::vec3(21.1686, 1.95133, 1.62146));
        rotateAroundY(angle);
    }
}