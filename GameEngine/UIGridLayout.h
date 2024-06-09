/*
  Copyright (c) Arnaud BANNIER and Nicolas BODIN.
  Licensed under the MIT License.
  See LICENSE.md in the project root for license information.
*/

#pragma once

#include "Settings.h"
#include "UIObject.h"

class UIGridLayout : public UIObject
{
public:
    UIGridLayout(Scene *scene, int rowCount, int columnCount);
    void SetPadding(float paddingX, float paddingY);
    void SetPadding(float padding);
    void SetSpacing(float spacing);
    void SetAnchor(Anchor anchor);

    void AddObject(
        UIObject *object, int rowIdx, int columnIdx,
        int rowSpan = 1, int columnSpan = 1);
    void RemoveObject(UIObject *object);

    void SetRowSize(int index, float size);
    void SetRowSize(float size);
    void SetColumnSize(int index, float size);
    void SetColumnSize(float size);
    void SetRowSpacing(int index, float spacing);
    void SetRowSpacing(float spacing);
    void SetColumnSpacing(int index, float spacing);
    void SetColumnSpacing(float spacing);

    void SetDebugGizmos(bool debugGizmos);
    virtual void Update() override;
    virtual void DrawGizmos(Gizmos &gizmos) override;

protected:

    struct GridCell
    {
        GridCell(int rowIdx, int colIdx, int rowSpan, int colSpan):
            rowIdx(rowIdx), colIdx(colIdx),
            rowSpan(rowSpan), colSpan(colSpan)
        {}
        int rowIdx;
        int colIdx;
        int rowSpan;
        int colSpan;
    };

    int m_rowCount;
    int m_colCount;
    b2Vec2 m_padding;
    std::vector<float> m_rowSizes;
    std::vector<float> m_colSizes;
    std::vector<float> m_rowSpacings;
    std::vector<float> m_colSpacings;
    std::vector<float> m_rowOffsets;
    std::vector<float> m_colOffsets;
    std::map <UIObject *, GridCell> m_gridMap;
    Anchor m_anchor;

private:
    void UpdateOffsets();
    bool m_debugGizmos;
};

inline void UIGridLayout::SetPadding(float paddingX, float paddingY)
{
    m_padding.Set(paddingX, paddingY);
    Update();
}

inline void UIGridLayout::SetPadding(float padding)
{
    m_padding.Set(padding, padding);
    Update();
}

inline void UIGridLayout::SetAnchor(Anchor anchor)
{
    m_anchor = anchor;
    Update();
}

inline void UIGridLayout::SetDebugGizmos(bool debugGizmos)
{
    m_debugGizmos = debugGizmos;
}
