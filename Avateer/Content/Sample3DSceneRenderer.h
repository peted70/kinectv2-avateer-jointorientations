#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include <DirectXMath.h>
#include "RigJoint.h"
#include "DXGrid.h"
#include "Axis.h"

using namespace WindowsPreview::Kinect;
using namespace Platform::Collections;
using namespace DirectX;
using namespace std;
using namespace Windows::System;

namespace Avateer
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void StartTracking(float positionX, float positionY, VirtualKeyModifiers mod);
		void TrackingUpdate(float positionX, float positionY, VirtualKeyModifiers mod);
		void StopTracking();
		bool IsTracking() { return m_tracking; }


	private:
		void Rotate(float radians);
		void DrawBone(ID3D11DeviceContext2 *context);
		void DrawGrid(ID3D11DeviceContext2 *context);
		void DrawAxis(ID3D11DeviceContext2 *context, Axis *axis);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32	m_indexCount;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;

		float _yaw;
		float _pitch;
		float _roll;
		float _zoom;

		KinectSensor^ _sensor;
		BodyFrameReader^ _reader;
		Vector<Body^>^ _bodies;

		ID3D11RasterizerState * m_pRasterState;

		XMFLOAT4 _eye;
		shared_ptr<RigJoint> _boneHierarchy;

		unique_ptr<DXGrid> _grid;
		unique_ptr<Axis> _axis;
		unique_ptr<Axis> _mainAxes;
		
	};
}

