// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/UMG/UIUserWidget.h"
#include "Animation/UMGSequencePlayer.h"

void UUIUserWidget::FadeOutDisplay(bool bInstant /* = false */, float PlayRate /*= 1.f */)
{
	if (FadeAnim)
	{
		AnimSequencePlayer = !bInstant ? PlayAnimationForward(FadeAnim, PlayRate) : PlayAnimationForward(FadeAnim, 10.f);
	} else
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
}

void UUIUserWidget::FadeInDisplay(bool bInstant /* = false */, float PlayRate /*= 1.f */)
{
	if (FadeAnim)
	{
		AnimSequencePlayer = !bInstant ? PlayAnimationReverse(FadeAnim, PlayRate) : PlayAnimationReverse(FadeAnim, 10.f);	
	} else
	{
		SetVisibility(ESlateVisibility::Visible);
	}
}

void UUIUserWidget::StartBlinking()
{
	if(FadeAnim)
	{
		if(!AnimSequencePlayer)
		{
			AnimSequencePlayer = PlayAnimation(FadeAnim, FadeAnim->GetEndTime(), 0, EUMGSequencePlayMode::PingPong, 1.f, false);
		} else if(AnimSequencePlayer->GetPlaybackStatus() != EMovieScenePlayerStatus::Playing)
		{
			AnimSequencePlayer = PlayAnimation(FadeAnim, FadeAnim->GetEndTime(), 0, EUMGSequencePlayMode::PingPong, 1.f, false);
		}
	}
}

void UUIUserWidget::StopBlinking()
{
	if(FadeAnim)
	{
		double CurrTime = 0;
		if(AnimSequencePlayer)
			CurrTime = AnimSequencePlayer->GetCurrentTime().AsSeconds();
		AnimSequencePlayer = PlayAnimation(FadeAnim, CurrTime);
	}
}