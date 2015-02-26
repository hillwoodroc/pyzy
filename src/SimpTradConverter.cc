/* vim:set et ts=4 sts=4:
 *
 * libpyzy - The Chinese PinYin and Bopomofo conversion library.
 *
 * Copyright (c) 2008-2010 Peng Huang <shawn.p.huang@gmail.com>
 * Copyright (c) 2010 BYVoid <byvoid1@gmail.com>
 * Copyright (c) 2015 Hillwood Yang <hillwood@opensuse.org>
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
#include "SimpTradConverter.h"

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#ifdef HAVE_OPENCC
#  include <opencc.h>
#else
#  include <cstring>
#  include <cstdlib>
#endif

#include "String.h"

namespace PyZy {

#ifdef HAVE_OPENCC

class opencc {
public:
    opencc (void)
    {
        m_od = opencc_open (OPENCC_DEFAULT_CONFIG_SIMP_TO_TRAD);
        g_assert (m_od != NULL);
    }

    ~opencc (void)
    {
        opencc_close(m_od);
    }

    void convert (const char *in, String &out)
    {
        char * converted = opencc_convert_utf8 (m_od, in, -1);
        g_assert (converted != NULL);
        out = converted;
        opencc_convert_utf8_free (converted);
    }
private:
    opencc_t m_od;
};

void
SimpTradConverter::simpToTrad (const char *in, String &out)
{
    static opencc opencc;
    opencc.convert (in, out);
}

#else

static int _xcmp (const char *p1, const char *p2, const char *str)
{
    for (;;) {
        // both reach end
        if (p1 == p2 && *str == '\0')
            return 0;
        // p1 reaches end
        if (p1 == p2)
            return -1;
        // str reaches end
        if (*str == '\0')
            return 1;

        if (*p1 < *str)
            return -1;
        if (*p1 > *str)
            return 1;

        p1 ++; str ++;
    };
}

static int _cmp (const void * p1, const void * p2)
{
    const char **pp = (const char **) p1;
    const char **s2 = (const char **) p2;

    return _xcmp (pp[0], pp[1], s2[0]);
}

#include "SimpTradConverterTable.h"

void
SimpTradConverter::simpToTrad (const char *in, String &out)
{
    const char *pend;
    const char *pp[2];
    size_t len;
    size_t begin;

    if (!g_utf8_validate (in, -1 , NULL)) {
        g_warning ("\%s\" is not an utf8 string!", in);
        g_assert_not_reached ();
    }

    begin = 0;
    pend = in + std::strlen (in);
    len = g_utf8_strlen (in, -1);   // length in charactoers
    pp[0] = in;

    while (pp[0] != pend) {
        size_t slen  = std::min (len - begin, static_cast<size_t>(SIMP_TO_TRAD_MAX_LEN)); // the length of sub string in character
        pp[1] = g_utf8_offset_to_pointer (pp[0], slen);    // the end of sub string

        for (;;) {
            const char **result;
            result = (const char **) std::bsearch (pp, simp_to_trad,
                                            G_N_ELEMENTS (simp_to_trad), sizeof (simp_to_trad[0]),
                                            _cmp);

            if (result != NULL) {
                // found item in table,
                // append the trad to out and adjust pointers
                out << result[1];
                pp[0] = pp[1];
                begin += slen;
                break;
            }

            if (slen == 1) {
                // if only one character left,
                // append origin character to out and adjust pointers
                out.append (pp[0], pp[1] - pp[0]);
                pp[0] = pp[1];
                begin += 1;
                break;
            }

            // if more than on characters left,
            // adjust pp[1] to previous character
            pp[1] = g_utf8_prev_char (pp[1]);
            slen--;
        }
    }
}
#endif  // HAVE_OPENCC

};  // namespace PyZy
