#include "FP_FirstPersonHUD.h"
#include "Global.h"
#include "Engine/Canvas.h"
#include "Engine/Font.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Texture2D.h"
#include "FP_FirstPersonCharacter.h"
#include "Game/CPlayerState.h"

AFP_FirstPersonHUD::AFP_FirstPersonHUD()
{
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;

	CHelpers::GetAsset(&Font, "Font'/Game/Widgets/Fonts/Audiowide-Regular_Font.Audiowide-Regular_Font'");
}

void AFP_FirstPersonHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw the crosshair
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	const FVector2D CrosshairDrawPosition( (Center.X - (CrosshairTex->GetSurfaceWidth() * 0.5)),
										   (Center.Y - (CrosshairTex->GetSurfaceHeight() * 0.5f)) );

	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );


	// Draw PlayerState Score
	CheckNull(PlayerOwner->GetPawn());
	
	AFP_FirstPersonCharacter* player = Cast<AFP_FirstPersonCharacter>(PlayerOwner->GetPawn());
	CheckNull(player);

	ACPlayerState* playerState = player->GetSelfPlayerState();
	CheckNull(playerState);

	int32 health = (int32)playerState->Health;
	int32 kill = (int32)playerState->Score;
	int32 death = (int32)playerState->Death;

	FString str;

	str = "Health : " + FString::FromInt(health);
	DrawText(str, FLinearColor::Red, 50, 50, Font);
	
	str = "Kill : " + FString::FromInt(kill);
	DrawText(str, FLinearColor::Red, 50, 70, Font);
	
	str = "Death : " + FString::FromInt(death);
	DrawText(str, FLinearColor::Red, 50, 90, Font);

	if (playerState->IsDead())
	{
		str = "You Dead, Noob";
		DrawText(str, FLinearColor::Red, Center.X - 300, Center.Y, Font, 5.f);
	}
}
