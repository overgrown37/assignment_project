#include "SSSEditorHelpers.h"

#include "Animation/Skeleton.h"
#include "Engine/SkeletalMeshSocket.h"

bool USSSEditorHelpers::AddSkeletonSocket(USkeleton* Skeleton, FName SocketName, FName BoneName, FVector RelativeLocation)
{
	if (!Skeleton || SocketName.IsNone())
	{
		return false;
	}

	if (USkeletalMeshSocket* Existing = Skeleton->FindSocket(SocketName))
	{
		Existing->BoneName = BoneName;
		Existing->RelativeLocation = RelativeLocation;
		Skeleton->MarkPackageDirty();
		return true;
	}

	USkeletalMeshSocket* Socket = NewObject<USkeletalMeshSocket>(Skeleton);
	Socket->SocketName = SocketName;
	Socket->BoneName = BoneName;
	Socket->RelativeLocation = RelativeLocation;
	Skeleton->Modify();
	Skeleton->Sockets.Add(Socket);
	Skeleton->MarkPackageDirty();
	return true;
}
