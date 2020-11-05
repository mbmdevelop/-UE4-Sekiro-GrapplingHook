#include "UI/HookeablePointWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UHookeablePointWidget::UHookeablePointWidget(const FObjectInitializer & ObjectInitializer) 
    : Super(ObjectInitializer),
      wbp_tracker_(nullptr),
      wbp_movable_tracker_(nullptr),
      screen_pos_(FVector::ZeroVector),
      fixed_screen_pos_(FVector::ZeroVector),
      out_screen_bounds_(false),
      focus_anim_flag_(true) 
      {}

void UHookeablePointWidget::CustomInit() 
{
	wbp_tracker_ = Cast<UHookeablePointTracker>(Widget);
	wbp_movable_tracker_ = CreateWidget<UHookeablePointTrackerMovable>(
      GetWorld(), wbp_movable_class_);

	wbp_tracker_->InitWidget(start_update_distance_, scale_rate_multiplier_);
	wbp_movable_tracker_->InitWidget(start_update_distance_, scale_rate_multiplier_);

	wbp_tracker_->UnhideDelegate().AddDynamic(this, 
      &UHookeablePointWidget::OnTrackerUnhide);
	wbp_movable_tracker_->UnhideDelegate().AddDynamic(this, 
      &UHookeablePointWidget::OnTrackerUnhide);
}

void UHookeablePointWidget::CustomUpdate(const FVector &point_world_pos,
	                                       float character_point_distance)
{
	// NOTE: Covert the 3D world-loc of the point to screen 2D loc 
	APlayerController *tmp_player_controller = UGameplayStatics::GetPlayerController(
      GetWorld(), 0);
	bool on_screen = UGameplayStatics::ProjectWorldToScreen(tmp_player_controller, 
                                                          point_world_pos, 
                                                          screen_pos_, false);

	// NOTE: Calculate the offset position to the WBP with the tracker arrow.
	FVector2D pos_offset_ = FVector2D(
      wbp_movable_tracker_->GetDesiredSize().X * 0.5f, 
      wbp_movable_tracker_->GetDesiredSize().Y * 0.75f);
	
  screen_pos_ -= pos_offset_;
	fixed_screen_pos_ = screen_pos_;

	ConstrainScreenBounds(point_world_pos);

	wbp_tracker_->CustomUpdate(character_point_distance);
	wbp_movable_tracker_->CustomUpdate(screen_pos_, fixed_screen_pos_,
		                                 character_point_distance);

	if (on_screen) 
	{
		if (TrackerActive()) 
		{
			// NOTE: Hide or Unhide both WBP depending on the screen position
			VisibilityBehaviour();
		}
		else 
		{
			HideTrackers();
		}
	}
	else 
	{
		HideTrackers();
	}
}

void UHookeablePointWidget::SetCharacterFocused(bool new_value) 
{
	if (wbp_tracker_)
    wbp_tracker_->SetFocused(new_value);
	
  if (wbp_movable_tracker_)
    wbp_movable_tracker_->SetFocused(new_value);
}

void UHookeablePointWidget::HideTrackers()
{
  wbp_tracker_->SetRenderOpacity(0.0f);
  wbp_movable_tracker_->SetRenderOpacity(0.0f);

  wbp_tracker_->ResetWidget();
  wbp_movable_tracker_->ResetWidget();

  focus_anim_flag_ = true;
}

bool UHookeablePointWidget::CheckIfIsHookeable() const
{
	if (!wbp_tracker_) return false;
	if (!wbp_movable_tracker_) return false;
	
	if ((wbp_tracker_->IsTrackerFull() && 
       wbp_tracker_->GetRenderOpacity() == 1.0f) ||
		  (wbp_movable_tracker_->IsTrackerFull() && 
       wbp_movable_tracker_->GetRenderOpacity() == 1.0f))
		return true;
	else
		return false;
}

void UHookeablePointWidget::PlayInputAnimation() 
{
  if (!wbp_tracker_) return;
	if (!wbp_movable_tracker_) return;
		
	wbp_tracker_->PlayInputAnimation();
	wbp_movable_tracker_->PlayInputAnimation();
}

void UHookeablePointWidget::OnTrackerUnhide(UHookeablePointTracker *tracker)
{
  if (!tracker) return;

  if (focus_anim_flag_)
  {
    tracker->PlayFocusAnimation();
    focus_anim_flag_ = false;
  }
}

void UHookeablePointWidget::ConstrainScreenBounds(const FVector &point_world_pos) 
{
	// NOTE: Screen bounds
	const FVector2D viewport_size = FVector2D(
      GEngine->GameViewport->Viewport->GetSizeXY());
	
  FVector2D screen_bounds = FVector2D(
      viewport_size.X - (screen_margin_offset_ * 2.0f), 
      viewport_size.Y - (screen_margin_offset_ * 2.0f));

	out_screen_bounds_ = false;

	//Top
	if (screen_pos_.Y < screen_margin_offset_) 
  {
		screen_pos_.Y = (screen_margin_offset_ - screen_pos_.Y) + screen_pos_.Y;
		out_screen_bounds_ = true;
	}

	//Left
	if (screen_pos_.X < screen_margin_offset_) 
  {
		screen_pos_.X = (screen_margin_offset_ - screen_pos_.X) + screen_pos_.X;
		out_screen_bounds_ = true;
	}

	//Bottom
	if (screen_pos_.Y > screen_bounds.Y) 
  {
		screen_pos_.Y = screen_pos_.Y - (screen_pos_.Y - screen_bounds.Y);
		out_screen_bounds_ = true;
	}

	//Right
	if (screen_pos_.X > screen_bounds.X) 
  {
		screen_pos_.X = screen_pos_.X - (screen_pos_.X - screen_bounds.X);
		out_screen_bounds_ = true;
	}
}

bool UHookeablePointWidget::TrackerActive() const
{
  if (!wbp_tracker_) return false;
  if (!wbp_movable_tracker_) return false;

	if (wbp_tracker_->FocusedState() && wbp_movable_tracker_->FocusedState())
		return true;
	else
		return false;
}

void UHookeablePointWidget::VisibilityBehaviour() 
{
	if (out_screen_bounds_) 
  {
		wbp_tracker_->SetRenderOpacity(0.0f);
		wbp_movable_tracker_->UnhideTracker();
	}
	else 
  {
		wbp_tracker_->UnhideTracker();
		wbp_movable_tracker_->SetRenderOpacity(0.0f);
	}
}