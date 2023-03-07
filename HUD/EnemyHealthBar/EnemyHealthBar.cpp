#include "EnemyHealthBar.h"
#include "Components/ProgressBar.h"

void UEnemyHealthBar::SetRatio(float Ratio) { HealthBar->SetPercent(Ratio); }

// 투명도를 활용해 위젯을 숨김
void UEnemyHealthBar::DisplayHealthBar(bool bDisplay) {
  HealthBar->SetFillColorAndOpacity(
      bDisplay ? BarColor : FLinearColor::Transparent);
  HealthBar->WidgetStyle.BackgroundImage.TintColor =
      bDisplay ? BgColor : FLinearColor::Transparent;
}