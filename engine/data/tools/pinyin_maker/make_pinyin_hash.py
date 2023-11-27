# coding=utf-8

import sys
import os


def load_keymap(filename):
    map_alpha_digit = {}
    f = open(filename)
    for each in f:
        items = each.split()
        if len(items) > 1 and items[0].isdigit() and (items[1].isalpha() or items[1].isdigit()):
            map_alpha_digit[items[1]] = items[0]

    return map_alpha_digit


def make_pinyin_group(keymap_filename, pinyin_filename, group_filename):
    map_alpha_digit = load_keymap(keymap_filename)
    map_pinyin_digit = {}
    map_digit = {}
    for each in map_alpha_digit:
        pinyin = each
        digit = map_alpha_digit[each]
        map_pinyin_digit[pinyin] = digit
        if digit not in map_digit:
            map_digit[digit] = 1
        else:
            map_digit[digit] += 1
        # print(pinyin, digit)

    f = open(pinyin_filename)
    for each in f:
        items = each.split()
        if len(items) > 0:
            pinyin = items[0]
            digit = ''
            for c in pinyin:
                digit += map_alpha_digit[c]

            if digit not in map_digit:
                map_digit[digit] = 1
            else:
                map_digit[digit] += 1
            map_pinyin_digit[pinyin] = digit
            # print(pinyin, digit)

    f.close()

    print(len(map_digit))

    set_digit = set()
    for each in map_digit:
        set_digit.add(int(each))

    lst_digit = sorted(set_digit)
    map_digit_idx = {}
    idx = 1
    for each in lst_digit:
        map_digit_idx[str(each)] = idx
        idx += 1

    f = open(group_filename, 'w')
    f.write('; ID' + '\t' + 'pinyin' + '\t' + 'digit' + '\n')
    for py_len in range(1, 7):
        for pinyin in map_pinyin_digit:
            if len(pinyin) == py_len:
                f.write(str(map_digit_idx[map_pinyin_digit[pinyin]]) + '\t' + pinyin + '\t' + map_pinyin_digit[pinyin] + '\n')
                print(pinyin, map_pinyin_digit[pinyin], map_digit_idx[map_pinyin_digit[pinyin]])
    f.close()



if __name__ == '__main__':

    if len(sys.argv) == 3:
        #make_src_code(sys.argv[1], sys.argv[2])
        pass
    else:

        make_pinyin_group("../text/group.txt", "../text/pinyin.txt", "../text/pinyin_group.txt")
        