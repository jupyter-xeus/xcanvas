/******************************************************************************
* Copyright (c) 2021, Martin Renou                                            *
*                                                                             *
* Distributed under the terms of the BSD 3-Clause License.                    *
*                                                                             *
* The full license is in the file LICENSE, distributed with this software.    *
*******************************************************************************/

#ifndef XCANVAS_HPP
#define XCANVAS_HPP

#include <iostream>

#include <array>
#include <functional>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "xtl/xoptional.hpp"

#include "nlohmann/json.hpp"

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
    namespace p
    {
        enum COMMANDS {
            fillRect, strokeRect, fillRects, strokeRects, clearRect, fillArc,
            fillCircle, strokeArc, strokeCircle, fillArcs, strokeArcs,
            fillCircles, strokeCircles, strokeLine, beginPath, closePath,
            stroke, fillPath, fill, moveTo, lineTo,
            rect, arc, ellipse, arcTo, quadraticCurveTo,
            bezierCurveTo, fillText, strokeText, setLineDash, drawImage,
            putImageData, clip, save, restore, translate,
            rotate, scale, transform, setTransform, resetTransform,
            set, clear, sleep, fillPolygon, strokePolygon,
            strokeLines
        };

        std::map<std::string, std::size_t> ATTRS = {
            { "fill_style", 0 }, { "stroke_style", 1 }, { "global_alpha", 2 }, { "font", 3 }, { "text_align", 4 },
            { "text_baseline", 5 }, { "direction", 6 }, { "global_composite_operation", 7 },
            { "line_width", 8 }, { "line_cap", 9 }, { "line_join", 10 }, { "miter_limit", 11 }, { "line_dash_offset", 12 },
            { "shadow_offset_x", 13 }, { "shadow_offset_y", 14 }, { "shadow_blur", 15 }, { "shadow_color", 16 }
        };
    }

    /**********************
     * canvas declaration *
     **********************/

    template <class D>
    class xcanvas : public xw::xwidget<D>
    {
    public:

        using base_type = xw::xwidget<D>;
        using derived_type = D;

        void serialize_state(nl::json&, xeus::buffer_sequence&) const;
        void apply_patch(const nl::json&, const xeus::buffer_sequence&);

        template <class T>
        void notify(const std::string& name, const T& value);

        // TODO Understand why sending the client_ready message too
        // early kills xeus-cling
        XPROPERTY(bool, derived_type, _send_client_ready_event, false);

        XPROPERTY(int, derived_type, width, 700);
        XPROPERTY(int, derived_type, height, 500);
        XPROPERTY(bool, derived_type, sync_image_data, false);
        XPROPERTY(double, derived_type, global_alpha, 1.0);
        XPROPERTY(std::string, derived_type, font, "12px serif");
        XPROPERTY(
            std::string, derived_type, text_align, "start",
            XEITHER("start", "end", "left", "right", "center")
        );
        XPROPERTY(
            std::string, derived_type, text_baseline, "alphabetic",
            XEITHER("top", "hanging", "middle", "alphabetic", "ideographic", "bottom")
        );
        XPROPERTY(
            std::string, derived_type, direction, "inherit",
            XEITHER("ltr", "rtl", "inherit")
        );
        XPROPERTY(
            std::string, derived_type, global_composite_operation, "source-over",
            XEITHER(
                "source-over", "source-in", "source-out", "source-atop",
                "destination-over", "destination-in", "destination-out",
                "destination-atop", "lighter", "copy", "xor", "multiply",
                "screen", "overlay", "darken", "lighten", "color-dodge",
                "color-burn", "hard-light", "soft-light", "difference",
                "exclusion", "hue", "saturation", "color", "luminosity"
            )
        );
        XPROPERTY(double, derived_type, shadow_offset_x, 0.0);
        XPROPERTY(double, derived_type, shadow_offset_y, 0.0);
        XPROPERTY(double, derived_type, shadow_blur, 0.0);
        XPROPERTY(xw::html_color, derived_type, shadow_color, "rgba(0, 0, 0, 0)");
        XPROPERTY(double, derived_type, line_width, 1.0);
        XPROPERTY(
            std::string, derived_type, line_cap, "butt",
            XEITHER("butt", "round", "square")
        );
        XPROPERTY(
            std::string, derived_type, line_join, "miter",
            XEITHER("round", "bevel", "miter")
        );
        XPROPERTY(double, derived_type, miter_limit, 10.0);
        XPROPERTY(double, derived_type, line_dash_offset, 0.0);

        XPROPERTY(xw::html_color, derived_type, fill_style, "black");
        XPROPERTY(xw::html_color, derived_type, stroke_style, "black");

        void fill_rect(int x, int y, int width);
        void fill_rect(int x, int y, int width, int height);
        void stroke_rect(int x, int y, int width);
        void stroke_rect(int x, int y, int width, int height);

        void clear();

        void cache();
        void flush();

        // void handle_custom_message(const nl::json&);

    protected:

        xcanvas();
        using base_type::base_type;

    private:

        void send_command(const nl::json& command);

        nl::json m_commands;
        bool m_caching;
    };

    using canvas = xw::xmaterialize<xcanvas>;

    /**************************
     * xcanvas implementation *
     **************************/

    template <class D>
    inline xcanvas<D>::xcanvas()
        : base_type()
    {
        this->_model_module() = "ipycanvas";
        this->_view_module() = "ipycanvas";
        this->_model_name() = "CanvasModel";
        this->_view_name() = "CanvasView";
        this->_model_module_version() = jupyter_canvas_semver();
        this->_view_module_version() = jupyter_canvas_semver();

        m_commands = nl::json::array();
        m_caching = false;
    }

    template <class D>
    inline void xcanvas<D>::serialize_state(nl::json& state, xeus::buffer_sequence& buffers) const
    {
        base_type::serialize_state(state, buffers);

        using xw::xwidgets_serialize;

        xwidgets_serialize(_send_client_ready_event(), state["_send_client_ready_event"], buffers);

        xwidgets_serialize(width(), state["width"], buffers);
        xwidgets_serialize(height(), state["height"], buffers);
        xwidgets_serialize(sync_image_data(), state["sync_image_data"], buffers);
    }

    template <class D>
    inline void xcanvas<D>::apply_patch(const nl::json& patch, const xeus::buffer_sequence& buffers)
    {
        base_type::apply_patch(patch, buffers);

        using xw::set_property_from_patch;

        set_property_from_patch(width, patch, buffers);
        set_property_from_patch(height, patch, buffers);
        set_property_from_patch(sync_image_data, patch, buffers);
    }

    template <class D>
    template <class T>
    inline void xcanvas<D>::notify(const std::string& name, const T& value)
    {
        auto property_idx = p::ATTRS.find(name);
        if (property_idx != p::ATTRS.end())
        {
            send_command(nl::json::array({ p::COMMANDS::set, { property_idx->second, value } }));
        }
        else
        {
            base_type::notify(name, value);
        }
    }

    template <class D>
    inline void xcanvas<D>::fill_rect(int x, int y, int width)
    {
        send_command(nl::json::array({ p::COMMANDS::fillRect, { x, y, width, width } }));
    }

    template <class D>
    inline void xcanvas<D>::fill_rect(int x, int y, int width, int height)
    {
        send_command(nl::json::array({ p::COMMANDS::fillRect, { x, y, width, height } }));
    }

    template <class D>
    inline void xcanvas<D>::stroke_rect(int x, int y, int width)
    {
        send_command(nl::json::array({ p::COMMANDS::strokeRect, { x, y, width, width } }));
    }

    template <class D>
    inline void xcanvas<D>::stroke_rect(int x, int y, int width, int height)
    {
        send_command(nl::json::array({ p::COMMANDS::strokeRect, { x, y, width, height } }));
    }

    template <class D>
    inline void xcanvas<D>::clear()
    {
        send_command(nl::json::array({ p::COMMANDS::clear }));
    }

    template <class D>
    inline void xcanvas<D>::send_command(const nl::json& command)
    {
        m_commands.push_back(command);

        if (!m_caching)
        {
            flush();
        }
    }

    template <class D>
    inline void xcanvas<D>::cache()
    {
        m_caching = true;
    }

    template <class D>
    inline void xcanvas<D>::flush()
    {
        nl::json content;
        content["dtype"] = "uint8";

        std::string buffer_str = m_commands.dump();
        std::vector<char> buffer(buffer_str.begin(), buffer_str.end());

        this->send(std::move(content), { buffer });

        m_commands.clear();
        m_caching = false;
    }

    // template <class D>
    // inline void xcanvas<D>::handle_custom_message(const nl::json& content)
    // {
        // auto it = content.find("event");
        // if (it != content.end() && it.value() == "interaction")
        // {
        //     for (auto it = m_interaction_callbacks.begin(); it != m_interaction_callbacks.end(); ++it)
        //     {
        //         it->operator()(content);
        //     }
        // }
    // }
}

/*********************
 * precompiled types *
 *********************/

    extern template class xw::xmaterialize<xc::xcanvas>;
    extern template class xw::xtransport<xw::xmaterialize<xc::xcanvas>>;

#endif
