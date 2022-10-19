#include "vtpch.h"
#include "Camera.h"

namespace Volt
{
	Camera::Camera(float fov, float aspect, float nearPlane, float farPlane)
		: myFieldOfView(fov), myAspecRatio(aspect), myNearPlane(nearPlane), myFarPlane(farPlane)
	{
		myProjectionMatrix = gem::perspective(gem::radians(myFieldOfView), aspect, myNearPlane, myFarPlane);
		myViewMatrix = gem::mat4(1.f);
	}

	Camera::Camera(float left, float right, float bottom, float top, float nearPlane, float farPlane)
	{
		myProjectionMatrix = gem::ortho(left, right, bottom, top, -1.f, 1.f);
		myViewMatrix = gem::mat4(1.f);
	}

	void Camera::SetPerspectiveProjection(float fov, float aspect, float nearPlane, float farPlane)
	{
		myFieldOfView = fov;
		myAspecRatio = aspect;
		myNearPlane = nearPlane;
		myFarPlane = farPlane;

		myProjectionMatrix = gem::perspective(gem::radians(myFieldOfView), myAspecRatio, myNearPlane, myFarPlane);
	}

	gem::vec3 Camera::ScreenToWorldCoords(const gem::vec2& someCoords, const gem::vec2& aSize)
	{
		float x = (someCoords.x / aSize.x) * 2.f - 1.f;
		float y = (someCoords.y / aSize.y) * 2.f - 1.f;

		gem::mat4 matInv = gem::inverse(myProjectionMatrix * myViewMatrix);

		gem::vec4 rayOrigin = matInv * gem::vec4(x, -y, 0, 1);
		gem::vec4 rayEnd = matInv * gem::vec4(x, -y, 1, 1);

		if (rayOrigin.w == 0 || rayEnd.w == 0)
		{
			return { 0,0,0 };
		}

		rayOrigin /= rayOrigin.w;
		rayEnd /= rayEnd.w;

		gem::vec3 rayDir = gem::normalize(rayEnd - rayOrigin);

		return rayDir;
	}

	void Camera::SetOrthographicProjection(float left, float right, float bottom, float top)
	{
		myProjectionMatrix = gem::ortho(left, right, bottom, top, -1.f, 1.f);
	}

	gem::vec3 Camera::GetUp() const
	{
		return gem::rotate(GetOrientation(), gem::vec3{ 0.f, 1.f, 0.f });
	}

	gem::vec3 Camera::GetRight() const
	{
		return gem::rotate(GetOrientation(), gem::vec3{ 1.f, 0.f, 0.f });
	}

	gem::vec3 Camera::GetForward() const
	{
		return gem::rotate(GetOrientation(), gem::vec3{ 0.f, 0.f, 1.f });
	}

	gem::quat Camera::GetOrientation() const
	{
		return gem::quat(myRotation);
	}

	void Camera::RecalculateViewMatrix()
	{
		const float yawSign = GetUp().y < 0 ? -1.0f : 1.0f;

		const gem::vec3 lookAt = myPosition + GetForward();
		myViewMatrix = gem::lookAt(myPosition, lookAt, gem::vec3(0.f, yawSign, 0.f));
	}
}