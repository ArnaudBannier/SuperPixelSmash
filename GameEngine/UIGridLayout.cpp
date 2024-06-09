/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#include "UIGridLayout.h"
#include "UICanvas.h"
#include "Utils.h"

UIGridLayout::UIGridLayout(Scene *scene, int rowCount, int columnCount) :
    UIObject(scene), m_rowCount(rowCount), m_colCount(columnCount),
    m_gridMap(), m_padding(b2Vec2_zero),
    m_rowSizes((size_t)rowCount, -1.f),
    m_colSizes((size_t)columnCount, -1.f),
    m_rowOffsets((size_t)rowCount + 1, 0.f),
    m_colOffsets((size_t)columnCount + 1, 0.f),
    m_rowSpacings((size_t)rowCount, 0.f),
    m_colSpacings((size_t)columnCount, 0.f),
    m_anchor(Anchor::NORTH_WEST),
    m_debugGizmos(true)
{
    SetName("UIGridLayout");
}

void UIGridLayout::AddObject(
    UIObject *object, int rowIdx, int columnIdx, int rowSpan, int columnSpan)
{
    if (rowIdx < 0 || rowIdx >= m_rowCount ||
        columnIdx < 0 || columnIdx >= m_colCount ||
        (object == nullptr))
    {
        assert(false);
        return;
    }

    rowSpan = Math::Clamp(rowSpan, 1, m_rowCount - rowIdx);
    columnSpan = Math::Clamp(columnSpan, 1, m_colCount - columnIdx);

    GridCell cell(rowIdx, columnIdx, rowSpan, columnSpan);

    std::pair<int, int> indices = std::make_pair(rowIdx, columnIdx);
    auto it = m_gridMap.find(object);
    if (it != m_gridMap.end())
    {
        it->second = cell;
    }
    else
    {
        m_gridMap.insert(std::make_pair(object, cell));
    }
    object->SetParent(this);
    object->SetUseParentAnimation(true);
    object->SetUseParentAlpha(true);
    Update();
}

void UIGridLayout::RemoveObject(UIObject *object)
{
    auto it = m_gridMap.find(object);
    if (it == m_gridMap.end())
    {
        return;
    }

    object->SetParent(m_scene->GetCanvas());
    object->SetUseParentAnimation(false);

    m_gridMap.erase(it);
}

void UIGridLayout::SetRowSize(int index, float size)
{
    if (index < 0 || index >= m_rowCount)
    {
        assert(false);
        return;
    }
    m_rowSizes[index] = size;
    Update();
}

void UIGridLayout::SetRowSize(float size)
{
    for (float &s : m_rowSizes)
    {
        s = size;
    }
    Update();
}

void UIGridLayout::SetColumnSize(int index, float size)
{
    if (index < 0 || index >= m_colCount)
    {
        assert(false);
        return;
    }
    m_colSizes[index] = size;
    Update();
}

void UIGridLayout::SetColumnSize(float size)
{
    for (float &s : m_colSizes)
    {
        s = size;
    }
    Update();
}

void UIGridLayout::SetRowSpacing(int index, float spacing)
{
    if (index < 0 || index >= m_rowCount - 1)
    {
        assert(false);
        return;
    }
    m_rowSpacings[index] = spacing;
    Update();
}

void UIGridLayout::SetRowSpacing(float spacing)
{
    for (float &s : m_rowSpacings)
    {
        s = spacing;
    }
    m_rowSpacings.back() = 0.f;
    Update();
}

void UIGridLayout::SetColumnSpacing(int index, float spacing)
{
    if (index < 0 || index >= m_colCount - 1)
    {
        assert(false);
        return;
    }
    m_colSpacings[index] = spacing;
    Update();
}

void UIGridLayout::SetColumnSpacing(float spacing)
{
    for (float &s : m_colSpacings)
    {
        s = spacing;
    }
    m_colSpacings.back() = 0.f;
    Update();
}

void UIGridLayout::SetSpacing(float spacing)
{
    for (float &s : m_rowSpacings)
    {
        s = spacing;
    }
    for (float &s : m_colSpacings)
    {
        s = spacing;
    }
    m_rowSpacings.back() = 0.f;
    m_colSpacings.back() = 0.f;
    Update();
}

