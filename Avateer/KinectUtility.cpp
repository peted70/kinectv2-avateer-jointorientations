#include "pch.h"
#include "KinectUtility.h"

const float KinectUtility::lengthFromSpineBaseToSpineMid = FACTOR*0.2f;
const float KinectUtility::lengthFromSpineMidToSpineShoulder = FACTOR*0.2f;
const float KinectUtility::lengthFromSpineMidToHipRight = FACTOR*0.1f;
const float KinectUtility::lengthFromHipRightToKneeRight = FACTOR*0.3f;
const float KinectUtility::lengthFromKneeRightToAnkleRight = FACTOR*0.2f;
const float KinectUtility::lengthFromAnkleToFoot = FACTOR*0.1f;
const float KinectUtility::lengthFromShoulderToNeck = FACTOR*0.05f;
const float KinectUtility::lengthFromNeckToHead = FACTOR*0.1f;
const float KinectUtility::lengthFromSpineShoulderToShoulder = FACTOR*0.2f;
const float KinectUtility::lengthFromShoulderToElbow = FACTOR*0.3f;
const float KinectUtility::lengthFromElbowToWrist = FACTOR*0.2f;
const float KinectUtility::lengthFromWristToHand = FACTOR*0.05f;
const float KinectUtility::lengthFromHandToThumb = FACTOR*0.05f;
const float KinectUtility::lengthFromHandToTip = FACTOR*0.05f;

shared_ptr<RigJoint> KinectUtility::CreateBoneHierarchy()
{
	auto boneHierarchy = make_shared<RigJoint>(JointType::SpineBase, XMFLOAT3(1.0f, 0.0f, 0.0f), 1.0f, L"Spine Base");

	boneHierarchy->AddChild(make_shared<RigJoint>(JointType::HipRight, XMFLOAT3(0.0f, 1.0f, 0.0f), lengthFromSpineMidToHipRight, L"Hip Right"))
		->AddChild(make_shared<RigJoint>(JointType::KneeRight, XMFLOAT3(0.0f, 0.0f, 1.0f), lengthFromHipRightToKneeRight, L"Knee Right"))
		->AddChild(make_shared<RigJoint>(JointType::AnkleRight, XMFLOAT3(0.0f, 1.0f, 1.0f), lengthFromKneeRightToAnkleRight, L"Ankle Right"))
		->AddChild(make_shared<RigJoint>(JointType::FootRight, XMFLOAT3(0.0f, 1.0f, 1.0f), lengthFromAnkleToFoot, L"Foot Right"));

	boneHierarchy->AddChild(make_shared<RigJoint>(JointType::HipLeft, XMFLOAT3(0.0f, 1.0f, 0.0f), lengthFromSpineMidToHipRight, L"Hip Left"))
		->AddChild(make_shared<RigJoint>(JointType::KneeLeft, XMFLOAT3(0.0f, 0.0f, 1.0f), lengthFromHipRightToKneeRight, L"Knee Left"))
		->AddChild(make_shared<RigJoint>(JointType::AnkleLeft, XMFLOAT3(0.0f, 1.0f, 1.0f), lengthFromKneeRightToAnkleRight, L"Ankle Left"))
		->AddChild(make_shared<RigJoint>(JointType::FootLeft, XMFLOAT3(0.0f, 1.0f, 1.0f), lengthFromAnkleToFoot, L"Foot Left"));

	auto spineShoulder = boneHierarchy->AddChild(make_shared<RigJoint>(JointType::SpineMid, XMFLOAT3(0.0f, 1.0f, 0.0f), lengthFromSpineBaseToSpineMid, L"Spine Mid"))
		->AddChild(make_shared<RigJoint>(JointType::SpineShoulder, XMFLOAT3(0.0f, 1.0f, 0.0f), lengthFromSpineMidToSpineShoulder, L"Spine Shoulder"));

	spineShoulder->AddChild(make_shared<RigJoint>(JointType::Neck, XMFLOAT3(0.0f, 1.0f, 0.0f), lengthFromShoulderToNeck, L"Neck"))
		->AddChild(make_shared<RigJoint>(JointType::Head, XMFLOAT3(1.0f, 1.0f, 1.0f), lengthFromNeckToHead, L"Head"));

	auto wristRight = spineShoulder->AddChild(make_shared<RigJoint>(JointType::ShoulderRight, XMFLOAT3(0.0f, 1.0f, 0.0f), lengthFromSpineShoulderToShoulder, L"Shoulder Right"))
		->AddChild(make_shared<RigJoint>(JointType::ElbowRight, XMFLOAT3(0.0f, 0.0f, 1.0f), lengthFromShoulderToElbow, L"Elbow Right"))
		->AddChild(make_shared<RigJoint>(JointType::WristRight, XMFLOAT3(1.0f, 1.0f, 0.0f), lengthFromElbowToWrist, L"Wrist Right"));

	auto handRight = wristRight->AddChild(make_shared<RigJoint>(JointType::HandRight, XMFLOAT3(1.0f, 1.0f, 0.0f), lengthFromWristToHand, L"Hand Right"));

	wristRight->AddChild(make_shared<RigJoint>(JointType::ThumbRight, XMFLOAT3(0.0f, 1.0f, 1.0f), lengthFromHandToThumb, L"Thumb Right"));
	handRight->AddChild(make_shared<RigJoint>(JointType::HandTipRight, XMFLOAT3(0.0f, 1.0f, 1.0f), lengthFromHandToTip, L"Hand Tip Right"));

	auto wristLeft = spineShoulder->AddChild(make_shared<RigJoint>(JointType::ShoulderLeft, XMFLOAT3(0.0f, 1.0f, 0.0f), lengthFromSpineShoulderToShoulder, L"Shoulder Left"))
		->AddChild(make_shared<RigJoint>(JointType::ElbowLeft, XMFLOAT3(0.0f, 0.0f, 1.0f), lengthFromShoulderToElbow, L"Elbow Left"))
		->AddChild(make_shared<RigJoint>(JointType::WristLeft, XMFLOAT3(1.0f, 1.0f, 0.0f), lengthFromElbowToWrist, L"Wrist Left"));

	auto handLeft = wristLeft->AddChild(make_shared<RigJoint>(JointType::HandLeft, XMFLOAT3(1.0f, 1.0f, 0.0f), lengthFromWristToHand, L"Hand Left"));
	wristLeft->AddChild(make_shared<RigJoint>(JointType::ThumbLeft, XMFLOAT3(0.0f, 1.0f, 1.0f), lengthFromHandToThumb, L"Thumb Left"));
	handLeft->AddChild(make_shared<RigJoint>(JointType::HandTipLeft, XMFLOAT3(0.0f, 1.0f, 1.0f), lengthFromHandToTip, L"Hand Tip Left"));

	return boneHierarchy;
}

void KinectUtility::TraverseBoneHierarchy(shared_ptr<RigJoint>& node, std::function<void(shared_ptr<RigJoint>&)> f,
					std::function<bool(shared_ptr<RigJoint>&)> pred)
{
	if (!node)
		return;

	if (pred(node))
		return;

	f(node);

	for (auto child : node->Children())
	{
		TraverseBoneHierarchy(child, f, pred);
	}
}

