#pragma once

using namespace std;
using namespace WindowsPreview::Kinect;
using namespace DirectX;

class RigJoint
{
public:
	RigJoint(JointType type, XMFLOAT3 colour, float boneLength, wstring name);
	const vector<shared_ptr<RigJoint>>& Children() { return _children; }
	const shared_ptr<RigJoint> AddChild(shared_ptr<RigJoint> child);
	XMFLOAT3 getColour() { return _colour; }
	void setColour(XMFLOAT3 col) { _colour = col; }

	const wstring& Name() { return _name; }
	void SetParent(shared_ptr<RigJoint> parent) { _parent = parent; }
	const shared_ptr<RigJoint>& Parent() { return _parent; }

	float BoneLength() { return _boneLength; }
	const JointType& JointType() { return _jointType; }

	XMVECTOR _transformed;
	JointOrientation _orientation;

private:
	shared_ptr<RigJoint> _parent;
	vector<shared_ptr<RigJoint>> _children;
	float _boneLength;
	XMFLOAT3 _colour;
	WindowsPreview::Kinect::JointType _jointType;
	wstring _name;
};

