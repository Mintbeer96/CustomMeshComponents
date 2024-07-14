// Fill out your copyright notice in the Description page of Project Settings.


#include "LayerWeightEdGraphNode_Base.h"

#include "EdGraphSchema_LayerWeightEditor.h"

void ULayerWeightEdGraphNode_Base::PostLoad()
{
	Super::PostLoad();

	if (GIsEditor && HasAllFlags(RF_Transactional) == false)
	{
		SetFlags(RF_Transactional);
	}
}

void ULayerWeightEdGraphNode_Base::AutowireNewNode(UEdGraphPin* FromPin)
{
	Super::AutowireNewNode(FromPin);
	if (FromPin != nullptr)
	{
		const UEdGraphSchema_LayerWeightEditor* Schema = CastChecked<UEdGraphSchema_LayerWeightEditor>(GetSchema());
		check(Schema);
		
		//ENiagaraCompoundType FromType = Schema->GetPinDataType(FromPin);

		//Find first of this nodes pins with the right type and direction.
		UEdGraphPin* FirstPinOfSameType = NULL;
		EEdGraphPinDirection DesiredDirection = FromPin->Direction == EGPD_Output ? EGPD_Input : EGPD_Output;
		for (UEdGraphPin* Pin : Pins)
		{
			//ENiagaraCompoundType ToType = Schema->GetPinDataType(Pin);
			if (Pin->Direction == DesiredDirection)
			{
				const FPinConnectionResponse Response = Schema->CanCreateConnection(FromPin, Pin);
				if (Response.Response != ECanCreateConnectionResponse::CONNECT_RESPONSE_DISALLOW)
				{
					FirstPinOfSameType = Pin;
					break;
				}
			}			
		}

		if (FirstPinOfSameType && GetSchema()->TryCreateConnection(FromPin, FirstPinOfSameType))
		{
			FromPin->GetOwningNode()->NodeConnectionListChanged();
		}
	}
}
