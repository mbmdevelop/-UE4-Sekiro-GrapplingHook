#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Materials/MaterialInstanceDynamic.h"
#include "Animation/WidgetAnimation.h"

#include "HookeablePointTracker.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTrackerUnhide, 
                                            UHookeablePointTracker*, tracker);

UCLASS()
class SEKIROHOOKPJ_API UHookeablePointTracker : public UUserWidget
{
 GENERATED_BODY()
 public:
	UHookeablePointTracker(const FObjectInitializer & ObjectInitializer);
	
  FORCEINLINE void SetFocused (bool new_value) { focused_ = new_value; }
  FORCEINLINE bool FocusedState() const { return focused_; }

  FORCEINLINE bool IsTrackerFull() const { return is_tracker_full_; }
  FORCEINLINE FTrackerUnhide& UnhideDelegate() { return unhide_delegate_; }

	virtual void InitWidget(float start_update_distance, 
                          float scale_rate_multiplier);
  void InitMaterial();

  void CustomUpdate(float character_point_distance);

  void UnhideTracker();

  void PlayFocusAnimation();
  void PlayInputAnimation();

	void ResetWidget();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables | Material")
		FLinearColor max_scale_color_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables | Material")
		FLinearColor normal_scale_color_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables | Material")
		float max_scale_value_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables | Material")
		float min_scale_value_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables | Material")
		float ring_radius_offset_;

	// NOTE:UMG Elements
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UVerticalBox *root_;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage *tracker_point_;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation *focus_anim_;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidgetAnim))
		UWidgetAnimation *input_anim_;

 protected:
	void UpdatetTrackerPointMaterial(float character_point_distance);

  UPROPERTY()
	  UMaterialInstanceDynamic *tracker_point_mat_;

  FTrackerUnhide unhide_delegate_;

	float start_update_distance_;
	float scale_rate_multiplier_;
	bool focused_;
	bool is_tracker_full_;
};
