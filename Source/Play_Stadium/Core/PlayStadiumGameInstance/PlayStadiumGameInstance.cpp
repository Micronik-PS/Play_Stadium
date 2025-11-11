#include "PlayStadiumGameInstance.h"

#include "Algo/RandomShuffle.h"
#include "Algo/Sort.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"

#include "Play_Stadium/Core/Questions/MatchingQuestion/MatchingQuestion.h"
#include "Play_Stadium/Core/Questions/MatchingQuestion/MatchingQuestionData.h"
#include "Play_Stadium/Core/Questions/MultipleChoiceQuestion/MultipleChoiceQuestion.h"
#include "Play_Stadium/Core/Questions/MultipleChoiceQuestion/MultipleChoiceQuestionData.h"
#include "Play_Stadium/Core/Questions/SingleChoiceQuestion/SingleChoiceQuestion.h"
#include "Play_Stadium/Core/Questions/SingleChoiceQuestion/SingleChoiceQuestionData.h"
#include "Play_Stadium/Core/Questions/TextInputQuestion/TextInputQuestion.h"
#include "Play_Stadium/Core/Questions/TextInputQuestion/TextInputQuestionData.h"
#include "Play_Stadium/Core/Data/QuestionMapListData/QuestionMapListData.h"


namespace
{
	constexpr TCHAR QuestionsDefinitionFileName[] = TEXT("Questions.json");
}


DEFINE_LOG_CATEGORY_STATIC(LogPlayStadiumGameInstance, Log, All);


void UPlayStadiumGameInstance::Init()
{
        Super::Init();

        LoadQuestionsFromJson();
        ApplyQuestionsShuffleMode();
}

void UPlayStadiumGameInstance::HandleStartTestRequested()
{
        if (Questions.IsEmpty())
        {
                UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Cannot start test: questions list is empty."));
                return;
        }

        if (NextQuestionIndex >= Questions.Num())
        {
                UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Cannot start test: all questions have already been used."));
                return;
        }

        const TObjectPtr<UQuestionBase> CurrentQuestion = Questions[NextQuestionIndex];
        ++NextQuestionIndex;

        if (!CurrentQuestion)
        {
                UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Cannot start test: selected question is invalid."));
                return;
        }

        const EQuestionType QuestionType = CurrentQuestion->GetType();

        const TSoftObjectPtr<UDataTable>* const DataTableEntry = MapsByType.Find(QuestionType);
        if (!DataTableEntry)
        {
                UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("No data table configured for question type %d."), static_cast<int32>(QuestionType));
                return;
        }

        UDataTable* const QuestionMapTable = DataTableEntry->LoadSynchronous();
        if (!QuestionMapTable)
        {
                UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Failed to load data table for question type %d."), static_cast<int32>(QuestionType));
                return;
        }

        const TArray<FName> RowNames = QuestionMapTable->GetRowNames();
        if (RowNames.IsEmpty())
        {
                UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Data table for question type %d has no rows."), static_cast<int32>(QuestionType));
                return;
        }

        const int32 RandomRowIndex = FMath::RandRange(0, RowNames.Num() - 1);
        const FName& SelectedRowName = RowNames[RandomRowIndex];

        const FQuestionMapListData* const RowData = QuestionMapTable->FindRow<FQuestionMapListData>(SelectedRowName, TEXT("HandleStartTestRequested"));
        if (!RowData)
        {
                UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Failed to find row '%s' in question map data table."), *SelectedRowName.ToString());
                return;
        }

        TSoftObjectPtr<UWorld> SelectedMap = RowData->Map;
        if (!SelectedMap.IsValid())
        {
                SelectedMap.LoadSynchronous();
        }

        if (!SelectedMap.IsValid())
        {
                UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Failed to load map for row '%s'."), *SelectedRowName.ToString());
                return;
        }

        UGameplayStatics::OpenLevelBySoftObjectPtr(this, SelectedMap);
}


