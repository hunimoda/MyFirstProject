#pragma once

#include "CoreMinimal.h"
#include "Crosshair.generated.h"

/** 크로스헤어 텍스처와 크기를 담고 있는 구조체. */
USTRUCT()
struct FCrosshair {
  GENERATED_BODY()

public:
  /** 크로스헤어 텍스처. */
  UPROPERTY(EditAnywhere)
  class UTexture2D *Texture;
  /** 크로스헤어 크기(픽셀 단위). */
  UPROPERTY(EditAnywhere)
  float Size;
};