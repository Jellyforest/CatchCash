// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include "ServerGameInstance.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include "Components/WidgetSwitcher.h"
#include "Components/ScrollBox.h"
#include "SessionSlotWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//�÷��� �������ڸ��� editable text�� �Է°��� �ʱ�ȭ������ ���̴�.
	editText_id->SetText(FText::FromString(""));
	editText_RoomName->SetText(FText::FromString(""));
	sl_PlayerCount->SetValue(2.0f);
	text_PlayerCount->SetText(FText::FromString("2"));

	/*��ư Ŭ�� �̺�Ʈ�� �Լ� ���ε��ϱ�*/
	//ID �Է��ϰ� �������� �Ѿ�� ��
	btn_Start->OnClicked.AddDynamic(this, &ULoginWidget::ClickStart);
	//�α��� �ؼ� ���� ��
	btn_CreateSession->OnClicked.AddDynamic(this, &ULoginWidget::CreateServer);
	// ��Ƽ�÷���
	btn_Multi->OnClicked.AddDynamic(this, &ULoginWidget::ClickMulti);

	btn_CreateRoom->OnClicked.AddDynamic(this, &ULoginWidget::CreateRoom);

	//��������
	btn_JoinRoom->OnClicked.AddDynamic(this, &ULoginWidget::GoFind);
	btn_Refresh->OnClicked.AddDynamic(this, &ULoginWidget::RefreshList);

	//�����̴��� ������ ������ ���ڵ� �������ֱ�
	sl_PlayerCount->OnValueChanged.AddDynamic(this, &ULoginWidget::OnMoveSlider);
	//�ڷΰ���
	btn_Back_0->OnClicked.AddDynamic(this, &ULoginWidget::ClickBack);
	btn_Back_1->OnClicked.AddDynamic(this, &ULoginWidget::ClickBack);
	btn_Back_3->OnClicked.AddDynamic(this, &ULoginWidget::ClickBack);

	gameInstance = Cast<UServerGameInstance>(GetGameInstance());
	//���� �ν��Ͻ� ������ ���� �̷��� �������� �ȴ�.

	if (gameInstance != nullptr)
	{
		//�α��� �������� ��ũ�ѹٿ� ������ ���� ���ʷ� ��� �߰��� ���̴�.
		//���� �̸��̳� ������ ���� �ν��Ͻ� ��������Ʈ���� ���� ���̰�, ���⼭ �� ���� ������ �߰��� ���̴�.
		gameInstance->searchResultDele.AddDynamic(this, &ULoginWidget::AddNewSlot);
		gameInstance->serachFinishedDelegate.AddDynamic(this, &ULoginWidget::RefreshEnabled);
	}

	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	playerController->SetShowMouseCursor(true);
}



void ULoginWidget::ClickStart()
{

	//������ �� ���̵� �ȴ��� ���� �ִ�. ���� ������ �������, ���� ���� ���� �������� �Ѿ���� ����������Ѵ�
	//����, ID�� �Է��Ͽ��ٸ�(��ĭ�� �ƴ϶��) 1�� ĵ������ �����Ѵ�.(0�� �ε����� ���� ĵ���� 1�� �ε����� Create Session ĵ����)
	if (!(editText_id->GetText().IsEmpty()))
	{
		widgetSwitcher->SetActiveWidgetIndex(1);
		//���� ���� �ν��Ͻ��� �޾ƿ� ���̴�
		//ĳ���� �迭�� ���� �� �ۿ� ���� ������ String���� ���ͼ� Text�� ��ȯ���־���Ѵ�
		gameInstance->sessionID = FName(*editText_id->GetText().ToString());
	}
}

void ULoginWidget::CreateServer()
{
	//�ۼ��� �� �̸��� �÷��̾� ī��Ʈ�� ���� �����.
							//�ݿø�
	int32 playerCnt = FMath::RoundHalfFromZero(sl_PlayerCount->GetValue());

	gameInstance->CreateMySession(editText_RoomName->GetText().ToString(), sl_PlayerCount->GetValue());

}

//�����̴� ���� ����� �� ȣ��Ǵ� �Լ�. �ʱⰪ�� �ʱ�ȭ������ϹǷ� Native Constructor���� �ʱ�ȭ���ش�.
void ULoginWidget::OnMoveSlider(float value)
{
	//����� �� �ؽ�Ʈ�� ���� �Է����ָ� �ȴ�.
	//sl_playerCount->GetValue() : ���� ������ �ҷ����°�. �ٵ� ���ڷ� float value�� �޾ƿ��� ������ value�� ���� �����ش�.
	FString num2str = FString::Printf(TEXT("%d"), value);  //�����̴��� ���� ���ڿ��� ��ȯ�ϰ�
	text_PlayerCount->SetText(FText::FromString(*num2str)); //�̰� �ٽ� �ؽ�Ʈ�� ��ȯ���־���.
}

void ULoginWidget::ClickMulti()
{
	widgetSwitcher->SetActiveWidgetIndex(1);

}

void ULoginWidget::ClickBack()
{
	//indexNum = widgetSwitcher->GetActiveWidgetIndex() - 1;
	widgetSwitcher->SetActiveWidgetIndex(0);

}

void ULoginWidget::CreateRoom()
{
	widgetSwitcher->SetActiveWidgetIndex(2);

}

void ULoginWidget::GoFind()
{
	widgetSwitcher->SetActiveWidgetIndex(3);

	RefreshList();
}

void ULoginWidget::RefreshList()
{
	//Add Slot�� �ϰ� ������ �ִ� childern���� ���� �����ش�.
	sbox_RoomList->ClearChildren();

	//������ �˻��϶�� ��������Ʈ �������ش�.
	gameInstance->FindMySession();

	//�������� ��ư�� ������ ��ư�� ���� �� ���� ��Ȱ��ȭ�����ش�
	btn_Refresh->SetIsEnabled(false);
}
void ULoginWidget::RefreshEnabled()
{
	btn_Refresh->SetIsEnabled(true);
}

//���� �ν��Ͻ��κ��� �˻��Ϸ� �̺�Ʈ�� �޾��� �� ����� �Լ��̴�.
//������ ��������Ʈ ������ ������ �Ű������� ��ġ�ؾ��Ѵ�.
//������ �����ϰ�, �Ѱܹ��� �������� ���Կ� ä���ȴ�. �� ������ ��ũ�ѹ��� �ڽ����� ����Ѵ�.
//�׷��� �ϳ��� �߰��Ǵ� ���̴�
void ULoginWidget::AddNewSlot(FSessionInfo sessionInfo)
{
	USessionSlotWidget* slotWidget = CreateWidget<USessionSlotWidget>(this, sessionSlot);
	if (slotWidget != nullptr)
	{
		slotWidget->text_RoomName->SetText(FText::FromString(sessionInfo.roomName));
		slotWidget->text_PlayerInfo->SetText(FText::FromString(FString::Printf(TEXT(" % d / % d"), sessionInfo.currentPlayers, sessionInfo.maxPlayers)));
		slotWidget->text_Ping->SetText(FText::FromString(FString::Printf(TEXT("%d ms"), sessionInfo.ping)));
		slotWidget->index = sessionInfo.idx;

		//�� ����Ʈ ������ŭ ���������� �þ�� ��
		sbox_RoomList->AddChild(slotWidget);
	}
}
