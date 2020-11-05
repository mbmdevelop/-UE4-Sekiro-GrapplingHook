#include "UI/HookeablePointTrackerMovable.h"

#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "Components/VerticalBox.h"

UHookeablePointTrackerMovable::UHookeablePointTrackerMovable(const FObjectInitializer & ObjectInitializer) 
    : Super(ObjectInitializer) {}

void UHookeablePointTrackerMovable::InitWidget(float start_update_distance,
                                               float scale_rate_multiplier) 
{
	start_update_distance_ = start_update_distance;
	scale_rate_multiplier_ = scale_rate_multiplier;
	
  InitMaterial();

	SetRenderOpacity(0.0f);
	root_->SetRenderOpacity(1.0f);
	tracker_arrow_->SetRenderOpacity(0.0f);

	AddToViewport();
}

void UHookeablePointTrackerMovable::CustomUpdate(const FVector2D& screen_pos, 
                                                 const FVector2D& fixed_screen_pos,
                                                 float character_point_distance) 
{
	if (character_point_distance <= start_update_distance_) 
    UpdatetTrackerPointMaterial(character_point_distance);
	
	SetPositionInViewport(screen_pos);

	UpdatetTrackerArrow(screen_pos, fixed_screen_pos);
}

void UHookeablePointTrackerMovable::UpdatetTrackerArrow(
    const FVector2D& screen_pos, const FVector2D& fixed_screen_pos) 
{
	// NOTE: Subtract the arrow image height to get the middle point of the arrow.
	FVector2D arrow_pos = screen_pos - FVector2D(0.0f, 
                                               tracker_arrow_->GetDesiredSize().Y);

	float arrow_facing_angle = FMath::RadiansToDegrees(FMath::Atan2(
      fixed_screen_pos.Y - arrow_pos.Y, fixed_screen_pos.X - arrow_pos.X));
	// NOTE: Addition of 90 to set the arrow "sprite" to 0 degrees + the 
  // face rotation to the point
	tracker_arrow_->SetRenderTransformAngle(arrow_facing_angle + 90.0f);

	FVector2D tracker_to_point = fixed_screen_pos - screen_pos;
	float distance_to_point = tracker_to_point.SizeSquared();

	if (distance_to_point > 10000.0f)
  	tracker_arrow_->SetRenderOpacity(1.0f);
	else
		tracker_arrow_->SetRenderOpacity(0.0f);
}

