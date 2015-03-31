#include "pch.h"
#include "Sample3DSceneRenderer.h"
#include "Common\DirectXHelper.h"
#include "KinectUtility.h"
#include "Utility.h"

using namespace Avateer;

using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_indexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	_grid = make_unique<DXGrid>();
	_mainAxes = make_unique<Axis>(2000000.0);
	_axis = make_unique<Axis>(2.0);

	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

	_sensor = KinectSensor::GetDefault();
	_reader = _sensor->BodyFrameSource->OpenReader();
	_bodies = ref new Vector<Body^>(_sensor->BodyFrameSource->BodyCount);
	_sensor->Open();

	_boneHierarchy = KinectUtility::CreateBoneHierarchy();
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		100.0f
		);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);

	static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	auto camMatrix = XMMatrixRotationRollPitchYaw(_pitch, _yaw, _roll);

	static const XMVECTORF32 alongZ = { 0.0f, 5.0f, 30.0f };

	auto eye = XMVector3TransformCoord(alongZ, camMatrix);
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	//CreateDeviceDependentResources();

	if (!m_tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		//Rotate(0.0f);
	}
}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

static float lastPosX;
static float lastPosY;

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX, float positionY)
{
	if (m_tracking)
	{
		_yaw = (positionX - lastPosX) / 100;
		_pitch = (positionY - lastPosY) / 100;
		CreateWindowSizeDependentResources();
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	lastPosX = 0;
	lastPosY = 0;
	m_tracking = false;
}

#define _USE_MATH_DEFINES 1

#include <math.h>

static void ExtractRotationInDegrees(Vector4 rotQuaternion, int& pitch, int& yaw, int& roll)
{
	double x = rotQuaternion.X;
	double y = rotQuaternion.Y;
	double z = rotQuaternion.Z;
	double w = rotQuaternion.W;

	// convert face rotation quaternion to Euler angles in degrees
	double yawD, pitchD, rollD;
	pitchD = atan2(2 * ((y * z) + (w * x)), (w * w) - (x * x) - (y * y) + (z * z)) / M_PI * 180.0;
	yawD = asin(2 * ((w * y) - (x * z))) / M_PI * 180.0;
	rollD = atan2(2 * ((x * y) + (w * z)), (w * w) + (x * x) - (y * y) - (z * z)) / M_PI * 180.0;
}

static float x = 0.0f;

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
		return;

	auto context = m_deviceResources->GetD3DDeviceContext();

	DrawGrid(context);
	DrawAxis(context, _mainAxes.get());

	//auto quat = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(0.0f), XMConvertToRadians(0.0f), XMConvertToRadians(x += 2.0f));
	//auto rotMatrix = XMMatrixRotationQuaternion(quat);
	//auto tmat = XMMatrixTranslation(0.0f, -2.0f, 0.0f);
	//auto wmat = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	//XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(tmat * rotMatrix * wmat));
	//DrawBone(context);
	//return;

	bool updated = false;

	{
		auto bodyFrame = _reader->AcquireLatestFrame();
		if (bodyFrame != nullptr)
		{
			bodyFrame->GetAndRefreshBodyData(_bodies);
			updated = true;
		}
	}

	context->RSSetState(m_pRasterState);

	// use bodies array if it is new...
	for (auto body : _bodies)
	{
		if ((Body^)body == nullptr || !body->IsTracked)
			continue;

		const float boneLength = 4.0f;
		XMVECTOR transformed = { 0.0f, 0.0f, 0.0f, 0.0f };
		XMVECTOR res = { 0.0f, 0.0f, 0.0f, 0.0f };
		bool terminated = false;

		KinectUtility::TraverseBoneHierarchy(_boneHierarchy, 
			[&body, &res, this, &transformed, boneLength, &context](shared_ptr<RigJoint>& t)
			{
				Utility::Out(L"Joint Type is %s\n", t->JointType().ToString()->Data());
				//if (t->JointType() == JointType::FootRight || t->JointType() == JointType::FootLeft || 
				//	t->JointType() == JointType::Head || t->JointType() == JointType::HandTipRight)
				//	transformed = res;
				if (t->JointType() == JointType::HandTipLeft)
				{
					int X = 3;
				}

				t->_orientation = body->JointOrientations->Lookup(t->JointType());

				// if orientation is zero use parent orientation...
				auto parent = t->Parent();
				JointOrientation or = t->_orientation;

				auto v4 = XMFLOAT4(t->_orientation.Orientation.X, 
					t->_orientation.Orientation.Y, 
					t->_orientation.Orientation.Z, 
					t->_orientation.Orientation.W);

				if (XMVector4Equal(XMLoadFloat4(&v4), XMVectorZero()) && parent != nullptr)
				{
					or = parent->_orientation;
				}
				
				auto f4 = XMFLOAT4(or.Orientation.X, or.Orientation.Y, or.Orientation.Z, or.Orientation.W);
				auto rotMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&f4));
				if (parent != nullptr)
				{
					transformed = parent->_transformed;
				}

				auto translatedOrigin = XMMatrixTranslationFromVector(transformed);
				XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(translatedOrigin));

				// draw something here to confirm...
				//DrawAxis(context, _axis.get());

				auto translated = XMMatrixTranslation(0.0f, boneLength, 0.0f);

				auto mat = rotMatrix * translatedOrigin;
				XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(mat));
				
				auto f3 = XMFLOAT3(0.0f, boneLength, 0.0f);
				t->_transformed = XMVector3TransformCoord(XMLoadFloat3(&f3), mat);

				if (parent != nullptr)
				{
					// draw...
					DrawBone(context);
				}
			},
			[&terminated](shared_ptr<RigJoint>& t) 
				{ 
					if (t->JointType() == JointType::FootRight)
					{
						//terminated = true;
					}
					return terminated;
			});
	}
}

