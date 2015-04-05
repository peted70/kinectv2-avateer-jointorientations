#pragma once
#define FACTOR 5
#include"RigJoint.h"

using namespace std;

#define ORANGE_COL XMFLOAT3(0.929f, 0.49f, 0.192f)
#define GREEN_COL XMFLOAT3(0.439f, 0.678f, 0.278f)
#define DARKBLUE_COL XMFLOAT3(0.267f, 0.329f, 0.416f)
#define LIGHTBLUE_COL XMFLOAT3(0.357f, 0.608f, 0.835f)
#define YELLOW_COL XMFLOAT3(1.0f, 0.753f, 0.0f)

class KinectUtility
{
public:
	static const float lengthFromSpineBaseToSpineMid;
	static const float lengthFromSpineMidToSpineShoulder;
	static const float lengthFromSpineMidToHipRight;
	static const float lengthFromHipRightToKneeRight;
	static const float lengthFromKneeRightToAnkleRight;
	static const float lengthFromAnkleToFoot;
	static const float lengthFromShoulderToNeck;
	static const float lengthFromNeckToHead;
	static const float lengthFromSpineShoulderToShoulder;
	static const float lengthFromShoulderToElbow;
	static const float lengthFromElbowToWrist;
	static const float lengthFromWristToHand;
	static const float lengthFromHandToThumb;
	static const float lengthFromHandToTip;

	static shared_ptr<RigJoint>	CreateBoneHierarchy();
	static void TraverseBoneHierarchy(shared_ptr<RigJoint>& node, 
				function<void(shared_ptr<RigJoint>&)> f);
};

