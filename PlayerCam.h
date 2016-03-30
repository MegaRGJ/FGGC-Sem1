
#include <DirectXMath.h>

using namespace DirectX;

class PlayerCam
{
public:

	PlayerCam();
	PlayerCam(const PlayerCam&);
	~PlayerCam();

	void SetFrameTime(float);// Do I need this?

	void SetPos(float, float, float); // setting the cameras position 
	void SetRot(float, float, float); // setting the rotation 

	void GetPos(float&, float&, float&);
	void GetRot(float&, float&, float&);

	void SetFrameTime(float);

	void MoveForward(bool);
	void MoveBack(bool);
	void MoveUp(bool);
	void MoveDown(bool);

	void TurnLeft(bool);
	void TurnRight(bool);
	void TurnUp(bool);
	void TurnDown(bool);

private:

	float m_posX, m_posY, m_posZ;
	float m_rotX, m_rotY, m_rotZ;

	float m_frameTime;

	float m_forwardSpeed, m_backwardSpeed;
	float m_upSpeed, m_downSpeed;
	float m_leftSpeed, m_rightSpeed;
	float m_lookUpSpeed, m_lookDownSpeed;

};

PlayerCam::PlayerCam()
{
}

PlayerCam::~PlayerCam()
{
}