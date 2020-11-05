#include "UI/HookeablePointTracker.h"

#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Components/VerticalBox.h"

UHookeablePointTracker::UHookeablePointTracker(const FObjectInitializer & ObjectInitializer) 
    : Super(ObjectInitializer),
      tracker_point_mat_(nullptr),
      start_update_distance_(0.0f),
      scale_rate_multiplier_(0.0f),
      focused_(false),
      is_tracker_full_(false) {}

void UHookeablePointTracker::InitWidget(float start_update_distance, 
                                        float scale_rate_multiplier) 
{
	start_update_distance_ = start_update_distance;
	scale_rate_multiplier_ = scale_rate_multiplier;
	
  InitMaterial();

	SetRenderOpacity(0.0f);
	root_->SetRenderOpacity(1.0f);
}

void UHookeablePointTracker::InitMaterial() 
{
  tracker_point_mat_ = tracker_point_->GetDynamicMaterial();

  if (tracker_point_mat_)
  {
    tracker_point_mat_->SetScalarParameterValue("Radius", min_scale_value_);

    tracker_point_mat_->SetScalarParameterValue("InnerRadius", 
        max_scale_value_ + ring_radius_offset_);
    tracker_point_mat_->SetScalarParameterValue("OuterRadius", 
        max_scale_value_ + ring_radius_offset_ * 2.0f);

    tracker_point_mat_->SetVectorParameterValue("Color", normal_scale_color_);
  }
}

void UHookeablePointTracker::CustomUpdate(float character_point_distance)
{
  if (character_point_distance <= start_update_distance_)
  {
    UpdatetTrackerPointMaterial(character_point_distance);
  }
}

void UHookeablePointTracker::UnhideTracker() 
{
	if (GetRenderOpacity() == 0.0f) 
  {	
		SetRenderOpacity(1.0f);

		unhide_delegate_.Broadcast(this);
	}
}

void UHookeablePointTracker::PlayFocusAnimation()
{
  PlayAnimation(focus_anim_);
}

void UHookeablePointTracker::PlayInputAnimation() 
{	
	PlayAnimation(input_anim_);
}

void UHookeablePointTracker::ResetWidget()
{
  focused_ = false;

  if (tracker_point_mat_)
  {
    tracker_point_mat_->SetScalarParameterValue("Radius", min_scale_value_);
    tracker_point_mat_->SetVectorParameterValue("Color", normal_scale_color_);
  }

  tracker_point_->SetRenderScale(FVector2D(1.0f, 1.0f));
  tracker_point_->SetRenderOpacity(1.0f);
  root_->SetRenderOpacity(1.0f);
}

void UHookeablePointTracker::UpdatetTrackerPointMaterial(
    float character_point_distance) 
{
	float material_scale_rate = 0;

	// NOTE: Scale per distance value
	material_scale_rate = -1.0f * (character_point_distance / start_update_distance_) + 1.0f;
	material_scale_rate *= scale_rate_multiplier_;
	
	tracker_point_mat_->SetScalarParameterValue("Radius", 
      FMath::Clamp(material_scale_rate, min_scale_value_, max_scale_value_));

	// NOTE: Color per radius scale value
	if (material_scale_rate >= max_scale_value_) 
  {	
		tracker_point_mat_->SetVectorParameterValue("Color", max_scale_color_);
		is_tracker_full_ = true;
	}
  else 
  {	
		tracker_point_mat_->SetVectorParameterValue("Color", normal_scale_color_);
		is_tracker_full_ = false;
	}
}