/* vim:set et ts=4 sts=4:
 *
 * libpyzy - The Chinese PinYin and Bopomofo conversion library.
 *
 * Copyright (c) 2008-2010 Peng Huang <shawn.p.huang@gmail.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */
#ifndef __PYZY_PHRASE_H_
#define __PYZY_PHRASE_H_

#include <cstring>
#include "PyZyTypes.h"

namespace PyZy {

#define PHRASE_LEN_IN_BYTE (MAX_UTF8_LEN * (MAX_PHRASE_LEN + 1))

struct Phrase {
    gchar phrase[PHRASE_LEN_IN_BYTE];
    guint freq;
    guint user_freq;
    struct {
        guint8 sheng;
        guint8 yun;
    } pinyin_id[MAX_PHRASE_LEN];
    guint len;

    void reset (void)
    {
        phrase[0] = 0;
        freq = 0;
        user_freq = 0;
        len = 0;
    }

    gboolean empty (void) const
    {
        return len == 0;
    }

    Phrase & operator += (const Phrase & a)
    {
        g_assert (len + a.len <= MAX_PHRASE_LEN);
        g_strlcat (phrase, a.phrase, sizeof (phrase));
        std::memcpy (pinyin_id + len, a.pinyin_id, a.len << 1);
        len += a.len;
        return *this;
    }

    operator const gchar * (void) const
    {
        return phrase;
    }

};

};  // namespace PyZy

#endif  // __PYZY_PHRASE_H_
