#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "QuestionMapListData.generated.h"


USTRUCT(BlueprintType)
struct FQuestionMapListData : public FTableRowBase
{
    GENERATED_BODY()


    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSoftObjectPtr<UWorld> Map;

};