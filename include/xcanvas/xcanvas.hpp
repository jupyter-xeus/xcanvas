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

        inline const std::map<std::string, std::size_t>& get_attrs()
        {
            static std::map<std::string, std::size_t> attrs = {
                { "fill_style", 0 }, { "stroke_style", 1 }, { "global_alpha", 2 }, { "font", 3 }, { "text_align", 4 },
                { "text_baseline", 5 }, { "direction", 6 }, { "global_composite_operation", 7 },
                { "line_width", 8 }, { "line_cap", 9 }, { "line_join", 10 }, { "miter_limit", 11 }, { "line_dash_offset", 12 },
                { "shadow_offset_x", 13 }, { "shadow_offset_y", 14 }, { "shadow_blur", 15 }, { "shadow_color", 16 }
            };
            return attrs;
        }
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

        // Rect methods
        void fill_rect(double x, double y, double width);
        void fill_rect(double x, double y, double width, double height);
        void stroke_rect(double x, double y, double width);
        void stroke_rect(double x, double y, double width, double height);
        void clear_rect(double x, double y, double width);
        void clear_rect(double x, double y, double width, double height);

        // Arc methods
        void fill_arc(double x, double y, double radius, double start_angle, double end_angle, bool anticlockwise=false);
        void fill_circle(double x, double y, double radius);
        void stroke_arc(double x, double y, double radius, double start_angle, double end_angle, bool anticlockwise=false);
        void stroke_circle(double x, double y, double radius);

        // Polygon methods
        // TODO

        // Line methods
        void stroke_line(double x1, double y1, double x2, double y2);

        // Path methods
        void begin_path();
        void close_path();
        void stroke();
        void fill(const std::string& rule="nonzero");
        // void fill(const path2D& path);
        void move_to(double x, double y);
        void line_to(double x, double y);
        void rect(double x, double y, double width, double height);
        void arc(double x, double y, double radius, double start_angle, double end_angle, bool anticlockwise=false);
        void ellipse(double x, double y, double radius_x, double radius_y, double rotation, double start_angle, double end_angle, bool anticlockwise=false);
        void arc_to(double x1, double y1, double x2, double y2, double radius);
        void quadratic_curve_to(double cp1x, double cp1y, double x, double y);
        void bezier_curve_to(double cp1x, double cp1y, double cp2x, double cp2y, double x, double y);

        // Text methods
        // TODO

        // Line style methods
        // TODO

        // Image methods
        // TODO

        // Clip methods
        void clip();

        // Transform methods
        void save();
        void restore();
        void translate(double x, double y);
        void rotate(double angle);
        // TODO

        // Extras
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
        auto property_idx = p::get_attrs().find(name);
        if (property_idx != p::get_attrs().end())
        {
            send_command(nl::json::array({ p::COMMANDS::set, { property_idx->second, value } }));
        }
        else
        {
            base_type::notify(name, value);
        }
    }

    /*
     * Rect methods
     */

    template <class D>
    inline void xcanvas<D>::fill_rect(double x, double y, double width)
    {
        send_command(nl::json::array({ p::COMMANDS::fillRect, { x, y, width, width } }));
    }

    template <class D>
    inline void xcanvas<D>::fill_rect(double x, double y, double width, double height)
    {
        send_command(nl::json::array({ p::COMMANDS::fillRect, { x, y, width, height } }));
    }

    template <class D>
    inline void xcanvas<D>::stroke_rect(double x, double y, double width)
    {
        send_command(nl::json::array({ p::COMMANDS::strokeRect, { x, y, width, width } }));
    }

    template <class D>
    inline void xcanvas<D>::stroke_rect(double x, double y, double width, double height)
    {
        send_command(nl::json::array({ p::COMMANDS::strokeRect, { x, y, width, height } }));
    }

    template <class D>
    inline void xcanvas<D>::clear_rect(double x, double y, double width)
    {
        send_command(nl::json::array({ p::COMMANDS::clearRect, { x, y, width, width } }));
    }

    template <class D>
    inline void xcanvas<D>::clear_rect(double x, double y, double width, double height)
    {
        send_command(nl::json::array({ p::COMMANDS::clearRect, { x, y, width, height } }));
    }

    /*
     * Arc methods
     */

    template <class D>
    inline void xcanvas<D>::fill_arc(double x, double y, double radius, double start_angle, double end_angle, bool anticlockwise)
    {
        send_command(nl::json::array({ p::COMMANDS::fillArc, { x, y, radius, start_angle, end_angle, anticlockwise } }));
    }

    template <class D>
    inline void xcanvas<D>::fill_circle(double x, double y, double radius)
    {
        send_command(nl::json::array({ p::COMMANDS::fillCircle, { x, y, radius } }));
    }

    template <class D>
    inline void xcanvas<D>::stroke_arc(double x, double y, double radius, double start_angle, double end_angle, bool anticlockwise)
    {
        send_command(nl::json::array({ p::COMMANDS::strokeArc, { x, y, radius, start_angle, end_angle, anticlockwise } }));
    }

    template <class D>
    inline void xcanvas<D>::stroke_circle(double x, double y, double radius)
    {
        send_command(nl::json::array({ p::COMMANDS::strokeCircle, { x, y, radius } }));
    }

    /*
     * Line methods
     */

    template <class D>
    inline void xcanvas<D>::stroke_line(double x1, double y1, double x2, double y2)
    {
        send_command(nl::json::array({ p::COMMANDS::strokeLine, { x1, y1, x2, y2 } }));
    }

    /*
     * Path methods
     */

    template <class D>
    inline void xcanvas<D>::begin_path()
    {
        send_command(nl::json::array({ p::COMMANDS::beginPath }));
    }

    template <class D>
    inline void xcanvas<D>::close_path()
    {
        send_command(nl::json::array({ p::COMMANDS::closePath }));
    }

    template <class D>
    inline void xcanvas<D>::stroke()
    {
        send_command(nl::json::array({ p::COMMANDS::stroke }));
    }

    template <class D>
    inline void xcanvas<D>::fill(const std::string& rule)
    {
        send_command(nl::json::array({ p::COMMANDS::fill, { rule } }));
    }

    template <class D>
    inline void xcanvas<D>::move_to(double x, double y)
    {
        send_command(nl::json::array({ p::COMMANDS::moveTo, { x, y } }));
    }

    template <class D>
    inline void xcanvas<D>::line_to(double x, double y)
    {
        send_command(nl::json::array({ p::COMMANDS::lineTo, { x, y } }));
    }

    template <class D>
    inline void xcanvas<D>::rect(double x, double y, double width, double height)
    {
        send_command(nl::json::array({ p::COMMANDS::rect, { x, y, width, height } }));
    }

    template <class D>
    inline void xcanvas<D>::arc(double x, double y, double radius, double start_angle, double end_angle, bool anticlockwise)
    {
        send_command(nl::json::array({ p::COMMANDS::arc, { x, y, radius, start_angle, end_angle, anticlockwise } }));
    }

    template <class D>
    inline void xcanvas<D>::ellipse(double x, double y, double radius_x, double radius_y, double rotation, double start_angle, double end_angle, bool anticlockwise)
    {
        send_command(nl::json::array({ p::COMMANDS::ellipse, { x, y, radius_x, radius_y, rotation, start_angle, end_angle, anticlockwise } }));
    }

    template <class D>
    inline void xcanvas<D>::arc_to(double x1, double y1, double x2, double y2, double radius)
    {
        send_command(nl::json::array({ p::COMMANDS::arcTo, { x1, y1, x2, y2, radius } }));
    }

    template <class D>
    inline void xcanvas<D>::quadratic_curve_to(double cp1x, double cp1y, double x, double y)
    {
        send_command(nl::json::array({ p::COMMANDS::quadraticCurveTo, { cp1x, cp1y, x, y } }));
    }

    template <class D>
    inline void xcanvas<D>::bezier_curve_to(double cp1x, double cp1y, double cp2x, double cp2y, double x, double y)
    {
        send_command(nl::json::array({ p::COMMANDS::bezierCurveTo, { cp1x, cp1y, cp2x, cp2y, x, y } }));
    }

    /*
     * Clip methods
     */

    template <class D>
    inline void xcanvas<D>::clip()
    {
        send_command(nl::json::array({ p::COMMANDS::clip }));
    }

    /*
     * Transform methods
     */

    template <class D>
    inline void xcanvas<D>::save()
    {
        send_command(nl::json::array({ p::COMMANDS::save }));
    }

    template <class D>
    inline void xcanvas<D>::restore()
    {
        send_command(nl::json::array({ p::COMMANDS::restore }));
    }

    template <class D>
    inline void xcanvas<D>::translate(double x, double y)
    {
        send_command(nl::json::array({ p::COMMANDS::translate, { x, y } }));
    }

    template <class D>
    inline void xcanvas<D>::rotate(double angle)
    {
        send_command(nl::json::array({ p::COMMANDS::rotate, { angle } }));
    }

    /*
     * Extras
     */

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
