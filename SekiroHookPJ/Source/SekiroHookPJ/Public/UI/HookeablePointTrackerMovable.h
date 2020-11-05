#pragma once

#include "CoreMinimal.h"
#include "UI/HookeablePointTracker.h"

#include "HookeablePointTrackerMovable.generated.h"

UCLASS()
class SEKIROHOOKPJ_API UHookeablePointTrackerMovable : public UHookeablePointTracker
{
 GENERATED_BODY()
 public:
  UHookeablePointTrackerMovable(const FObjectInitializer & ObjectInitializer);

	virtual void InitWidget(float start_update_distance, 
                          float scale_rate_multiplier) override;

	void CustomUpdate(const FVector2D& screen_pos, 
                    const FVector2D& fixed_screen_pos, 
					          float character_point_distance);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UImage *tracker_arrow_;

 private:
  void UpdatetTrackerArrow(const FVector2D& screen_pos, const FVector2D& fixed_screen_pos);
};
