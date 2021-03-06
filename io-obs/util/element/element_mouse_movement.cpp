/**
 * This file is part of input-overlay
 * which is licensed under the MPL 2.0 license
 * See LICENSE or mozilla.org/en-US/MPL/2.0/
 * github.com/univrsal/input-overlay
 */

#include "../../sources/input_source.hpp"
#include "../../../ccl/ccl.hpp"
#include "element_mouse_movement.hpp"
#include "util/layout_constants.hpp"
#include "util/util.hpp"

void element_mouse_movement::load(ccl_config* cfg, const std::string &id)
{
    element_texture::load(cfg, id);
    m_keycode = VC_MOUSE_DATA;
    m_radius = cfg->get_int(id + CFG_MOUSE_RADIUS);
    m_movement_type = cfg->get_int(id + CFG_MOUSE_TYPE) == 0 ? DOT : ARROW;
}

void element_mouse_movement::draw(gs_effect_t* effect, gs_image_file_t* image, element_data* data,
                                  sources::overlay_settings* settings)
{
    if (data && data->get_type() == MOUSE_STATS) {
        const auto element_data = dynamic_cast<element_data_mouse_stats*>(data);

        if (element_data) {
            if (m_movement_type == ARROW) {
                element_texture::draw(effect, image, &m_mapping, &m_pos, element_data->get_mouse_angle(settings));
            } else {
                element_data->get_mouse_offset(settings, m_pos, m_offset_pos, m_radius);
                element_texture::draw(effect, image, &m_mapping, &m_offset_pos);
            }
        }
    } else {
        element_texture::draw(effect, image, &m_mapping, &m_pos);
    }
}

element_data_mouse_stats::element_data_mouse_stats(const int16_t x, const int16_t y) : element_data(MOUSE_STATS)
{
    m_type = stat_pos;
    m_x = x;
    m_y = y;
}

element_data_mouse_stats::element_data_mouse_stats(const stat_type type) : element_data(MOUSE_STATS)
{
    m_type = type;
}

element_data_mouse_stats::element_data_mouse_stats(int scroll_amount, const wheel_direction dir, bool unused)
        : element_data(MOUSE_STATS)
{
    UNUSED_PARAMETER(unused);
    m_type = stat_scroll_amount;
    m_wheel_current = scroll_amount;
    m_dir = dir;
}

bool element_data_mouse_stats::is_persistent()
{
    return true;
}

void element_data_mouse_stats::merge(element_data* other)
{
    if (other && other->get_type() == MOUSE_STATS) {
        const auto data = dynamic_cast<element_data_mouse_stats*>(other);
        if (data) {
            switch (data->m_type) {
                case stat_pos:
                    m_last_x = m_x;
                    m_last_y = m_y;
                    m_x = data->m_x;
                    m_y = data->m_y;
                    break;
                case stat_scroll_amount:
                    if (data->m_wheel_current <= WHEEL_UP)
                        m_wheel_up_total += data->m_wheel_current;
                    else
                        m_wheel_down_total += data->m_wheel_current;
                    m_dir = data->m_dir;

                    if ((m_wheel_current < 0) == (data->m_wheel_current < 0))
                        m_wheel_current += data->m_wheel_current;
                    else
                        m_wheel_current = data->m_wheel_current;
                    break;
                case stat_lmb:
                    m_lmbcount_total++;
                    m_rmbcount_current = 0;
                    m_mmbcount_current = 0;
                    break;
                case stat_rmb:
                    m_lmbcount_current = 0;
                    m_rmbcount_total++;
                    m_mmbcount_current = 0;
                    break;
                case stat_mmb:
                    m_lmbcount_current = 0;
                    m_rmbcount_current = 0;
                    m_mmbcount_total++;
                    break;
                default:;
            }
        }
    }

}

float element_data_mouse_stats::get_mouse_angle(sources::overlay_settings* settings)
{
    auto d_x = 0, d_y = 0;

    if (settings->use_center) {
        d_x = m_x - settings->monitor_h;
        d_y = m_y - settings->monitor_w;
    } else {
        d_x = m_x - m_last_x;
        d_y = m_y - m_last_y;
    }

    const float new_angle = (0.5 * M_PI) + (atan2f(d_y, d_x));
    if (abs(d_x) < settings->mouse_deadzone || abs(d_y) < settings->mouse_deadzone) {
        /* Draw old angle (new movement was to minor) */
        return m_old_angle;
    }

    m_old_angle = new_angle;
    return new_angle;
}

void element_data_mouse_stats::get_mouse_offset(sources::overlay_settings* settings, const vec2 &center, vec2 &out,
                                                const uint8_t radius) const
{
    auto d_x = 0, d_y = 0;

    if (settings->use_center) {
        d_x = m_x - settings->monitor_h;
        d_y = m_y - settings->monitor_w;
    } else {
        d_x = m_x - m_last_x;
        d_y = m_y - m_last_y;

        if (abs(d_x) < settings->mouse_deadzone)
            d_x = 0;
        if (abs(d_y) < settings->mouse_deadzone)
            d_y = 0;
    }

    const auto factor_x = UTIL_CLAMP(-1, ((double) d_x / settings->mouse_sens), 1);
    const auto factor_y = UTIL_CLAMP(-1, ((double) d_y / settings->mouse_sens), 1);

    out.x = center.x + radius * factor_x;
    out.y = center.y + radius * factor_y;

}

uint32_t element_data_mouse_stats::get_mmb_total() const
{
    return m_mmbcount_total;
}

uint32_t element_data_mouse_stats::get_mmb_current() const
{
    return m_mmbcount_current;
}

uint32_t element_data_mouse_stats::get_lmb_total() const
{
    return m_lmbcount_total;
}

uint32_t element_data_mouse_stats::get_lmb_current() const
{
    return m_lmbcount_current;
}

uint32_t element_data_mouse_stats::get_rmb_total() const
{
    return m_rmbcount_total;
}

uint32_t element_data_mouse_stats::get_rmb_current() const
{
    return m_rmbcount_current;
}

int16_t element_data_mouse_stats::get_mouse_x() const
{
    return m_x;
}

int16_t element_data_mouse_stats::get_mouse_y() const
{
    return m_y;
}

int32_t element_data_mouse_stats::get_wheel_current() const
{
    return m_wheel_current;
}

int32_t element_data_mouse_stats::get_wheel_total() const
{
    switch (m_dir) {
        case WHEEL_DIR_DOWN:
            return m_wheel_down_total;
        case WHEEL_DIR_UP:
            return m_wheel_up_total;
        default:
            return 0;
    }
}

element_mouse_movement::element_mouse_movement() : element_texture(MOUSE_STATS), m_movement_type()
{
};