void UPlayStadiumGameInstance::LoadQuestionsFromJson()
{
        Questions.Reset();
        NextQuestionIndex = 0;

        const FString QuestionsFilePath = FPaths::Combine(FPaths::ProjectConfigDir(), QuestionsDefinitionFileName);

	if (!FPaths::FileExists(QuestionsFilePath))
	{
		UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Questions definition file was not found at path: %s"), *QuestionsFilePath);
		return;
	}

	FString FileContent;
	if (!FFileHelper::LoadFileToString(FileContent, *QuestionsFilePath))
	{
		UE_LOG(LogPlayStadiumGameInstance, Error, TEXT("Failed to read questions definition file: %s"), *QuestionsFilePath);
		return;
	}

	TSharedPtr<FJsonObject> RootObject;
	const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(FileContent);
	if (!FJsonSerializer::Deserialize(JsonReader, RootObject) || !RootObject.IsValid())
	{
		UE_LOG(LogPlayStadiumGameInstance, Error, TEXT("Failed to parse questions definition JSON: %s"), *QuestionsFilePath);
		return;
	}

	int32 ShuffleModeValue = static_cast<int32>(EShuffleMode::ShuffleNone);
	if (TryReadIntegerField(RootObject, TEXT("QuestionsShuffleMode"), ShuffleModeValue))
	{
		if (ShuffleModeValue >= static_cast<int32>(EShuffleMode::ShuffleNone) &&
		ShuffleModeValue <= static_cast<int32>(EShuffleMode::ShuffleTypeSort))
		{
			QuestionsShuffleMode = static_cast<EShuffleMode>(ShuffleModeValue);
		}
		else
		{
			UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Shuffle mode value %d is out of range. Using default value."), ShuffleModeValue);
			QuestionsShuffleMode = EShuffleMode::ShuffleNone;
		}
	}
	else
	{
		UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("QuestionsShuffleMode field is missing in %s. Using default value."), *QuestionsFilePath);
		QuestionsShuffleMode = EShuffleMode::ShuffleNone;
	}

	const TArray<TSharedPtr<FJsonValue>>* QuestionsArray = nullptr;
	if (!RootObject->TryGetArrayField(TEXT("Questions"), QuestionsArray) || QuestionsArray == nullptr)
	{
		UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Questions array is missing in %s."), *QuestionsFilePath);
		return;
	}

	for (int32 QuestionIndex = 0; QuestionIndex < QuestionsArray->Num(); ++QuestionIndex)
	{
		const TSharedPtr<FJsonValue>& QuestionValue = (*QuestionsArray)[QuestionIndex];
		const TSharedPtr<FJsonObject> QuestionObject = QuestionValue->AsObject();
		if (!QuestionObject.IsValid())
		{
			UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Question entry at index %d is not a valid JSON object."), QuestionIndex);
			continue;
		}

		TObjectPtr<UQuestionBase> ParsedQuestion;
		if (TryParseQuestionObject(QuestionObject, ParsedQuestion) && ParsedQuestion)
		{
			Questions.Add(ParsedQuestion);
		}
		else
		{
			UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Failed to parse question at index %d."), QuestionIndex);
		}
	}
}


void UPlayStadiumGameInstance::ApplyQuestionsShuffleMode()
{
	switch (QuestionsShuffleMode)
	{
	case EShuffleMode::ShuffleNone:
		return;

	case EShuffleMode::ShuffleRandom:
		Algo::RandomShuffle(Questions);
		return;

	case EShuffleMode::ShuffleTypeSort:
		{
			const auto GetQuestionTypeValue = [](const TObjectPtr<UQuestionBase>& Question) -> int32
			{
				if (!Question)
				{
					return TNumericLimits<int32>::Max();
				}

				return static_cast<int32>(Question->GetType());
			};

			Algo::Sort(Questions, [&GetQuestionTypeValue](const TObjectPtr<UQuestionBase>& Left, const TObjectPtr<UQuestionBase>& Right)
			{
				return GetQuestionTypeValue(Left) < GetQuestionTypeValue(Right);
			});
		}
		return;
	}
}


