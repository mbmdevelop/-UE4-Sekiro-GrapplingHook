#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"

#include "Engine/EngineBaseTypes.h"

#include "UI/HookeablePointTracker.h"
#include "UI/HookeablePointTrackerMovable.h"

#include "HookeablePointWidget.generated.h"

UCLASS()
class SEKIROHOOKPJ_API UHookeablePointWidget : public UWidgetComponent
{
 GENERATED_BODY()
 public:
	UHookeablePointWidget(const FObjectInitializer & ObjectInitializer);

  FORCEINLINE const FVector2D& GetWBPScreenPos() const { return screen_pos_; }

	void CustomInit();
	void CustomUpdate(const FVector &point_world_pos, 
                    float character_point_distance);


	void SetCharacterFocused(bool new_value);
	void HideTrackers();
	bool CheckIfIsHookeable() const;
	void PlayInputAnimation();

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "UWidgetComp_Var | Widget")
	TSubclassOf<UHookeablePointTrackerMovable> wbp_movable_class_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWidgetComp_Var | Material")
	  float start_update_distance_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWidgetComp_Var | Material")
		float scale_rate_multiplier_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UWidgetComp_Var | Material")
		float screen_margin_offset_;

 private:
   UFUNCTION()
     void OnTrackerUnhide(UHookeablePointTracker *tracker);

	void ConstrainScreenBounds(const FVector &point_world_pos);
	bool TrackerActive() const;
	void VisibilityBehaviour();
  
  UPROPERTY()
    // NOTE: Static WBP managed by the UWidgetComp.
	  UHookeablePointTracker *wbp_tracker_; 
  
  UPROPERTY()
    // NOTE: // Movable WBP created and controlled depending on the screen bounds. 
    UHookeablePointTrackerMovable *wbp_movable_tracker_; 

	FVector2D screen_pos_;
	FVector2D fixed_screen_pos_;

	bool out_screen_bounds_;
	bool focus_anim_flag_;
};