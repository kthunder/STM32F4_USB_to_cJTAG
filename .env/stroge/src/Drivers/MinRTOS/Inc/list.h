#ifndef __LIST_H
#define __LIST_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "portmacro.h"

    typedef struct xLIST_ITEM
    {
        TickType_t xItemValue;         // 辅助值，用于排序
        struct xLIST_ITEM *pxNext;     // 后驱
        struct xLIST_ITEM *pxPrevious; // 前驱
        void *pvOwner;                 // 指向拥有节点的内核
        void *pvContainer;             // 指向节点所在的链表
    } ListItem_t;

    typedef struct xMINI_LIST_ITEM
    {
        TickType_t xItemValue;         // 辅助值，用于排序
        struct xLIST_ITEM *pxNext;     // 后驱
        struct xLIST_ITEM *pxPrevious; // 前驱
    } MiniListItem_t;

    typedef struct xLIST
    {
        uint32_t uxNumberOfItems; // 节点计数器
        ListItem_t *pxIndex;      // 节点索引指针
        //最后一个节点只是辅助记录用，不包含有效信息，不要使用
        MiniListItem_t xListEnd; // 最后一个节点
    } List_t;

    void vListInitListItem(ListItem_t *const pxItem);

    void vListInit(List_t *const pxList);

    void vListInsertEnd(List_t *const pxList, ListItem_t *const pxNewListItem);

    void vListInsert(List_t *const pxList, ListItem_t *const pxNewListItem);

    uint32_t uxListRemove(ListItem_t *const pxItemToRemove);

#ifdef __cplusplus
}
#endif

#endif