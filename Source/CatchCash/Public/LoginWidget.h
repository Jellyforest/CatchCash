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

	UFUNCTION() //������ ����°� �ƴ����� �̸� �򰥸��� �ʵ���
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

	//������ �޾ƿ;��ϴϱ� ��������Ʈ �Ű������� ���� �޾ƿð��̴�.
	UFUNCTION()
	void AddNewSlot(FSessionInfo sessionInfo);

	//Ư���ϰ� ��� Ŭ�������� ������ �����ϴ�.
	class UServerGameInstance* gameInstance;

public:
	
	//��ư�� �ϴ� ������̰�
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
	
	//���� �����
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UEditableText* editText_RoomName;

	//����� ���� �� �ְ� �����Ұ����� �����̴��� ������ ���̴�
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class USlider* sl_PlayerCount;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* text_PlayerCount;

	//��ư�� ������ ���� ����������� �ϴ� ��ư
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UButton* btn_CreateSession;

//�̸��� ������ ���� ����� �������� �̵��ϰ� �� ���̴�
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UWidgetSwitcher* widgetSwitcher;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, meta = (BindWidget))
	class UScrollBox* sbox_RoomList;

	//�渮��Ʈ ����
	UPROPERTY(EditAnywhere)
	TSubclassOf<class USessionSlotWidget> sessionSlot;

	int32 indexNum;
};
