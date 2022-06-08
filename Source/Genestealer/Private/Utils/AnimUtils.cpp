// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/AnimUtils.h"

void UAnimUtils::AnimUtils_MeshAdditiveFramePose(UObject* InAnimSequence, UAnimSequence* FramePoseAsset)
{
	UAnimSequence* CastedAnimSequence = Cast<UAnimSequence>(InAnimSequence);
	if(!CastedAnimSequence) return;
	CastedAnimSequence->AdditiveAnimType = AAT_RotationOffsetMeshSpace;
	CastedAnimSequence->RefPoseType = ABPT_AnimFrame;
	CastedAnimSequence->RefPoseSeq = FramePoseAsset;
	CastedAnimSequence->RefFrameIndex = 0;
}

void UAnimUtils::AnimUtils_CopyCurves(UObject* AnimToCopy, UAnimSequence* AnimSource)
{
	UAnimSequence* CastedAnimToCopy = Cast<UAnimSequence>(AnimToCopy);
	if(!CastedAnimToCopy || !AnimSource) return;
	CastedAnimToCopy->Modify();
	// CastedAnimToCopy->RawCurveData = AnimSource->GetCurveData();
}
