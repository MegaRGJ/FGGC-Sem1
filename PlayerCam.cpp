#include "PlayerCam.h"

PlayerCam::PlayerCam()
{
	m_posX = 0.0f;
	m_posY = 0.0f;
	m_posZ = 0.0f;

	m_rotX = 0.0f;
	m_rotY = 0.0f;
	m_rotZ = 0.0f;

	m_frameTime = 0.0f;

	m_forwardSpeed = 0.0f;
	m_backwardSpeed = 0.0f;

	m_leftSpeed = 0.0f;
	m_rightSpeed = 0.0f;

	m_upSpeed = 0.0f;
	m_downSpeed = 0.0f;

	m_lookUpSpeed = 0.0f;
	m_lookDownSpeed = 0.0f;

}
PlayerCam::PlayerCam(const PlayerCam& other) // check
{
}
PlayerCam::~PlayerCam()
{
}
void PlayerCam::SetPos(float x, float y, float z)
{
	m_posX = x;
	m_posY = y;
	m_posZ = z;
	return;
}
void PlayerCam::SetRot(float x, float y, float z)
{
	m_rotX = x;
	m_rotY = y;
	m_rotZ = z;
	return;
}
void PlayerCam::SetFrameTime(float t) // check
{
	m_frameTime = t;
	return;
}
void PlayerCam::MoveForward(bool KD) // KD stands for KeyDown
{
	float radian;

	if (KD)
	{
		m_forwardSpeed += m_frameTime * 0.001f;

		if (m_forwardSpeed > (m_frameTime * 0.03f))
		{
			m_forwardSpeed = m_frameTime * 0.03f;
		}
	}
	else
	{
		m_forwardSpeed -= m_frameTime * 0.0007f;
		if (m_forwardSpeed < 0.0f)
		{
			m_forwardSpeed = 0.0f;
		}
	}
	radian = m_rotY * 0.0174532925f; //conerts degrees to radians

	m_posX += sinf(radian) * m_forwardSpeed;
	m_posZ += cosf(radian) * m_forwardSpeed;

	return;
}
void PlayerCam::MoveBack(bool KD)
{
	float radian;

	if (KD)
	{
		m_backwardSpeed += m_frameTime * 0.001f;

		if (m_backwardSpeed > (m_frameTime * 0.03f))
		{
			m_backwardSpeed = m_frameTime * 0.03f;
		}
	}
	else
	{
		m_backwardSpeed -= m_frameTime * 0.0007f;

		if (m_backwardSpeed < 0.0f)
		{
			m_backwardSpeed = 0.0f;
		}
	}

	// Convert degrees to radians.
	radian = m_rotY * 0.0174532925f;

	// Update the position.
	m_posX -= sinf(radian) * m_backwardSpeed;
	m_posZ -= cosf(radian) * m_backwardSpeed;

	return;
}
void PlayerCam::MoveUp(bool KD)
{

	if (KD)
	{
		m_upSpeed += m_frameTime * 0.003f;

		if (m_upSpeed > (m_frameTime * 0.03f))
		{
			m_upSpeed = m_frameTime * 0.03f;
		}
	}
	else
	{
		m_upSpeed -= m_frameTime * 0.002f;

		if (m_upSpeed < 0.0f)
		{
			m_upSpeed = 0.0f;
		}
	}

	// Update the height position.
	m_posY += m_upSpeed;

	return;
}

