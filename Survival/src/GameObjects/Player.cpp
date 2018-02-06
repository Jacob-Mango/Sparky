#include "sp/sp.h"
#include "Player.h"

#include "sp/app/Input.h"


using namespace sp;
using namespace graphics;
using namespace maths;

using namespace events;
using namespace entity;
using namespace component;

using namespace API;

using namespace world;

#define TORADS (PI / 180.0f)

Player::Player(World* world, Shader* shader)
	: m_World(world), m_MouseSensitivity(2.0f), m_MouseWasGrabbed(false)
{
	String name = "Player";

	PBRMaterial* playerMaterial = spnew PBRMaterial(name, shader);
	PBRMaterialInstance* m = spnew PBRMaterialInstance(playerMaterial);

	Model* playerModel = spnew Model("/models/" + name + "/" + name + ".spm", m);

	AddComponent(spnew MeshComponent(playerModel->GetMesh()));
	AddComponent(spnew AnimationComponent());
	Camera* camera = spnew Camera(mat4::Identity());
	world->SetCamera(camera);

	maths::mat4 pos = maths::mat4::Identity();
	pos.SetPosition(maths::vec3(0, 0, 0));
	TransformComponent* pc = spnew TransformComponent(pos);
	AddComponent(pc);
	AddComponent(spnew CameraComponent(camera));



	m_ShouldUpdate = true;
}

Player::~Player()
{
}

void Player::OnInit()
{

}

void Player::OnUpdate(const sp::Timestep& ts)
{
	GetComponent<AnimationComponent>()->SetJoint("Torso", mat4::Rotate(-90.0f, vec3(1, 0, 0)));

	CheckInputs(ts);
}

void Player::CheckInputs(const sp::Timestep& ts)
{
	if (Input::IsMouseButtonPressed(SP_MOUSE_RIGHT))
	{
		if (!Input::GetInputManager()->IsMouseGrabbed())
		{
			Input::GetInputManager()->SetMouseGrabbed(true);
			Input::GetInputManager()->SetMouseCursor(SP_NO_CURSOR);
		}
	}

	if (Input::GetInputManager()->IsMouseGrabbed())
	{
		vec2 windowSize = Application::GetApplication().GetWindowSize();
		vec2 windowCenter = vec2((int32)(windowSize.x / 2.0f), (int32)(windowSize.y / 2.0f));

		vec2 mouse = Input::GetInputManager()->GetMousePosition();
		mouse.x -= windowCenter.x;
		mouse.y -= windowCenter.y;

		if (m_MouseWasGrabbed)
		{
			//GetComponent<CameraComponent>()->camera->Yaw -= mouse.x * m_MouseSensitivity * ts.GetSeconds();
			//GetComponent<CameraComponent>()->camera->Pitch += mouse.y * m_MouseSensitivity * ts.GetSeconds();
		}

		//if (GetComponent<CameraComponent>()->camera->Pitch < -90) GetComponent<CameraComponent>()->camera->Pitch = -90;
		//else if (GetComponent<CameraComponent>()->camera->Pitch > +90) GetComponent<CameraComponent>()->camera->Pitch = +90;

		m_MouseWasGrabbed = true;
		Input::GetInputManager()->SetMousePosition(windowCenter);
	}


	//float Yaw = GetComponent<CameraComponent>()->camera->Yaw;

	//maths::Quaternion orientation = maths::Quaternion::RotationY(-GetComponent<CameraComponent>()->camera->Yaw);
	vec3 forward = vec3(); // maths::Quaternion::Rotate(orientation, vec3::Forward());

	//forward.x = -sin(toRadians(-Yaw));
	forward.y = 0; // +cos(toRadians(Pitch + 180));
	//forward.z = +cos(toRadians(-Yaw));

	vec3 right = vec3(); //maths::Quaternion::Rotate(orientation, vec3::Right());

	//right.x = -sin(toRadians(-Yaw + 180));
	right.y = 0; // +cos(toRadians(Pitch + 180));
	//right.z = +cos(toRadians(-Yaw + 180));

	vec3 up = vec3::Up();

	if (Input::GetInputManager()->IsMouseGrabbed())
	{
		m_SprintSpeed = 20.0f;
		m_Speed = 4.0f;
		float speed = (Input::IsKeyPressed(SP_KEY_SHIFT) ? m_SprintSpeed : m_Speed);

		vec3 moveDirection;

		if (Input::IsKeyPressed(SP_KEY_W))
			moveDirection.Add(forward);
		else if (Input::IsKeyPressed(SP_KEY_S))
			moveDirection.Subtract(forward);

		if (Input::IsKeyPressed(SP_KEY_A))
			moveDirection.Subtract(right);
		else if (Input::IsKeyPressed(SP_KEY_D))
			moveDirection.Add(right);

		if (Input::IsKeyPressed(SP_KEY_SPACE))
			moveDirection.Add(up);
		else if (Input::IsKeyPressed(SP_KEY_CONTROL))
			moveDirection.Subtract(up);

		moveDirection.Multiply(speed);
	}


	if (Input::IsKeyPressed(SP_KEY_ESCAPE))
	{
		Input::GetInputManager()->SetMouseGrabbed(false);
		Input::GetInputManager()->SetMouseCursor(1);
		m_MouseWasGrabbed = false;
	}
}