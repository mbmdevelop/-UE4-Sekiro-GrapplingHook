#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Math/Color.h"
#include "EngineUtils.h"

#include "UI/HookeablePointWidget.h"

#include "HookeablePoint.generated.h"

class UArrowComponent;
class UBillboardComponent;
class UBoxComponent;
class USphereComponent;
class UTexture2D;

class UHookeablePointWidget;

UCLASS()
class SEKIROHOOKPJ_API AHookeablePoint : public AActor
{
 GENERATED_BODY()
 public:	
  AHookeablePoint();

  FORCEINLINE bool IsValidPoint() const { return is_valid_; }
  FORCEINLINE bool IsHookeable() const { return is_hookeable_; }
  FORCEINLINE FVector GetTrackerLoc() const 
  { 
    return ui_tracker_->GetComponentLocation(); 
  }
  FORCEINLINE const FVector& GetLandLoc() const { return land_point_loc_; }
  FORCEINLINE const FVector2D& GetScreenPos() const 
  {
    return ui_tracker_->GetWBPScreenPos();
  }

  // NOTE: Allows Tick when PIE is stopped.
  virtual bool ShouldTickIfViewportsOnly() const override;
	virtual void Tick(float DeltaTime) override;
	void CustomUpdate(float character_point_distance);

	void SetCharacterFocused(bool new_value);
	void PlayInputAnimation();
	void HidePoint();

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Actor_Var")
	  float line_trace_length_;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor_Var")
    float depth_loc_offset_;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor_Var")
		float max_update_distance_;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor_Var")
		float min_update_distance_;

 protected:
  virtual void BeginPlay() override;

 private:
  void CalculateHookAndLandLoc();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components",
            meta = (AllowPrivateAccess = "true"))
	  UBillboardComponent *root_point_;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components",
            meta = (AllowPrivateAccess = "true"))
		UBillboardComponent *hook_point_;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components",
            meta = (AllowPrivateAccess = "true"))
		UBillboardComponent *land_point_;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components",
            meta = (AllowPrivateAccess = "true"))
		UBillboardComponent *status_sprite_;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components",
            meta = (AllowPrivateAccess = "true"))
		UHookeablePointWidget *ui_tracker_;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components",
            meta = (AllowPrivateAccess = "true"))
		UBoxComponent *activable_area_;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components",
            meta = (AllowPrivateAccess = "true"))
		USphereComponent *visibility_collision_;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components",
            meta = (AllowPrivateAccess = "true"))
		UArrowComponent *directional_arrow_;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Actor_Var",
            meta = (AllowPrivateAccess = "true"))
		TArray<UTexture2D*> status_sprites_;

  FVector land_point_loc_;

	bool is_valid_;
	bool is_hookeable_;
};
