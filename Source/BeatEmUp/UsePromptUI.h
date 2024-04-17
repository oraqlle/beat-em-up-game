// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "UsePromptUI.generated.h"

/**
 * 
 */
UCLASS()
class BEATEMUP_API UUsePromptUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
    UTextBlock* PromptText;
};