void Sample3DSceneRenderer::DrawGrid(ID3D11DeviceContext2 *context)
{
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixIdentity()));

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0);

	_grid->RenderBuffers(context);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

	// Attach our pixel shader.
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	// Draw the objects.
	context->DrawIndexed(_grid->IndexCount(), 0, 0);
}

void Sample3DSceneRenderer::DrawAxis(ID3D11DeviceContext2 *context, Axis *axis)
{
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixIdentity()));

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0);

	axis->RenderBuffers(context);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

	// Attach our pixel shader.
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	// Draw the objects.
	context->DrawIndexed(axis->IndexCount(), 0, 0);
}

void Sample3DSceneRenderer::DrawBone(ID3D11DeviceContext2 *context)
{
	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0);

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

	// Attach our pixel shader.
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	// Draw the objects.
	context->DrawIndexed(m_indexCount, 0, 0);
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	D3D11_RASTERIZER_DESC rasterizerState;
	rasterizerState.FillMode = D3D11_FILL_SOLID;
	rasterizerState.CullMode = D3D11_CULL_NONE;
	rasterizerState.FrontCounterClockwise = true;
	rasterizerState.DepthBias = false;
	rasterizerState.DepthBiasClamp = 0;
	rasterizerState.SlopeScaledDepthBias = 0;
	rasterizerState.DepthClipEnable = false;
	rasterizerState.ScissorEnable = false;
	rasterizerState.MultisampleEnable = false;
	rasterizerState.AntialiasedLineEnable = false;
	m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizerState, &m_pRasterState);

	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader
				)
			);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc [] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
				)
			);
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
				)
			);

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer) , D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_constantBuffer
				)
			);
	});

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this] () {

		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPositionColor cubeVertices[] = 
		{
			{XMFLOAT3(-0.5f, 0.0f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{ XMFLOAT3(-0.5f, 0.0f, 0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(-0.5f, 4.0f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(-0.5f, 4.0f, 0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f) },
			{ XMFLOAT3(0.5f, 0.0f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f) },
			{ XMFLOAT3(0.5f, 0.0f, 0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f) },
			{ XMFLOAT3(0.5f, 4.0f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f) },
			{ XMFLOAT3(0.5f, 4.0f, 0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f) },
		};

		D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
				)
			);

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the 
		// first triangle of this mesh.
		static const unsigned short cubeIndices [] =
		{
			0,2,1, // -x
			1,2,3,

			4,5,6, // +x
			5,7,6,

			0,1,5, // -y
			0,5,4,

			2,6,7, // +y
			2,7,3,

			0,4,6, // -z
			0,6,2,

			1,3,7, // +z
			1,7,5,
		};

		m_indexCount = ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = {0};
		indexBufferData.pSysMem = cubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_indexBuffer
				)
			);
	});

	auto createGridTask = (createCubeTask).then([this]() {
		_grid->Initialise(m_deviceResources->GetD3DDevice());
		_axis->Initialise(m_deviceResources->GetD3DDevice());
		_mainAxes->Initialise(m_deviceResources->GetD3DDevice());
		m_loadingComplete = true;
	});
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	_grid->ShutdownBuffers();
	_axis->ShutdownBuffers();
	_mainAxes->ShutdownBuffers();
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}