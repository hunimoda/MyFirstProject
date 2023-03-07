#include "MySaveGame.h"
#include "Kismet/GameplayStatics.h"

// 정적 멤버 초기화
FString UMySaveGame::SlotName = TEXT("DefaultSlot");
uint32 UMySaveGame::UserIndex = 0;

bool UMySaveGame::HasSavedGame() {
  return UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex);
}

// 저장된 게임이 있으면 해당 정보로 채워진 로드 인스턴스를, 없다면 빈 인스턴스 반환
UMySaveGame *UMySaveGame::MakeLoadGameInstance() {
  if (USaveGame *LoadGameInstance =
          UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex)) {
    return Cast<UMySaveGame>(LoadGameInstance);
  }
  return MakeSaveGameInstance();
}

UMySaveGame *UMySaveGame::MakeSaveGameInstance() {
  USaveGame *SaveGameInstance =
      UGameplayStatics::CreateSaveGameObject(StaticClass());
  return Cast<UMySaveGame>(SaveGameInstance);
}

void UMySaveGame::SaveGame(UMySaveGame *SaveGameInstance) {
  UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, UserIndex);
}

void UMySaveGame::DeleteGame() {
  UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);
}