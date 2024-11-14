// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class CATCHCASH_API USessionSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

private:
	class UServerGameInstance* gameInstance;

public:
	UPROPERTY(meta = (BindWidget))
	class UButton* btn_RoomName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* text_RoomName;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* text_PlayerInfo;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* text_Ping;

	UPROPERTY()
	int32 index;


	UFUNCTION()
	void JoinRoom();

};