bool UPlayStadiumGameInstance::TryParseQuestionObject(const TSharedPtr<FJsonObject>& QuestionObject, TObjectPtr<UQuestionBase>& OutQuestion)
{
	OutQuestion = nullptr;

	if (!QuestionObject.IsValid())
	{
		return false;
	}

	int32 Reward = 0;
	if (!TryReadIntegerField(QuestionObject, TEXT("Reward"), Reward))
	{
		UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Question is missing required field Reward."));
		return false;
	}

	int32 TimeLimitInSeconds = 0;
	if (!TryReadIntegerField(QuestionObject, TEXT("TimeLimitInSeconds"), TimeLimitInSeconds))
	{
		UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Question is missing required field TimeLimitInSeconds."));
		return false;
	}

	int32 TypeValue = 0;
	if (!TryReadIntegerField(QuestionObject, TEXT("Type"), TypeValue))
	{
		UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Question is missing required field Type."));
		return false;
	}

	if (TypeValue < static_cast<int32>(EQuestionType::SingleChoiceQuestion) ||
	TypeValue > static_cast<int32>(EQuestionType::MatchingQuestion))
	{
		UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Question has invalid Type value %d."), TypeValue);
		return false;
	}

	const EQuestionType ParsedType = static_cast<EQuestionType>(TypeValue);

	switch (ParsedType)
	{
	case EQuestionType::SingleChoiceQuestion:
		{
			USingleChoiceQuestion* Question = NewObject<USingleChoiceQuestion>(this);
			if (!Question)
			{
				return false;
			}

			FSingleChoiceQuestionData QuestionData;
			QuestionData.SetReward(Reward);
			QuestionData.SetTimeLimitInSeconds(TimeLimitInSeconds);
			QuestionData.SetType(ParsedType);

			FString QuestionText;
			if (TryReadStringField(QuestionObject, TEXT("QuestionText"), QuestionText))
			{
				QuestionData.QuestionText = QuestionText;
			}
			else
			{
				UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Single choice question is missing QuestionText field."));
			}

			const TSharedPtr<FJsonObject>* ChoicesObject = nullptr;
			if (QuestionObject->TryGetObjectField(TEXT("Choices"), ChoicesObject) && ChoicesObject && ChoicesObject->IsValid())
			{
				for (const TPair<FString, TSharedPtr<FJsonValue>>& ChoicePair : (*ChoicesObject)->Values)
				{
					if (!ChoicePair.Value.IsValid())
					{
						UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Single choice question has invalid choice value for key '%s'."), *ChoicePair.Key);
						continue;
					}

					QuestionData.Choices.Add(ChoicePair.Key, ChoicePair.Value->AsBool());
				}
			}
			else
			{
				UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Single choice question is missing Choices object."));
			}

			Question->InitializeQuestionData(QuestionData);
			OutQuestion = Question;
			return true;
		}

	case EQuestionType::MultipleChoiceQuestion:
		{
			UMultipleChoiceQuestion* Question = NewObject<UMultipleChoiceQuestion>(this);
			if (!Question)
			{
				return false;
			}

			FMultipleChoiceQuestionData QuestionData;
			QuestionData.SetReward(Reward);
			QuestionData.SetTimeLimitInSeconds(TimeLimitInSeconds);
			QuestionData.SetType(ParsedType);

			FString QuestionText;
			if (TryReadStringField(QuestionObject, TEXT("QuestionText"), QuestionText))
			{
				QuestionData.QuestionText = QuestionText;
			}
			else
			{
				UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Multiple choice question is missing QuestionText field."));
			}

			const TSharedPtr<FJsonObject>* ChoicesObject = nullptr;
			if (QuestionObject->TryGetObjectField(TEXT("Choices"), ChoicesObject) && ChoicesObject && ChoicesObject->IsValid())
			{
				for (const TPair<FString, TSharedPtr<FJsonValue>>& ChoicePair : (*ChoicesObject)->Values)
				{
					if (!ChoicePair.Value.IsValid())
					{
						UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Multiple choice question has invalid choice value for key '%s'."), *ChoicePair.Key);
						continue;
					}

					QuestionData.Choices.Add(ChoicePair.Key, ChoicePair.Value->AsBool());
				}
			}
			else
			{
				UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Multiple choice question is missing Choices object."));
			}

			Question->InitializeQuestionData(QuestionData);
			OutQuestion = Question;
			return true;
		}

	case EQuestionType::TextInputQuestion:
		{
			UTextInputQuestion* Question = NewObject<UTextInputQuestion>(this);
			if (!Question)
			{
				return false;
			}

			FTextInputQuestionData QuestionData;
			QuestionData.SetReward(Reward);
			QuestionData.SetTimeLimitInSeconds(TimeLimitInSeconds);
			QuestionData.SetType(ParsedType);

			FString QuestionText;
			if (TryReadStringField(QuestionObject, TEXT("QuestionText"), QuestionText))
			{
				QuestionData.QuestionText = QuestionText;
			}
			else
			{
				UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Text input question is missing QuestionText field."));
			}

			FString CorrectAnswer;
			if (TryReadStringField(QuestionObject, TEXT("CorrectAnswer"), CorrectAnswer))
			{
				QuestionData.CorrectAnswer = CorrectAnswer;
			}
			else
			{
				UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Text input question is missing CorrectAnswer field."));
			}

			Question->InitializeQuestionData(QuestionData);
			OutQuestion = Question;
			return true;
		}

	case EQuestionType::MatchingQuestion:
		{
			UMatchingQuestion* Question = NewObject<UMatchingQuestion>(this);
			if (!Question)
			{
				return false;
			}

			FMatchingQuestionData QuestionData;
			QuestionData.SetReward(Reward);
			QuestionData.SetTimeLimitInSeconds(TimeLimitInSeconds);
			QuestionData.SetType(ParsedType);

			const TSharedPtr<FJsonObject>* PairsObject = nullptr;
			if (QuestionObject->TryGetObjectField(TEXT("Pairs"), PairsObject) && PairsObject && PairsObject->IsValid())
			{
				for (const TPair<FString, TSharedPtr<FJsonValue>>& Pair : (*PairsObject)->Values)
				{
					if (!Pair.Value.IsValid())
					{
						UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Matching question has invalid pair value for key '%s'."), *Pair.Key);
						continue;
					}

					QuestionData.Pairs.Add(Pair.Key, Pair.Value->AsString());
				}
			}
			else
			{
				UE_LOG(LogPlayStadiumGameInstance, Warning, TEXT("Matching question is missing Pairs object."));
			}

			Question->InitializeQuestionData(QuestionData);
			OutQuestion = Question;
			return true;
		}

	default:
		break;
	}

	return false;
}


bool UPlayStadiumGameInstance::TryReadIntegerField(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName, int32& OutValue) const
{
	if (!JsonObject.IsValid())
	{
		return false;
	}

	double NumericValue = 0.0;
	if (!JsonObject->TryGetNumberField(FieldName, NumericValue))
	{
		return false;
	}

	OutValue = static_cast<int32>(NumericValue);
	return true;
}


bool UPlayStadiumGameInstance::TryReadStringField(const TSharedPtr<FJsonObject>& JsonObject, const FString& FieldName, FString& OutValue) const
{
	if (!JsonObject.IsValid())
	{
		return false;
	}

	return JsonObject->TryGetStringField(FieldName, OutValue);
}
