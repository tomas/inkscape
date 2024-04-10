#!/usr/bin/env python
# -*- coding: utf-8 -*-

import chardataeffect, inkex, string

convert_table = {\
'a': u'\u2801',
'c': u'\u2809',
'b': u'\u2803',
'e': u'\u2811',
'd': u'\u2819',
'g': u'\u281b',
'f': u'\u280b',
'i': u'\u280a',
'h': u'\u2813',
'k': u'\u2805',
'j': u'\u281a',
'm': u'\u280d',
'l': u'\u2807',
'o': u'\u2815',
'n': u'\u281d',
'q': u'\u281f',
'p': u'\u280f',
's': u'\u280e',
'r': u'\u2817',
'u': u'\u2825',
't': u'\u281e',
'w': u'\u283a',
'v': u'\u2827',
'y': u'\u283d',
'x': u'\u282d',
'z': u'\u2835',
}

class C(chardataeffect.CharDataEffect):

  def process_chardata(self,text, line, par):
    r = ""
    for c in text:
      if c.lower() in convert_table:
        r = r + convert_table[c.lower()]
      else:
        r = r + c
    return r

c = C()
c.affect()
