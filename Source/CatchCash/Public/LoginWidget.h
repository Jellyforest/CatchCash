// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class CATCHCASH_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()

		
protected:
	virtual void NativeConstruct() override;
	
private:
	UFUNCTION()
	void ClickStart();

	UFUNCTION() //서버를 만드는건 아니지만 이름 헷갈리지 않도록
	void CreateServer();

	UFUNCTION()
	void OnMoveSlider(float value);
	
	UFUNCTION()
	void ClickMulti();
	UFUNCTION()
	void ClickBack();
	UFUNCTION()
	void CreateRoom();

	UFUNCTION()
	void GoFind();

	UFUNCTION()
	void RefreshList();
	UFUNCTION()
	void RefreshEnabled();

	//정보를 받아와야하니까 델리게이트 매개변수도 같이 받아올것이다.
	UFUNCTION()
	void AddNewSlot(FSessionInfo sessionInfo);

	//특이하게 모든 클래스에서 접근이 가능하다.
	class UServerGameInstance* gameInstance;

public:
	
	//버튼을 일단 만들것이고
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UEditableText* editText_id;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UButton* btn_Start;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UButton* btn_Multi;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UButton* btn_CreateRoom;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UButton* btn_JoinRoom;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UButton* btn_Refresh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UButton* btn_Back_0;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UButton* btn_Back_1;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UButton* btn_Back_3;
	
	//방제 만들기
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UEditableText* editText_RoomName;

	//몇명이 들어올 수 있게 설정할것인지 슬라이더로 정해줄 것이다
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class USlider* sl_PlayerCount;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* text_PlayerCount;

	//버튼을 누르면 방이 만들어지도록 하는 버튼
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UButton* btn_CreateSession;

//이름을 적으면 방을 만드는 공간으로 이동하게 할 것이다
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UWidgetSwitcher* widgetSwitcher;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UScrollBox* sbox_RoomList;

	//방리스트 슬롯
	UPROPERTY(EditAnywhere)
	TSubclassOf<class USessionSlotWidget> sessionSlot;

	int32 indexNum;
};