void UIGridLayout::Update()
{
    UIObject::Update();
    UpdateOffsets();

    if (m_debugGizmos) SetVisible(true);

    b2Vec2 gridDim(
        m_colOffsets[m_colCount],
        m_rowOffsets[m_rowCount]
    );

    for (auto it = m_gridMap.begin(); it != m_gridMap.end(); ++it)
    {
        UIObject *object = it->first;
        GridCell cell = it->second;

        object->GetLocalRect().offsetMin.x = +m_colOffsets[cell.colIdx];
        object->GetLocalRect().offsetMax.x = +m_colOffsets[cell.colIdx + cell.colSpan] - m_colSpacings[cell.colIdx + cell.colSpan - 1];
        object->GetLocalRect().offsetMin.y = -m_rowOffsets[cell.rowIdx + cell.rowSpan] + m_rowSpacings[cell.rowIdx + cell.rowSpan - 1];
        object->GetLocalRect().offsetMax.y = -m_rowOffsets[cell.rowIdx];

        switch (m_anchor)
        {
        default:
        case Anchor::NORTH_WEST:
        case Anchor::WEST:
        case Anchor::SOUTH_WEST:
            object->GetLocalRect().anchorMin.x = 0.f;
            object->GetLocalRect().anchorMax.x = 0.f;
            object->GetLocalRect().offsetMin.x += m_padding.x;
            object->GetLocalRect().offsetMax.x += m_padding.x;
            break;
        case Anchor::NORTH:
        case Anchor::CENTER:
        case Anchor::SOUTH:
            object->GetLocalRect().anchorMin.x = 0.5f;
            object->GetLocalRect().anchorMax.x = 0.5f;
            object->GetLocalRect().offsetMin.x -= 0.5f * gridDim.x;
            object->GetLocalRect().offsetMax.x -= 0.5f * gridDim.x;
            break;
        case Anchor::NORTH_EAST:
        case Anchor::EAST:
        case Anchor::SOUTH_EAST:
            object->GetLocalRect().anchorMin.x = 1.f;
            object->GetLocalRect().anchorMax.x = 1.f;
            object->GetLocalRect().offsetMin.x -= gridDim.x + m_padding.x;
            object->GetLocalRect().offsetMax.x -= gridDim.x + m_padding.x;
            break;
        }

        switch (m_anchor)
        {
        default:
        case Anchor::NORTH_WEST:
        case Anchor::NORTH:
        case Anchor::NORTH_EAST:
            object->GetLocalRect().anchorMin.y = 1.f;
            object->GetLocalRect().anchorMax.y = 1.f;
            object->GetLocalRect().offsetMin.y -= m_padding.y;
            object->GetLocalRect().offsetMax.y -= m_padding.y;
            break;
        case Anchor::WEST:
        case Anchor::CENTER:
        case Anchor::EAST:
            object->GetLocalRect().anchorMin.y = 0.5f;
            object->GetLocalRect().anchorMax.y = 0.5f;
            object->GetLocalRect().offsetMin.y += 0.5f * gridDim.y;
            object->GetLocalRect().offsetMax.y += 0.5f * gridDim.y;
            break;
        case Anchor::SOUTH_WEST:
        case Anchor::SOUTH:
        case Anchor::SOUTH_EAST:
            object->GetLocalRect().anchorMin.y = 0.f;
            object->GetLocalRect().anchorMax.y = 0.f;
            object->GetLocalRect().offsetMin.y += gridDim.y + m_padding.y;
            object->GetLocalRect().offsetMax.y += gridDim.y + m_padding.y;
            break;
        }
    }
}

