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

	//플레이 시작하자마자 editable text의 입력값을 초기화시켜줄 것이다.
	editText_id->SetText(FText::FromString(""));
	editText_RoomName->SetText(FText::FromString(""));
	sl_PlayerCount->SetValue(2.0f);
	text_PlayerCount->SetText(FText::FromString("2"));

	/*버튼 클릭 이벤트에 함수 바인딩하기*/
	//ID 입력하고 다음으로 넘어가는 것
	btn_Start->OnClicked.AddDynamic(this, &ULoginWidget::ClickStart);
	//로그인 해서 들어가는 것
	btn_CreateSession->OnClicked.AddDynamic(this, &ULoginWidget::CreateServer);
	// 멀티플레이
	btn_Multi->OnClicked.AddDynamic(this, &ULoginWidget::ClickMulti);

	btn_CreateRoom->OnClicked.AddDynamic(this, &ULoginWidget::CreateRoom);

	//게임조인
	btn_JoinRoom->OnClicked.AddDynamic(this, &ULoginWidget::GoFind);
	btn_Refresh->OnClicked.AddDynamic(this, &ULoginWidget::RefreshList);

	//슬라이더를 움직일 때마다 숫자도 변경해주기
	sl_PlayerCount->OnValueChanged.AddDynamic(this, &ULoginWidget::OnMoveSlider);
	//뒤로가기
	btn_Back_0->OnClicked.AddDynamic(this, &ULoginWidget::ClickBack);
	btn_Back_1->OnClicked.AddDynamic(this, &ULoginWidget::ClickBack);
	btn_Back_3->OnClicked.AddDynamic(this, &ULoginWidget::ClickBack);

	gameInstance = Cast<UServerGameInstance>(GetGameInstance());
	//게임 인스턴스 가져올 때는 이렇게 가져오면 된다.

	if (gameInstance != nullptr)
	{
		//로그인 위젯에서 스크롤바에 슬롯을 만들어서 차례로 계속 추가할 것이다.
		//방의 이름이나 정보는 게임 인스턴스 델리게이트에서 받을 것이고, 여기서 그 받은 정보를 추가할 것이다.
		gameInstance->searchResultDele.AddDynamic(this, &ULoginWidget::AddNewSlot);
		gameInstance->serachFinishedDelegate.AddDynamic(this, &ULoginWidget::RefreshEnabled);
	}

	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	playerController->SetShowMouseCursor(true);
}



void ULoginWidget::ClickStart()
{

	//눌렀을 때 아이디를 안누를 수도 있다. 뭐라도 누르게 만들려면, 값이 있을 때만 다음으로 넘어가도록 설정해줘야한다
	//만일, ID를 입력하였다면(빈칸이 아니라면) 1번 캔버스로 변경한다.(0번 인덱스는 현재 캔버스 1번 인덱스는 Create Session 캔버스)
	if (!(editText_id->GetText().IsEmpty()))
	{
		widgetSwitcher->SetActiveWidgetIndex(1);
		//서버 게임 인스턴스를 받아올 것이다
		//캐릭터 배열로 받을 수 밖에 없기 때문에 String으로 들고와서 Text로 변환해주어야한다
		gameInstance->sessionID = FName(*editText_id->GetText().ToString());
	}
}

void ULoginWidget::CreateServer()
{
	//작성된 방 이름과 플레이어 카운트로 방을 만든다.
							//반올림
	int32 playerCnt = FMath::RoundHalfFromZero(sl_PlayerCount->GetValue());

	gameInstance->CreateMySession(editText_RoomName->GetText().ToString(), sl_PlayerCount->GetValue());

}

//슬라이더 값이 변경될 때 호출되는 함수. 초기값을 초기화해줘야하므로 Native Constructor에서 초기화해준다.
void ULoginWidget::OnMoveSlider(float value)
{
	//변경될 때 텍스트에 값을 입력해주면 된다.
	//sl_playerCount->GetValue() : 숫자 정보를 불러오는것. 근데 인자로 float value를 받아오기 때문에 value로 값을 적어준다.
	FString num2str = FString::Printf(TEXT("%d"), value);  //슬라이더의 값을 문자열로 변환하고
	text_PlayerCount->SetText(FText::FromString(*num2str)); //이걸 다시 텍스트로 변환해주었다.
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
	//Add Slot을 하고 기존에 있는 childern들을 전부 지워준다.
	sbox_RoomList->ClearChildren();

	//세션을 검색하라고 델리게이트 실행해준다.
	gameInstance->FindMySession();

	//리프레시 버튼을 누르면 버튼을 누를 수 없게 비활성화시켜준다
	btn_Refresh->SetIsEnabled(false);
}
void ULoginWidget::RefreshEnabled()
{
	btn_Refresh->SetIsEnabled(true);
}

//게임 인스턴스로부터 검색완료 이벤트를 받았을 때 실행될 함수이다.
//만들어둔 델리게이트 변수와 여기의 매개변수가 일치해야한다.
//슬롯을 생성하고, 넘겨받은 정보들을 슬롯에 채우면된다. 그 슬롯을 스크롤바의 자식으로 등록한다.
//그렇게 하나씩 추가되는 것이다
void ULoginWidget::AddNewSlot(FSessionInfo sessionInfo)
{
	USessionSlotWidget* slotWidget = CreateWidget<USessionSlotWidget>(this, sessionSlot);
	if (slotWidget != nullptr)
	{
		slotWidget->text_RoomName->SetText(FText::FromString(sessionInfo.roomName));
		slotWidget->text_PlayerInfo->SetText(FText::FromString(FString::Printf(TEXT(" % d / % d"), sessionInfo.currentPlayers, sessionInfo.maxPlayers)));
		slotWidget->text_Ping->SetText(FText::FromString(FString::Printf(TEXT("%d ms"), sessionInfo.ping)));
		slotWidget->index = sessionInfo.idx;

		//방 리스트 받은만큼 슬롯위젯이 늘어나는 것
		sbox_RoomList->AddChild(slotWidget);
	}
}
