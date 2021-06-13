#include "ForgePch.h"
#include "Layer.h"

namespace Forge
{

    LayerStack::~LayerStack()
    {
        for (const std::unique_ptr<Layer>& layer : m_Layers)
            layer->OnDetach();
    }

    Layer& LayerStack::PushLayer(std::unique_ptr<Layer>&& layer)
    {
        Layer& l = *layer;
        m_Layers.emplace(m_Layers.begin() + m_InsertIndex, std::move(layer));
        m_InsertIndex++;
        return l;
    }

    Layer& LayerStack::PushOverlay(std::unique_ptr<Layer>&& layer)
    {
        return *m_Layers.emplace_back(std::move(layer));
    }

    void LayerStack::PopLayer(Layer* layer)
    {
        auto it = std::find_if(m_Layers.begin(), m_Layers.begin() + m_InsertIndex, [layer](const std::unique_ptr<Layer>& l)
        {
            return l.get() == layer;
        });
        if (it != m_Layers.begin() + m_InsertIndex)
        {
            layer->OnDetach();
            m_Layers.erase(it);
            m_InsertIndex--;
        }
    }

    void LayerStack::PopOverlay(Layer* layer)
    {
        auto it = std::find_if(m_Layers.begin(), m_Layers.begin() + m_InsertIndex, [layer](const std::unique_ptr<Layer>& l)
        {
            return l.get() == layer;
        });
        if (it != m_Layers.end())
        {
            layer->OnDetach();
            m_Layers.erase(it);
        }
    }

}
