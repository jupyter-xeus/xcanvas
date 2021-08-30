/******************************************************************************
* Copyright (c) 2021, Martin Renou                                            *
*                                                                             *
* Distributed under the terms of the BSD 3-Clause License.                    *
*                                                                             *
* The full license is in the file LICENSE, distributed with this software.    *
*******************************************************************************/

#ifndef XCANVAS_HPP
#define XCANVAS_HPP

#include <array>
#include <functional>
#include <list>
#include <string>
#include <utility>
#include <vector>

#include "xtl/xoptional.hpp"

#include "xwidgets/xcolor.hpp"
#include "xwidgets/xeither.hpp"
#include "xwidgets/xholder.hpp"
#include "xwidgets/xmaterialize.hpp"
#include "xwidgets/xobject.hpp"
#include "xwidgets/xstyle.hpp"
#include "xwidgets/xwidget.hpp"

#include "xcanvas_config.hpp"

namespace nl = nlohmann;

namespace xc
{
    /**********************
     * canvas declaration *
     **********************/

    template <class D>
    class xcanvas : public xw::xwidget<D>
    {
    public:

        using callback_type = std::function<void(const nl::json&)>;

        using base_type = xw::xwidget<D>;
        using derived_type = D;

        void serialize_state(nl::json&, xeus::buffer_sequence&) const;
        void apply_patch(const nl::json&, const xeus::buffer_sequence&);

        // void on_interaction(callback_type);

        // TODO

    protected:

        xcanvas();
        using base_type::base_type;

    private:

        // std::list<callback_type> m_interaction_callbacks;
    };

    using canvas = xw::xmaterialize<xcanvas>;

    /**************************
     * xcanvas implementation *
     **************************/

    template <class D>
    inline void xcanvas<D>::serialize_state(nl::json& state, xeus::buffer_sequence& buffers) const
    {
        base_type::serialize_state(state, buffers);

        using xw::xwidgets_serialize;

        // TODO
    }

    template <class D>
    inline void xcanvas<D>::apply_patch(const nl::json& patch, const xeus::buffer_sequence& buffers)
    {
        base_type::apply_patch(patch, buffers);

        using xw::set_property_from_patch;

        // TODO
    }

    // template <class D>
    // inline void xcanvas<D>::on_interaction(callback_type cb)
    // {
    //     m_interaction_callbacks.emplace_back(std::move(cb));
    // }

    template <class D>
    inline void xcanvas<D>::handle_custom_message(const nl::json& content)
    {
        // auto it = content.find("event");
        // if (it != content.end() && it.value() == "interaction")
        // {
        //     for (auto it = m_interaction_callbacks.begin(); it != m_interaction_callbacks.end(); ++it)
        //     {
        //         it->operator()(content);
        //     }
        // }
    }
}

/*********************
 * precompiled types *
 *********************/

#ifndef _WIN32
    extern template class xw::xmaterialize<xc::xcanvas>;
    extern template xw::xmaterialize<xc::xcanvas>::xmaterialize();
    extern template class xw::xtransport<xw::xmaterialize<xc::xcanvas>>;
#endif

#endif
