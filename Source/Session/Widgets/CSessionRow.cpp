#include "CSessionRow.h"
#include "Global.h"
#include "Components/Button.h"
#include "CMainMenu.h"

void UCSessionRow::Setup(UCMainMenu* InParent, uint32 InIndex)
{
	Parent = InParent;
	Index = InIndex;

	CheckNull(RowButton);
	RowButton->OnClicked.AddDynamic(this, &UCSessionRow::OnClicked);
}

void UCSessionRow::OnClicked()
{
	CheckNull(Parent);

	Parent->SetSessionRowIndex(Index);
}