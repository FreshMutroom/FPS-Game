// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "Ver2_MouseoverDisplay.generated.h"

/**
 *  This class is needed because getting a reference to the Ver2_Bot that owns a mouseover
 *  widget from blueprints is impossible. I've added more fields to it and it is also the
 *	base class for other widgets too
 */
UCLASS()
class FPS_PROJECT_API UVer2_MouseoverDisplay : public UUserWidget
{
	GENERATED_BODY()
	
	
public:

	// A reference to the bot that owns this widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class AVer2_Bot* OwningPawn;
	
	// A reference to a widget animation. This is here for the low health for the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetAnimation* Animation;

	// A reference to another widget animation
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetAnimation* Animation2;

	// A reference to the player. Used by mouse sensitivity menu
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AFPS_ProjectCharacter* Player;

	// A reference to an advanced bot if needed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AAdvanced_Bot* OwningAdvancedBot;

	// A reference to an AIPatrol if needed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AAIPatrol* OwningAIPatrolBot;

};