void UIGridLayout::DrawGizmos(Gizmos &gizmos)
{
    if (m_debugGizmos == false) return;

    SDL_FRect gridRect = GetCanvasRect();
    float pixelsPerUnit = m_scene->GetUIPixelsPerUnit();
    b2Vec2 gridDim(
        m_colOffsets[m_colCount],
        m_rowOffsets[m_rowCount]
    );
    
    SDL_SetRenderDrawColor(g_renderer, 0, 255, 0, 128);
    for (int i = 0; i < m_rowCount; i++)
    {
        for (int j = 0; j < m_colCount; j++)
        {
            SDL_FRect rect;
            rect.x = gridRect.x + m_colOffsets[j] * pixelsPerUnit;
            rect.y = gridRect.y + m_rowOffsets[i] * pixelsPerUnit;
            rect.w = (m_colOffsets[j + 1] - m_colOffsets[j] - m_colSpacings[j]) * pixelsPerUnit;
            rect.h = (m_rowOffsets[i + 1] - m_rowOffsets[i] - m_rowSpacings[i]) * pixelsPerUnit;

            switch (m_anchor)
            {
            default:
            case Anchor::NORTH_WEST:
            case Anchor::WEST:
            case Anchor::SOUTH_WEST:
                rect.x += m_padding.x * pixelsPerUnit;
                break;
            case Anchor::NORTH:
            case Anchor::CENTER:
            case Anchor::SOUTH:
                rect.x += 0.5f * gridRect.w - 0.5f * gridDim.x * pixelsPerUnit;
                break;
            case Anchor::NORTH_EAST:
            case Anchor::EAST:
            case Anchor::SOUTH_EAST:
                rect.x += gridRect.w - (gridDim.x + m_padding.x) * pixelsPerUnit;
                break;
            }
            switch (m_anchor)
            {
            default:
            case Anchor::NORTH_WEST:
            case Anchor::NORTH:
            case Anchor::NORTH_EAST:
                rect.y += m_padding.y * pixelsPerUnit;
                break;
            case Anchor::WEST:
            case Anchor::CENTER:
            case Anchor::EAST:
                rect.y += 0.5f * gridRect.h - 0.5f * gridDim.y * pixelsPerUnit;
                break;
            case Anchor::SOUTH_WEST:
            case Anchor::SOUTH:
            case Anchor::SOUTH_EAST:
                rect.y += gridRect.h - (gridDim.y + m_padding.y) * pixelsPerUnit;
                break;
            }

            SDL_RenderDrawRectF(g_renderer, &rect);
        }
    }
}

void UIGridLayout::UpdateOffsets()
{
    b2Vec2 dimensions = GetRectSize();
    b2Vec2 spacing = b2Vec2_zero;
    for (float x : m_colSpacings) spacing.x += x;
    for (float y : m_rowSpacings) spacing.y += y;

    b2Vec2 fixedDim = 2 * m_padding + spacing;

    b2Vec2 stretchSum = b2Vec2_zero;
    for (float size : m_colSizes)
    {
        if (size > 0.f) fixedDim.x += size;
        else stretchSum.x += size;
    }
    for (float size : m_rowSizes)
    {
        if (size > 0.f) fixedDim.y += size;
        else stretchSum.y += size;
    }
    b2Vec2 stretchSize = dimensions - fixedDim;
    stretchSize.x /= stretchSum.x;
    stretchSize.y /= stretchSum.y;

    m_rowOffsets[0] = 0.f;
    for (size_t i = 0; i < m_rowCount; i++)
    {
        m_rowOffsets[i + 1] = m_rowOffsets[i];
        m_rowOffsets[i + 1] += m_rowSpacings[i];

        if (m_rowSizes[i] > 0.f)
        {
            m_rowOffsets[i + 1] += m_rowSizes[i];
        }
        else
        {
            m_rowOffsets[i + 1] += m_rowSizes[i] * stretchSize.y;
        }
    }

    m_colOffsets[0] = 0.f;
    for (size_t i = 0; i < m_colCount; i++)
    {
        m_colOffsets[i + 1] = m_colOffsets[i];
        m_colOffsets[i + 1] += m_colSpacings[i];
        if (m_colSizes[i] > 0.f)
        {
            m_colOffsets[i + 1] += m_colSizes[i];
        }
        else
        {
            m_colOffsets[i + 1] += m_colSizes[i] * stretchSize.x;
        }
    }
}
