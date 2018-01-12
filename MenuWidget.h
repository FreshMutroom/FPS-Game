// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"

/**
 *	All menu widgets should inherit from this class
 */
UCLASS()
class FPS_PROJECT_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:

	/* A reference to a combo box if needed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UComboBoxString * ComboBox;
	
	/* A reference to an animation if needed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetAnimation * OnVisibleAnimation;

	/* A reference to an animation if needed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetAnimation * OnDisappearAnimation;

	/* A reference to a media player if needed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMediaPlayer * MoviePlayer;

	/* A reference to a media playlist if needed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UMediaPlaylist * Playlist;

	/* Make widget fade from black. Needed since sometimes animations will not work */
	UFUNCTION(BlueprintImplementableEvent)
	void FadeOut(float Duration);

	/* True if widget is fading out. Needed for the function above */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsFadingOut;

	/* A reference to a widget switcher if needed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UWidgetSwitcher * WidgetSwitcher;

	/* A reference to a text widget if needed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UTextBlock * TextWidget;

	/* A reference to an array of buttons if needed */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UButton *> Buttons;

};
