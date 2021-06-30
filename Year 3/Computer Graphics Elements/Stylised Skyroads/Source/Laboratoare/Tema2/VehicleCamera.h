#pragma once
#include <include/glm.h>
#include <include/math.h>
#include "Tema2_Player.h"

namespace Vehicle
{
	class Camera
	{
		public:
			Camera()
			{
				position = glm::vec3(0, 2, 5);
				forward = glm::vec3(0, 0, -1);
				up		= glm::vec3(0, 1, 0);
				right	= glm::vec3(1, 0, 0);
			}

			Camera(const glm::vec3 &position, const glm::vec3 &forward, const glm::vec3 &up)
			{
				Set(position, forward, up);
			}

			~Camera()
			{
			}

			void Set(const glm::vec3 &position, const glm::vec3 & forward, const glm::vec3 &up)
			{
				// set gets forward from a parameter for easier rotation setting
				this->position = position;
				this->forward = glm::normalize(forward);
				right = glm::cross(forward, up);
				this->up = glm::cross(right, forward);
			}

			void MoveForward(float distance)
			{
				glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
				position += dir * distance;
			}

			void TranslateForward(float distance)
			{
				position += glm::normalize(forward) * distance;
			}

			void TranslateUpword(float distance)
			{
				position += glm::normalize(glm::vec3(0, 1, 0)) * distance;
			}

			void TranslateRight(float distance)
			{
				position += glm::normalize(right) * distance;
			}

			void RotateFirstPerson_OX(float angle)
			{
				glm::vec4 newForward = glm::rotate(glm::mat4(1.0f), angle, right) * glm::vec4(forward, 1);
				forward = glm::normalize(glm::vec3(newForward));
				up = glm::normalize(glm::cross(right, forward));
			}

			void RotateFirstPerson_OY(float angle)
			{
				glm::vec4 newForward = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 1);
				forward = glm::normalize(glm::vec3(newForward));
				glm::vec4 newRight = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1);
				right = glm::normalize(glm::vec3(newRight));
				up = glm::normalize(glm::cross(right, forward));
			}

			void RotateFirstPerson_OZ(float angle)
			{
				glm::vec4 newRight = glm::rotate(glm::mat4(1.0f), angle, forward) * glm::vec4(right, 1);
				right = glm::normalize(glm::vec3(newRight));
				up = glm::normalize(glm::cross(right, forward));
			}

			glm::mat4 GetViewMatrix()
			{
				return glm::lookAt(position, position + forward, up);
			}
			
			/*
				Moves the camera to the vehicle depending on the camera person setting
			*/
			void MoveToVehicle()
			{
				Tema2_Player::VehicleObject *vehicle = Tema2_Player::vehicle;
				// hides the vehicle in first person
				vehicle->hidden = (cameraPerson == 1);
				switch (cameraPerson)
				{
				case 1: Set(glm::vec3(vehicle->x, vehicle->y, vehicle->z - 0.25), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0)); break;
				case 2: Set(glm::vec3(vehicle->x, vehicle->y + 1, vehicle->z - 6), glm::vec3(0, -0.25, 1), glm::vec3(0, 1, 0)); break;
				case 3: Set(glm::vec3(vehicle->x, vehicle->y + 1, vehicle->z + 2), glm::vec3(0, -0.5, -1), glm::vec3(0, 1, 0)); break;
				default: break;
				}
			}

		public:
			int cameraPerson = 3;
			glm::vec3 position;
			glm::vec3 forward;
			glm::vec3 right;
			glm::vec3 up;
		};
}