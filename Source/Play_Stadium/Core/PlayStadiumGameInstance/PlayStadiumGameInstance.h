#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UObject/ObjectPtr.h"

#include "ShuffleMode.h"
#include "Play_Stadium/Core/Questions/QuestionBase.h"

#include "PlayStadiumGameInstance.generated.h"

class FJsonObject;
class UDataTable;

UCLASS()
class PLAY_STADIUM_API UPlayStadiumGameInstance : public UGameInstance
{
        GENERATED_BODY()

public:
        virtual void Init() override;

        EShuffleMode GetQuestionsShuffleMode() const { return QuestionsShuffleMode; }
        const TArray<TObjectPtr<UQuestionBase>>& GetQuestions() const { return Questions; }

        UFUNCTION(BlueprintCallable, Category = "Questions")
        void HandleStartTestRequested();

protected:
        void LoadQuestionsFromJson();
        bool TryParseQuestionObject(const TSharedPtr<FJsonObject>& QuestionObject, TObjectPtr<UQuestionBase>& OutQuestion);
        void ApplyQuestionsShuffleMode();

        bool TryReadIntegerField(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName, int32& OutValue) const;
        bool TryReadStringField(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName, FString& OutValue) const;

protected:
        UPROPERTY(BlueprintReadOnly, Category = "Questions")
        EShuffleMode QuestionsShuffleMode = EShuffleMode::ShuffleNone;

        UPROPERTY(BlueprintReadOnly, Category = "Questions")
        TArray<TObjectPtr<UQuestionBase>> Questions;

        UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Questions")
        TMap<EQuestionType, TSoftObjectPtr<UDataTable>> MapsByType;

        UPROPERTY(BlueprintReadOnly, Category = "Questions")
        int32 NextQuestionIndex = 0;
};
