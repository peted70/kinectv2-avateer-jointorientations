#include "pch.h"
#include "RigJoint.h"

RigJoint::RigJoint(WindowsPreview::Kinect::JointType type, XMFLOAT3 colour, float boneLength, wstring name) :
_jointType(type), _colour(colour), _boneLength(boneLength), _name(name)
{
}

const shared_ptr<RigJoint> RigJoint::AddChild(shared_ptr<RigJoint> child)
{
	child->SetParent(shared_ptr<RigJoint>(this));
	_children.push_back(child);
	return child;
}
