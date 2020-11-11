#include "Actors/HookeablePoint.h"

#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"

#include "UI/HookeablePointWidget.h"

constexpr bool g_draw_debug_helpers = false;

AHookeablePoint::AHookeablePoint()
    : land_point_loc_(FVector::ZeroVector),
      is_valid_(false),
      is_hookeable_(false)
{
	PrimaryActorTick.bCanEverTick = true;

	root_point_ = CreateDefaultSubobject<UBillboardComponent>(TEXT("Root Point"));
	hook_point_ = CreateDefaultSubobject<UBillboardComponent>(TEXT("Hook Point"));
	land_point_ = CreateDefaultSubobject<UBillboardComponent>(TEXT("Land Point"));
	status_sprite_ = CreateDefaultSubobject<UBillboardComponent>(TEXT("Status BillBoard"));
	ui_tracker_ = CreateDefaultSubobject<UHookeablePointWidget>(TEXT("UI Tracker"));
	activable_area_ = CreateDefaultSubobject<UBoxComponent>(TEXT("Activable Area"));
	visibility_collision_ = CreateDefaultSubobject<USphereComponent>(TEXT("Visibility Collision"));
	directional_arrow_ = CreateDefaultSubobject<UArrowComponent>(TEXT("Directional Arrow"));

	RootComponent = root_point_;
	hook_point_->SetupAttachment(RootComponent);
	land_point_->SetupAttachment(RootComponent);
	status_sprite_->SetupAttachment(RootComponent);
	ui_tracker_->SetupAttachment(RootComponent);
	activable_area_->SetupAttachment(RootComponent);
	visibility_collision_->SetupAttachment(RootComponent);
	directional_arrow_->SetupAttachment(RootComponent);

	visibility_collision_->InitSphereRadius(60.0f);
}

bool AHookeablePoint::ShouldTickIfViewportsOnly() const
{
  return true;
}

void AHookeablePoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// NOTE: Active the DebugHelpers when the game is not playing.
	if (!GetWorld()->IsPlayInEditor())
		CalculateHookAndLandLoc();
}

void AHookeablePoint::CustomUpdate(float character_point_distance)
{
	ui_tracker_->CustomUpdate(ui_tracker_->GetComponentLocation(),
		character_point_distance);

	is_hookeable_ = ui_tracker_->CheckIfIsHookeable();
}

void AHookeablePoint::SetCharacterFocused(bool new_value) 
{
	ui_tracker_->SetCharacterFocused(new_value);
}

void AHookeablePoint::PlayInputAnimation() 
{
	ui_tracker_->PlayInputAnimation();
}

void AHookeablePoint::HidePoint()
{
  ui_tracker_->HideTrackers();
}

void AHookeablePoint::BeginPlay()
{
  Super::BeginPlay();

  root_point_->SetHiddenInGame(!g_draw_debug_helpers);
  hook_point_->SetHiddenInGame(!g_draw_debug_helpers);
  land_point_->SetHiddenInGame(!g_draw_debug_helpers);
  status_sprite_->SetHiddenInGame(!g_draw_debug_helpers);
  visibility_collision_->SetHiddenInGame(!g_draw_debug_helpers);

  ui_tracker_->CustomInit();
  CalculateHookAndLandLoc();
}

void AHookeablePoint::CalculateHookAndLandLoc()
{
	bool draw_debug_in_play;

	if (GetWorld()->IsPlayInEditor())
		draw_debug_in_play = g_draw_debug_helpers;
	else
		draw_debug_in_play = false;

	FCollisionQueryParams trace_params(TEXT("HookeablePointTrace"), false, this);
	trace_params.bReturnPhysicalMaterial = false;

	FVector trace_end_offset = FVector(0.0f, 0.0f, -1.0f) * line_trace_length_;

	// NOTE: Hook Point Trace
	FVector hook_trace_start = hook_point_->GetComponentLocation();
	FVector hook_trace_end = hook_trace_start + trace_end_offset;
	FHitResult hook_trace_hit_details = FHitResult(ForceInit);

	bool hook_trace_hited = GetWorld()->LineTraceSingleByChannel(
      hook_trace_hit_details,
		  hook_trace_start,
		  hook_trace_end,
		  ECC_GameTraceChannel2,
		  trace_params);

	if (hook_trace_hited)
	{
		// NOTE: Hook Point Trace Line
		DrawDebugLine(GetWorld(), hook_trace_start, hook_trace_hit_details.Location,
			            FColor::Red, draw_debug_in_play, -1.0f, 0, 2.0f);

		// NOTE: Hook Point Loc
		DrawDebugPoint(GetWorld(), hook_trace_hit_details.Location, 20.0f,
			             FColor::Green, draw_debug_in_play, -1.0f);

		ui_tracker_->SetWorldLocation(hook_trace_hit_details.Location);
	}
	else
	{
		// NOTE: Hook Point Trace Line
		DrawDebugLine(GetWorld(), hook_trace_start, hook_trace_end, FColor::Red, 
                  draw_debug_in_play,-1.0f, 0, 2.0f);
	}

	// NOTE: Land Point Trace
	FVector land_trace_start = land_point_->GetComponentLocation();
	FVector land_trace_end = land_trace_start + trace_end_offset;
	FHitResult land_trace_hit_details = FHitResult(ForceInit);

	bool land_trace_hited = GetWorld()->LineTraceSingleByChannel(
      land_trace_hit_details,
		  land_trace_start,
		  land_trace_end,
		  ECC_GameTraceChannel2,
		  trace_params);

	if (land_trace_hited)
	{
		// NOTE: Land Point Trace Line
		DrawDebugLine(GetWorld(), land_trace_start, land_trace_hit_details.Location,
			            FColor::Red, draw_debug_in_play, -1.0f, 0, 2.0f);

		// NOTE: Land Loc
		DrawDebugPoint(GetWorld(), 
                   land_trace_hit_details.Location + FVector(0.0f, 0.0f, 97.0f),
			             20.0f, FColor::Blue, draw_debug_in_play, -1.0f);

		land_point_loc_ = land_trace_hit_details.Location;
	}
	else
	{
		// NOTE: Land Point Trace Line
		DrawDebugLine(GetWorld(), land_trace_start, land_trace_end, FColor::Red, 
                  draw_debug_in_play,-1.0f, 0, 2.0f);
	}

	// NOTE: Check if the point is valid or not
	if (hook_trace_hited && land_trace_hited)
		is_valid_ = true;
	else
		is_valid_ = false;

	status_sprite_->SetSprite(status_sprites_[is_valid_]);
}