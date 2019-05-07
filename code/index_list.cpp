
struct index_list_element
{
    u16 Index;
    u16 Next;
};

struct index_list
{
    u16 ElementCount;
    index_list_element Elements[1];
};

static index_list*
IndexList_InitTemp(u16 Count)
{
    if (!Count) return 0;
    index_list *List = (index_list *)Api_Talloc(sizeof(index_list) + sizeof(index_list_element) * (Count - 1));
    for (u16 i = 0; i < Count; ++i)
        List->Elements[i] = { i, (u16)(i+1) };
    List->Elements[Count-1].Next = 0;
    List->ElementCount = Count;
    return List;
}

static index_list*
IndexList_CopyTemp(index_list *List)
{
    size Size = sizeof(index_list) + sizeof(index_list_element) * (List->ElementCount - 1);
    index_list *New = (index_list *)Api_Talloc(Size);
    memcpy(New, List, Size);
    return New;
}

static void
IndexList_Remove(index_list *List, u16 Index)
{
    List->Elements[Index] = List->Elements[List->Elements[Index].Next];
    --List->ElementCount;
}

static u16
IndexList_Next(index_list *List, u16 Index)
{
    return List->Elements[Index].Next;
}

static u16
IndexList_Get(index_list *List, u16 Index)
{
    return List->Elements[Index].Index;
}

static void
IndexList_Compress(index_list *List)
{
    u16 Index = 0;
    for (u16 i = 0; i < List->ElementCount; ++i)
    {
        if (List->Elements[Index].Next != (Index+1))
        {
            if (List->Elements[Index].Next == 0)
                return;
            List->Elements[Index+1] = List->Elements[List->Elements[Index].Next];
            List->Elements[Index].Next = Index+1;
        }
        Index = IndexList_Next(List, Index);
    }
}
