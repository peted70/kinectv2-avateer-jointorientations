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

#define ORANGE_COL XMFLOAT3(0.929f, 0.49f, 0.192f)
#define GREEN_COL XMFLOAT3(0.439f, 0.678f, 0.278f)
#define DARKBLUE_COL XMFLOAT3(0.267f, 0.329f, 0.416f)
#define LIGHTBLUE_COL XMFLOAT3(0.357f, 0.608f, 0.835f)
#define YELLOW_COL XMFLOAT3(1.0f, 0.753f, 0.0f)

shared_ptr<RigJoint> KinectUtility::CreateBoneHierarchy()
{
	auto boneHierarchy = make_shared<RigJoint>(JointType::SpineBase, LIGHTBLUE_COL, 1.0f, L"Spine Base");

	boneHierarchy->AddChild(make_shared<RigJoint>(JointType::HipRight, YELLOW_COL, lengthFromSpineMidToHipRight, L"Hip Right"))
		->AddChild(make_shared<RigJoint>(JointType::KneeRight, YELLOW_COL, lengthFromHipRightToKneeRight, L"Knee Right"))
		->AddChild(make_shared<RigJoint>(JointType::AnkleRight, YELLOW_COL, lengthFromKneeRightToAnkleRight, L"Ankle Right"))
		->AddChild(make_shared<RigJoint>(JointType::FootRight, YELLOW_COL, lengthFromAnkleToFoot, L"Foot Right"));

	boneHierarchy->AddChild(make_shared<RigJoint>(JointType::HipLeft, YELLOW_COL, lengthFromSpineMidToHipRight, L"Hip Left"))
		->AddChild(make_shared<RigJoint>(JointType::KneeLeft, YELLOW_COL, lengthFromHipRightToKneeRight, L"Knee Left"))
		->AddChild(make_shared<RigJoint>(JointType::AnkleLeft, YELLOW_COL, lengthFromKneeRightToAnkleRight, L"Ankle Left"))
		->AddChild(make_shared<RigJoint>(JointType::FootLeft, YELLOW_COL, lengthFromAnkleToFoot, L"Foot Left"));

	auto spineShoulder = boneHierarchy->AddChild(make_shared<RigJoint>(JointType::SpineMid, GREEN_COL, lengthFromSpineBaseToSpineMid, L"Spine Mid"))
		->AddChild(make_shared<RigJoint>(JointType::SpineShoulder, GREEN_COL, lengthFromSpineMidToSpineShoulder, L"Spine Shoulder"));

	spineShoulder->AddChild(make_shared<RigJoint>(JointType::Neck, DARKBLUE_COL, lengthFromShoulderToNeck, L"Neck"))
		->AddChild(make_shared<RigJoint>(JointType::Head, DARKBLUE_COL, lengthFromNeckToHead, L"Head"));

	auto wristRight = spineShoulder->AddChild(make_shared<RigJoint>(JointType::ShoulderRight, ORANGE_COL, lengthFromSpineShoulderToShoulder, L"Shoulder Right"))
		->AddChild(make_shared<RigJoint>(JointType::ElbowRight, ORANGE_COL, lengthFromShoulderToElbow, L"Elbow Right"))
		->AddChild(make_shared<RigJoint>(JointType::WristRight, ORANGE_COL, lengthFromElbowToWrist, L"Wrist Right"));

	auto handRight = wristRight->AddChild(make_shared<RigJoint>(JointType::HandRight, ORANGE_COL, lengthFromWristToHand, L"Hand Right"));

	wristRight->AddChild(make_shared<RigJoint>(JointType::ThumbRight, ORANGE_COL, lengthFromHandToThumb, L"Thumb Right"));
	handRight->AddChild(make_shared<RigJoint>(JointType::HandTipRight, ORANGE_COL, lengthFromHandToTip, L"Hand Tip Right"));

	auto wristLeft = spineShoulder->AddChild(make_shared<RigJoint>(JointType::ShoulderLeft, ORANGE_COL, lengthFromSpineShoulderToShoulder, L"Shoulder Left"))
		->AddChild(make_shared<RigJoint>(JointType::ElbowLeft, ORANGE_COL, lengthFromShoulderToElbow, L"Elbow Left"))
		->AddChild(make_shared<RigJoint>(JointType::WristLeft, ORANGE_COL, lengthFromElbowToWrist, L"Wrist Left"));

	auto handLeft = wristLeft->AddChild(make_shared<RigJoint>(JointType::HandLeft, ORANGE_COL, lengthFromWristToHand, L"Hand Left"));
	wristLeft->AddChild(make_shared<RigJoint>(JointType::ThumbLeft, ORANGE_COL, lengthFromHandToThumb, L"Thumb Left"));
	handLeft->AddChild(make_shared<RigJoint>(JointType::HandTipLeft, ORANGE_COL, lengthFromHandToTip, L"Hand Tip Left"));

	return boneHierarchy;
}

void KinectUtility::TraverseBoneHierarchy(shared_ptr<RigJoint>& node, std::function<void(shared_ptr<RigJoint>&)> f)
{
	if (!node)
		return;

	f(node);

	for (auto child : node->Children())
	{
		TraverseBoneHierarchy(child, f);
	}
}

