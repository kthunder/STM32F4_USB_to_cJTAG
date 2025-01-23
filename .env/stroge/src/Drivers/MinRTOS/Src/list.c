#include "list.h"

// 初始化链表节点
void vListInitListItem(ListItem_t *const pxItem)
{
    pxItem->pvContainer = NULL;
}
void vListInit(List_t *const pxList)
{
    pxList->pxIndex = (ListItem_t *)&(pxList->xListEnd);

    pxList->xListEnd.xItemValue = portMAX_DELAY;
    pxList->xListEnd.pxNext = (ListItem_t *)&(pxList->xListEnd);
    pxList->xListEnd.pxPrevious = (ListItem_t *)&(pxList->xListEnd);

    pxList->uxNumberOfItems = 0;
}

void vListInsertEnd(List_t *const pxList, ListItem_t *const pxNewListItem)
{
    ListItem_t *const pxIndex = pxList->pxIndex;

    pxNewListItem->pxNext = pxIndex;                 // new item 的后驱设置成 链表的节点索引;
    pxNewListItem->pxPrevious = pxIndex->pxPrevious; // new item 的前驱设置成 链表的节点索引的前驱;
    pxIndex->pxPrevious->pxNext = pxNewListItem;     // 将索引节点的前驱的后驱换成new item
    pxIndex->pxPrevious = pxNewListItem;             // 将索引节点的前驱换成new item，此时pre item->index->new item

    pxNewListItem->pvContainer = (void *)pxList; // 将new item的pvContainer指向链表

    (pxList->uxNumberOfItems)++; // 节点数加1
}

// 将节点按照升序插入链表
void vListInsert(List_t *const pxList, ListItem_t *const pxNewListItem)
{
    // 暂时不实现，借用vListInsertEnd
    vListInsertEnd(pxList, pxNewListItem);
}

uint32_t uxListRemove(ListItem_t *const pxItemToRemove)
{
    List_t *const pxList = (List_t *)pxItemToRemove->pvContainer;
    pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;
    pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;

    if (pxList->pxIndex == pxItemToRemove)
    {
        pxList->pxIndex = pxItemToRemove->pxPrevious;
    }
    pxItemToRemove->pvContainer = NULL;

    (pxList->uxNumberOfItems)--;

    return pxList->uxNumberOfItems;
}